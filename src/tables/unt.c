/*****************************************************************************
 * unt.c: UNT decoder/generator
 *----------------------------------------------------------------------------
 * Copyright (C) 2001-2011 VideoLAN
 * $Id$
 *
 * Authors: somebody
 *          heavily based on pmt.c which was written by
 *          Arnaud de Bossoreille de Ribou <bozo@via.ecp.fr>
 *          Jean-Paul Saman <jpsaman@videolan.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *----------------------------------------------------------------------------
 *
 *****************************************************************************/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#include <assert.h>

#include "../dvbpsi.h"
#include "../dvbpsi_private.h"
#include "../psi.h"
#include "../descriptor.h"
#include "../demux.h"
#include "unt.h"
#include "unt_private.h"

/*****************************************************************************
 * dvbpsi_unt_attach
 *****************************************************************************
 * Initialize a UNT subtable decoder.
 *****************************************************************************/
bool dvbpsi_unt_attach(dvbpsi_t* p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,
                       dvbpsi_unt_callback pf_callback, void* p_cb_data)
{
    assert(p_dvbpsi);
    assert(p_dvbpsi->p_decoder);

    dvbpsi_demux_t* p_demux = (dvbpsi_demux_t*)p_dvbpsi->p_decoder;

    if (dvbpsi_demuxGetSubDec(p_demux, i_table_id, i_extension))
    {
        dvbpsi_error(p_dvbpsi, "UNT decoder",
                     "Already a decoder for (table_id == 0x%02x,"
                     "extension == 0x%02x)",
                     i_table_id, i_extension);
        return false;
    }

    dvbpsi_unt_decoder_t *p_unt_decoder;
    p_unt_decoder = (dvbpsi_unt_decoder_t *) dvbpsi_decoder_new(NULL,
                                                0, true, sizeof(dvbpsi_unt_decoder_t));
    if (p_unt_decoder == NULL)
        return false;

    /* subtable decoder configuration */
    dvbpsi_demux_subdec_t* p_subdec;
    p_subdec = dvbpsi_NewDemuxSubDecoder(i_table_id, i_extension, dvbpsi_unt_detach,
                                         dvbpsi_unt_sections_gather, DVBPSI_DECODER(p_unt_decoder));
    if (p_subdec == NULL)
    {
        dvbpsi_decoder_delete(DVBPSI_DECODER(p_unt_decoder));
        return false;
    }

    /* Attach the subtable decoder to the demux */
    int rc = dvbpsi_AttachDemuxSubDecoder(p_demux, p_subdec);
    if (rc != 0)
    {
        dvbpsi_decoder_delete(DVBPSI_DECODER(p_unt_decoder));
        return false;
    }

    /* UNT decoder information */
    p_unt_decoder->pf_unt_callback = pf_callback;
    p_unt_decoder->p_cb_data = p_cb_data;
    p_unt_decoder->p_building_unt = NULL;

    return true;
}

/*****************************************************************************
 * dvbpsi_unt_detach
 *****************************************************************************
 * Close a UNT decoder.
 *****************************************************************************/
void dvbpsi_unt_detach(dvbpsi_t* p_dvbpsi, uint8_t i_table_id,
                      uint16_t i_extension)
{
    assert(p_dvbpsi);
    assert(p_dvbpsi->p_decoder);

    dvbpsi_demux_t *p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
    dvbpsi_demux_subdec_t* p_subdec;

    i_extension = 0; /* NOTE: force to 0 when handling UNT */
    p_subdec = dvbpsi_demuxGetSubDec(p_demux, i_table_id, i_extension);
    if (p_subdec == NULL)
    {
        dvbpsi_error(p_dvbpsi, "UNT Decoder",
                     "No such UNT decoder (table_id == 0x%02x,"
                     "extension == 0x%02x)",
                     i_table_id, i_extension);
        return;
    }

    assert(p_subdec->p_decoder);

    dvbpsi_unt_decoder_t* p_unt_decoder;
    p_unt_decoder = (dvbpsi_unt_decoder_t*)p_subdec->p_decoder;
    if (p_unt_decoder->p_building_unt)
        dvbpsi_unt_delete(p_unt_decoder->p_building_unt);
    p_unt_decoder->p_building_unt = NULL;

    dvbpsi_DetachDemuxSubDecoder(p_demux, p_subdec);
    dvbpsi_DeleteDemuxSubDecoder(p_subdec);
}

/*****************************************************************************
 * dvbpsi_unt_init
 *****************************************************************************
 * Initialize a pre-allocated dvbpsi_unt_t structure.
 *****************************************************************************/
void dvbpsi_unt_init(dvbpsi_unt_t* p_unt, uint8_t i_table_id, uint16_t i_extension,
                     uint8_t i_version, bool b_current_next)
{
    assert(p_unt);

    p_unt->i_table_id = i_table_id;
    p_unt->i_extension = i_extension;

    p_unt->i_version = i_version;
    p_unt->b_current_next = b_current_next;

    p_unt->p_first_descriptor = NULL;
    p_unt->p_first_compatibility = NULL;
}

/*****************************************************************************
 * dvbpsi_unt_new
 *****************************************************************************
 * Allocate and Initialize a new dvbpsi_unt_t structure.
 *****************************************************************************/
dvbpsi_unt_t *dvbpsi_unt_new(uint8_t i_table_id, uint16_t i_extension, uint8_t i_version,
                             bool b_current_next, uint32_t oui, uint8_t processing_order)
{
  dvbpsi_unt_t *p_unt = (dvbpsi_unt_t*)malloc(sizeof(dvbpsi_unt_t));
  if (p_unt != NULL) {
        dvbpsi_unt_init(p_unt, i_table_id, i_extension, i_version, b_current_next);
	p_unt->i_action_type = (uint8_t)((i_extension >> 8) & 0xFF);
	p_unt->i_oui_hash = (uint8_t)(i_extension & 0xFF);
	p_unt->i_oui = oui;
	p_unt->i_processing_order = processing_order;
  }
  return p_unt;
}

/*****************************************************************************
 * dvbpsi_DeleteCompatibilityDescriptors
 *****************************************************************************
 * Clean a dvbpsi_unt_compatibility_descriptor_t structure.
 *****************************************************************************/
void dvbpsi_DeleteCompatibilityDescriptors(dvbpsi_unt_compatibility_descriptor_t* p_compatibility_descriptor)
{
	/* compatibility descriptor */
	while (p_compatibility_descriptor != NULL) {
		dvbpsi_unt_compatibility_descriptor_t *p_tmp = p_compatibility_descriptor->p_next;

		dvbpsi_DeleteDescriptors(p_compatibility_descriptor->p_first_descriptor);
		p_compatibility_descriptor->p_first_descriptor = NULL;
		
		free(p_compatibility_descriptor);
		p_compatibility_descriptor = p_tmp;
	}
}

/*****************************************************************************
 * dvbpsi_unt_empty
 *****************************************************************************
 * Clean a dvbpsi_unt_t structure.
 *****************************************************************************/
void dvbpsi_unt_empty(dvbpsi_unt_t* p_unt)
{
	dvbpsi_unt_compatibility_t *p_compatibility;
	dvbpsi_unt_platform_t *p_platform;

	dvbpsi_DeleteDescriptors(p_unt->p_first_descriptor);
	p_unt->p_first_descriptor = NULL;

	/* compatibility description */
	p_compatibility = p_unt->p_first_compatibility;
	while (p_compatibility != NULL) {
		dvbpsi_unt_compatibility_t *p_tmp = p_compatibility->p_next;

		/* compatibility descriptor */
		dvbpsi_DeleteCompatibilityDescriptors(p_compatibility->p_first_compatibility_descriptor);
		p_compatibility->p_first_compatibility_descriptor = NULL;

		/* platform description */
		p_platform = p_compatibility->p_first_platform;
		while (p_platform != NULL) {
			dvbpsi_unt_platform_t *p_platform_tmp = p_platform->p_next;

			/* target descriptor */
			dvbpsi_DeleteDescriptors(p_platform->p_first_target_descriptor);
			p_platform->p_first_target_descriptor = NULL;
			/* operational descriptor */
			dvbpsi_DeleteDescriptors(p_platform->p_first_operational_descriptor);
			p_platform->p_first_operational_descriptor = NULL;

			free(p_platform);
			p_platform = p_platform_tmp;
		}
		p_compatibility->p_first_platform = NULL;

		free(p_compatibility);
		p_compatibility = p_tmp;
	}
	p_unt->p_first_compatibility = NULL;
}

/*****************************************************************************
 * dvbpsi_unt_delete
 *****************************************************************************
 * Clean and Delete dvbpsi_unt_t structure.
 *****************************************************************************/
void dvbpsi_unt_delete(dvbpsi_unt_t* p_unt)
{
    if (p_unt)
        dvbpsi_unt_empty(p_unt);
    free(p_unt);
}

/*****************************************************************************
 * dvbpsi_unt_descriptor_add
 *****************************************************************************
 * Add a descriptor in the UNT.
 *****************************************************************************/
dvbpsi_descriptor_t* dvbpsi_unt_descriptor_add(dvbpsi_unt_t* p_unt,
                                               uint8_t i_tag, uint8_t i_length,
                                               uint8_t* p_data)
{
    dvbpsi_descriptor_t* p_descriptor = dvbpsi_NewDescriptor(i_tag, i_length, p_data);			
    if (p_descriptor == NULL)
        return NULL;

    p_unt->p_first_descriptor = dvbpsi_AddDescriptor(p_unt->p_first_descriptor, p_descriptor);
    assert(p_unt->p_first_descriptor);
    if (p_unt->p_first_descriptor == NULL)
        return NULL;

    return p_descriptor;
}

/*****************************************************************************
 * dvbpsi_unt_compatibility_add
 *****************************************************************************
 * Add an compatibility structure in the UNT.
 *****************************************************************************/
dvbpsi_unt_compatibility_t* dvbpsi_unt_compatibility_add (dvbpsi_unt_t* p_unt)
{
	dvbpsi_unt_compatibility_t* p_compatibility = 
		(dvbpsi_unt_compatibility_t *)malloc(sizeof(dvbpsi_unt_compatibility_t));
	if (NULL == p_compatibility)
		return NULL;

	p_compatibility->p_first_compatibility_descriptor = NULL;
	p_compatibility->p_first_platform = NULL;
	p_compatibility->p_next = NULL;

	if (p_unt->p_first_compatibility == NULL) {
		p_unt->p_first_compatibility = p_compatibility;
	} else {
		dvbpsi_unt_compatibility_t *p_last_compatibility = p_unt->p_first_compatibility;

		while(p_last_compatibility->p_next != NULL)
			p_last_compatibility = p_last_compatibility->p_next;

		p_last_compatibility->p_next = p_compatibility;
	}

	return p_compatibility;
}

/*****************************************************************************
 * dvbpsi_unt_compatibility_descriptor_add
 *****************************************************************************
 * Add an compatibility descriptor in the UNT.
 *****************************************************************************/
dvbpsi_unt_compatibility_descriptor_t* dvbpsi_unt_compatibility_descriptor_add (dvbpsi_unt_compatibility_t* p_compatibility)
{
	dvbpsi_unt_compatibility_descriptor_t *p_compatibility_descriptor =
		(dvbpsi_unt_compatibility_descriptor_t *)malloc(sizeof(dvbpsi_unt_compatibility_descriptor_t));
	if (NULL == p_compatibility_descriptor)
		return NULL;  /* out of memory */

	p_compatibility_descriptor->p_first_descriptor = NULL;
	p_compatibility_descriptor->p_next = NULL;

	if (p_compatibility->p_first_compatibility_descriptor == NULL) {
		p_compatibility->p_first_compatibility_descriptor = p_compatibility_descriptor;
	} else {
		dvbpsi_unt_compatibility_descriptor_t *p_last_compatibility_descriptor =
			p_compatibility->p_first_compatibility_descriptor;

		while(p_last_compatibility_descriptor->p_next != NULL)
			p_last_compatibility_descriptor = p_last_compatibility_descriptor->p_next;

		p_last_compatibility_descriptor->p_next = p_compatibility_descriptor;
	}

	return p_compatibility_descriptor;
}

/*****************************************************************************
 * dvbpsi_unt_platform_add
 *****************************************************************************
 * Add an platform descriptor in the UNT.
 *****************************************************************************/
dvbpsi_unt_platform_t* dvbpsi_unt_platform_add (dvbpsi_unt_compatibility_t* p_compatibility)
{
	dvbpsi_unt_platform_t *p_platform = (dvbpsi_unt_platform_t *)malloc(sizeof(dvbpsi_unt_platform_t));
	if (NULL == p_platform)
		return NULL;  /* out of memory */

	p_platform->p_first_target_descriptor = NULL;
	p_platform->p_first_operational_descriptor = NULL;
	p_platform->p_next = NULL;

	if (NULL == p_compatibility->p_first_platform) {
		p_compatibility->p_first_platform = p_platform;
	} else {
		dvbpsi_unt_platform_t *p_last_platform = p_compatibility->p_first_platform;

		while(p_last_platform->p_next != NULL)
		p_last_platform = p_last_platform->p_next;

		p_last_platform->p_next = p_platform;
	}
	return p_platform;
}

/*****************************************************************************
 * dvbpsi_unt_target_descriptor_add
 *****************************************************************************
 * Add a target descriptor in the UNT.
 *****************************************************************************/
dvbpsi_descriptor_t* dvbpsi_unt_target_descriptor_add (
	dvbpsi_unt_platform_t *p_platform,
	uint8_t i_tag,
	uint8_t i_length,
	uint8_t* p_data )
{
	dvbpsi_descriptor_t* p_descriptor = dvbpsi_NewDescriptor(i_tag, i_length, p_data);
	if (p_descriptor == NULL)
		return NULL;

	p_platform->p_first_target_descriptor = 
		dvbpsi_AddDescriptor(p_platform->p_first_target_descriptor, p_descriptor);
	assert(p_platform->p_first_target_descriptor);

	return p_descriptor;
}

/*****************************************************************************
 * dvbpsi_unt_operational_descriptor_add
 *****************************************************************************
 * Add a operational descriptor in the UNT.
 *****************************************************************************/
dvbpsi_descriptor_t* dvbpsi_unt_operational_descriptor_add (
	dvbpsi_unt_platform_t *p_platform,
	uint8_t i_tag,
	uint8_t i_length,
	uint8_t* p_data )
{
	dvbpsi_descriptor_t* p_descriptor = dvbpsi_NewDescriptor(i_tag, i_length, p_data);
	if (p_descriptor == NULL)
		return NULL;

	p_platform->p_first_operational_descriptor = 
		dvbpsi_AddDescriptor(p_platform->p_first_operational_descriptor, p_descriptor);
	assert(p_platform->p_first_operational_descriptor);

	return p_descriptor;
}

/* */
static void dvbpsi_ReInitUNT(dvbpsi_unt_decoder_t* p_decoder, const bool b_force)
{
    assert(p_decoder);

    dvbpsi_decoder_reset(DVBPSI_DECODER(p_decoder), b_force);

    /* Force redecoding */
    if (b_force)
    {
        /* Free structures */
        if (p_decoder->p_building_unt)
            dvbpsi_unt_delete(p_decoder->p_building_unt);
    }
    p_decoder->p_building_unt = NULL;
}

static bool dvbpsi_CheckUNT(dvbpsi_t *p_dvbpsi, dvbpsi_unt_decoder_t *p_unt_decoder,
                            dvbpsi_psi_section_t *p_section)
{
    bool b_reinit = false;
    assert(p_dvbpsi);
    assert(p_unt_decoder);

    if (p_unt_decoder->p_building_unt->i_extension != p_section->i_extension)
    {
        /* transport_stream_id */
        dvbpsi_error(p_dvbpsi, "UNT decoder",
                "'transport_stream_id' differs"
                " whereas no TS discontinuity has occured");
        b_reinit = true;
    }
    else if (p_unt_decoder->p_building_unt->i_version != p_section->i_version)
    {
        /* version_number */
        dvbpsi_error(p_dvbpsi, "UNT decoder",
                "'version_number' differs"
                " whereas no discontinuity has occured");
        b_reinit = true;
    }
    else if (p_unt_decoder->i_last_section_number != p_section->i_last_number)
    {
        /* last_section_number */
        dvbpsi_error(p_dvbpsi, "UNT decoder",
                "'last_section_number' differs"
                " whereas no discontinuity has occured");
        b_reinit = true;
    }

    return b_reinit;
}

static bool dvbpsi_AddSectionUNT(dvbpsi_t *p_dvbpsi, dvbpsi_unt_decoder_t *p_unt_decoder,
                                 dvbpsi_psi_section_t* p_section)
{
    assert(p_dvbpsi);
    assert(p_unt_decoder);
    assert(p_section);

    /* Initialize the structures if it's the first section received */
    if (!p_unt_decoder->p_building_unt)
    {
	p_unt_decoder->p_building_unt = dvbpsi_unt_new(
		p_section->i_table_id, p_section->i_extension,
		p_section->i_version, p_section->b_current_next,
		(p_section->p_payload_start[0] << 16) +
		(p_section->p_payload_start[1] << 8) +
		p_section->p_payload_start[2],
		p_section->p_payload_start[3]);

	if (p_unt_decoder->p_building_unt == NULL)
		return false;

        p_unt_decoder->i_last_section_number = p_section->i_last_number;
    }

    /* Add to linked list of sections */
    if (dvbpsi_decoder_psi_section_add(DVBPSI_DECODER(p_unt_decoder), p_section))
        dvbpsi_debug(p_dvbpsi, "UNT decoder", "overwrite section number %d",
                     p_section->i_number);

    return true;
}

/*****************************************************************************
 * dvbpsi_unt_sections_gather
 *****************************************************************************
 * Callback for the PSI decoder.
 *****************************************************************************/
void dvbpsi_unt_sections_gather(dvbpsi_t* p_dvbpsi,
                                dvbpsi_decoder_t* p_decoder,
                                dvbpsi_psi_section_t* p_section)
{
    assert(p_dvbpsi);
    assert(p_dvbpsi->p_decoder);

    const uint8_t i_table_id = p_section->i_table_id;

    if (!dvbpsi_CheckPSISection(p_dvbpsi, p_section, i_table_id, "UNT decoder"))
    {
        dvbpsi_DeletePSISections(p_section);
        return;
    }

    /* Valid UNT section */
    dvbpsi_unt_decoder_t* p_unt_decoder = (dvbpsi_unt_decoder_t*)p_decoder;

    /* TS discontinuity check */
    if (p_unt_decoder->b_discontinuity)
    {
        dvbpsi_ReInitUNT(p_unt_decoder, true);
        p_unt_decoder->b_discontinuity = false;
    }
    else
    {
        /* Perform a few sanity checks */
        if (p_unt_decoder->p_building_unt)
        {
            if (dvbpsi_CheckUNT(p_dvbpsi, p_unt_decoder, p_section))
                dvbpsi_ReInitUNT(p_unt_decoder, true);
        }
    }

    /* Add section to UNT */
    if (!dvbpsi_AddSectionUNT(p_dvbpsi, p_unt_decoder, p_section))
    {
        dvbpsi_error(p_dvbpsi, "UNT decoder", "failed decoding section %d",
                     p_section->i_number);
        dvbpsi_DeletePSISections(p_section);
        return;
    }

    /* Check if we have all the sections */
    if (dvbpsi_decoder_psi_sections_completed(DVBPSI_DECODER(p_unt_decoder)))
    {
        assert(p_unt_decoder->pf_unt_callback);

        /* Save the current information */
        p_unt_decoder->current_unt = *p_unt_decoder->p_building_unt;
        p_unt_decoder->b_current_valid = true;

        /* Decode the sections */
        dvbpsi_unt_sections_decode(p_dvbpsi, p_unt_decoder->p_building_unt,
                                   p_unt_decoder->p_sections);
        /* signal the new UNT */
        p_unt_decoder->pf_unt_callback(p_unt_decoder->p_cb_data,
                                       p_unt_decoder->p_building_unt);
        /* Delete sections and Reinitialize the structures */
        dvbpsi_ReInitUNT(p_unt_decoder, false);
        assert(p_unt_decoder->p_sections == NULL);
    }
}

/*****************************************************************************
 * dvbpsi_unt_section_valid
 *****************************************************************************
 * Check the CRC_32 if the section has b_syntax_indicator set.
 *****************************************************************************/
static bool dvbpsi_unt_section_valid(dvbpsi_t *p_dvbpsi, dvbpsi_psi_section_t* p_section)
{
    /* CRC32 has already been checked by dvbpsi_packet_push()
     * and by dvbpsi_BuildPSISection(). */
    return true;
}

/*****************************************************************************
 * dvbpsi_unt_sections_decode
 *****************************************************************************
 * UNT decoder.
 *****************************************************************************/
void dvbpsi_unt_sections_decode(dvbpsi_t* p_dvbpsi, dvbpsi_unt_t* p_unt,
                                dvbpsi_psi_section_t* p_section)
{
	while (p_section)
	{
		uint8_t *p_byte, *p_end, *p_end2, *section_end;
		uint16_t i_loop_length;
		uint16_t i_count;

		if (!dvbpsi_unt_section_valid(p_dvbpsi, p_section))
			break;

		section_end = p_section->p_payload_end;
		/* common descriptor loop */
		p_byte = p_section->p_payload_start + 4;
		i_loop_length = (((uint16_t)p_byte[0] & 0x0F) << 8) + p_byte[1];
		p_byte += 2;
		p_end  = p_byte + i_loop_length;

		while (p_byte + 2 <= p_end) {
			uint8_t i_tag = p_byte[0];
			uint8_t i_length = p_byte[1];

			if (i_length + 2 <= p_end - p_byte)
				dvbpsi_unt_descriptor_add(p_unt, i_tag, i_length, p_byte + 2);

			p_byte += 2 + i_length;
		}
		p_byte = p_end;
		/* */
		while ((p_byte + 4 /*CRC*/) < section_end) {
			dvbpsi_unt_compatibility_t *p_compatibility = NULL;
			dvbpsi_unt_platform_t* p_platform = NULL;
			
			/* ----------------------------- */
			/* compatibility descriptor loop */
			/* ----------------------------- */
			i_loop_length = ((uint16_t)p_byte[0] << 8) + p_byte[1];		
			i_count = ((uint16_t)p_byte[2] << 8) + p_byte[3];
	
			p_end = p_byte + i_loop_length + 2;
			p_byte += 4;
			if ((0 != i_count) && (0 != i_loop_length)) {
				p_compatibility = dvbpsi_unt_compatibility_add(p_unt);
				if (NULL == p_compatibility)
					break;  /* out of memory */		
			}
			while(p_byte + 11 <= p_end) {
				dvbpsi_unt_compatibility_descriptor_t* p_compatibility_descriptor =
					dvbpsi_unt_compatibility_descriptor_add(p_compatibility);
				if (NULL == p_compatibility_descriptor)
					break;  /* out of memory */

				/* compatibility descriptors */
				p_compatibility_descriptor->descriptorType = p_byte[0];
				p_compatibility_descriptor->descriptorLength = p_byte[1];
				p_compatibility_descriptor->specifierType = p_byte[2];
				p_compatibility_descriptor->specifierData = ((uint32_t)p_byte[3]<<16) + ((uint32_t)p_byte[4]<<8) + p_byte[5];
				p_compatibility_descriptor->model = ((uint16_t)p_byte[6]<<8) + p_byte[7];
				p_compatibility_descriptor->version = ((uint16_t)p_byte[8]<<8) + p_byte[9];
				p_compatibility_descriptor->subDescriptorCount = p_byte[10];
				if (p_compatibility_descriptor->subDescriptorCount)
					p_compatibility_descriptor->subDescriptorData = p_byte + 11;
				else
					p_compatibility_descriptor->subDescriptorData = NULL;

				p_compatibility_descriptor->p_first_descriptor = NULL;
				p_byte += p_compatibility_descriptor->descriptorLength + 2;
			}
			/* ------------------- */
			/* platform descriptor */
			/* ------------------- */
			p_byte = p_end;
			/* platform_loop_length */
			i_loop_length = ((uint16_t)p_byte[0]<<8) + p_byte[1];
			p_byte += 2;
			p_end2 = p_byte + i_loop_length;

			if (i_loop_length) {
				p_platform = dvbpsi_unt_platform_add(p_compatibility);
				if (NULL == p_platform)
					break;  /* out of memory */
			}
			
			/* ---------------------- */
			/* target descriptor loop */
			/* ---------------------- */
			i_loop_length = (((uint16_t)p_byte[0] & 0x0F) << 8) + p_byte[1];
			p_byte += 2;
			p_end  = p_byte + i_loop_length;
			while (p_byte + 2 <= p_end) {
				uint8_t i_tag = p_byte[0];
				uint8_t i_length = p_byte[1];

				if (i_length + 2 <= p_end - p_byte)
					dvbpsi_unt_target_descriptor_add(p_platform, i_tag, i_length, p_byte + 2);

				p_byte += 2 + i_length;
			}
			p_byte = p_end;

			/* --------------------------- */
			/* operational descriptor loop */
			/* --------------------------- */
			i_loop_length = (((uint16_t)p_byte[0] & 0x0F) << 8) + p_byte[1];
			p_byte += 2;
			p_end  = p_byte + i_loop_length;
			while (p_byte + 2 <= p_end) {
				uint8_t i_tag = p_byte[0];
				uint8_t i_length = p_byte[1];

				if (i_length + 2 <= p_end - p_byte)
					dvbpsi_unt_operational_descriptor_add(p_platform, i_tag, i_length, p_byte + 2);

				p_byte += 2 + i_length;
			}
			/* */
			p_end = p_end2;
			p_byte = p_end2;
		}
		/* next section */
		p_section = p_section->p_next;
	}
	
	return;
}

/*****************************************************************************
 * dvbpsi_unt_sections_generate
 *****************************************************************************
 * Generate UNT sections based on the dvbpsi_unt_t structure.
 *****************************************************************************/
dvbpsi_psi_section_t* dvbpsi_unt_sections_generate(dvbpsi_t *p_dvbpsi, dvbpsi_unt_t* p_unt)
{
    dvbpsi_psi_section_t* p_result;
    dvbpsi_descriptor_t* p_descriptor = p_unt->p_first_descriptor;

    /* If it has descriptors, it must be a UNT, otherwise a UNT */
    p_result = dvbpsi_NewPSISection((p_descriptor != NULL) ? 4096 : 8);

    p_result->i_table_id = 0x4B;
    p_result->b_syntax_indicator = false;
    p_result->b_private_indicator = false;
    p_result->i_length = 5;
    p_result->p_payload_start = p_result->p_data + 3;
    p_result->p_payload_end = p_result->p_data + 8;

    if (!dvbpsi_unt_section_valid(p_dvbpsi, p_result))
    {
        dvbpsi_error(p_dvbpsi, "UNT generator", "********************************************");
        dvbpsi_error(p_dvbpsi, "UNT generator", "*  Generated UNT section is invalid.   *");
        dvbpsi_error(p_dvbpsi, "UNT generator", "* THIS IS A BUG, PLEASE REPORT TO THE LIST *");
        dvbpsi_error(p_dvbpsi, "UNT generator", "*  ---  libdvbpsi-devel@videolan.org  ---  *");
        dvbpsi_error(p_dvbpsi, "UNT generator", "********************************************");
    }

    return p_result;
}

/*

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

user_dr_93.c

User-defined Logical Channel Number Descriptor.

*/

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

#include "../../dvbpsi.h"
#include "../../dvbpsi_private.h"
#include "../../descriptor.h"

#include "user_dr_93.h"


/*****************************************************************************
 * dvbpsi_DecodeUserLogicalChannelNumberDr
 *****************************************************************************/
dvbpsi_user_lcn_dr_t *
dvbpsi_DecodeUserLogicalChannelNumberDr (dvbpsi_descriptor_t * p_descriptor)
{
    dvbpsi_user_lcn_dr_t *p_decoded;
    uint8_t *buf = p_descriptor->p_data;

    /* Check the tag */
    if (!dvbpsi_CanDecodeAsDescriptor(p_descriptor, 0x93))
        return NULL;

    /* Don't decode twice */
    if (dvbpsi_IsDescriptorDecoded(p_descriptor))
        return p_descriptor->p_decoded;

    /* Check length */
    if (p_descriptor->i_length < 4)
        return NULL;

    if (p_descriptor->i_length % 4)
        return NULL;

    /* Allocate memory */
    p_decoded = (dvbpsi_user_lcn_dr_t *)
            malloc (sizeof (dvbpsi_user_lcn_dr_t));
    if (!p_decoded)
        return NULL;

    memset (p_decoded, 0, sizeof (dvbpsi_user_lcn_dr_t));
    p_descriptor->p_decoded = (void *) p_decoded;

    p_decoded->i_total = p_descriptor->i_length / 4;
    for (int i=0; i < p_decoded->i_total; i++) {
    	p_decoded->pair[i].sid = ((uint16_t)buf[i*4 + 0] << 2) + (uint16_t)buf[i*4 + 1];
    	p_decoded->pair[i].lcn = ((uint16_t)buf[i*4 + 2] << 2) + (uint16_t)buf[i*4 + 3];
    }

    return p_decoded;
}

/*****************************************************************************
 * dvbpsi_GenUserLogicalChannelNumberDr
 *****************************************************************************/
dvbpsi_descriptor_t* dvbpsi_GenUserLogicalChannelNumberDr(
                                        dvbpsi_user_lcn_dr_t* p_decoded,
                                        bool b_duplicate)
{
    uint8_t i_desc_length = p_decoded->i_total * 4;
    dvbpsi_descriptor_t* p_descriptor = dvbpsi_NewDescriptor(0x93, i_desc_length, NULL);
    if (!p_descriptor)
        return NULL;

    uint8_t* p_data = p_descriptor->p_data;

    for (int i=0; i < p_decoded->i_total; i++) {
	p_data[i*4 + 0] = (p_decoded->pair[i].sid >> 8) & 0xFF;
	p_data[i*4 + 1] = p_decoded->pair[i].sid & 0xFF ;
	p_data[i*4 + 2] = (p_decoded->pair[i].lcn >> 8) & 0xFF;
	p_data[i*4 + 3] = p_decoded->pair[i].lcn & 0xFF;
    }

    if (b_duplicate) {
        p_descriptor->p_decoded = dvbpsi_DuplicateDecodedDescriptor(p_decoded,
                                         sizeof(dvbpsi_user_lcn_dr_t));
    }

    return p_descriptor;
}

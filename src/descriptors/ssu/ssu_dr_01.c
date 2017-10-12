/*****************************************************************************
 * ssu_dr_03.c
 * Copyright (C) 2001-2012 VideoLAN
 * $Id: dr_03.c,v 1.4 2003/07/25 20:20:40 fenrir Exp $
 *
 * Authors: somebody
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

#include "../../dvbpsi.h"
#include "../../dvbpsi_private.h"
#include "../../descriptor.h"

#include "ssu_dr_01.h"

#define DESCRIPTOR_TAG		0x01

/*****************************************************************************
 * dvbpsi_DecodeSsuSchedulingDr
 *****************************************************************************/
dvbpsi_ssu_scheduling_dr_t* dvbpsi_DecodeSsuSchedulingDr(dvbpsi_descriptor_t * p_descriptor)
{
  dvbpsi_ssu_scheduling_dr_t * p_decoded;
  uint8_t *p_data;
  uint8_t length;

  /* Check the tag */
  if (!dvbpsi_CanDecodeAsDescriptor(p_descriptor, DESCRIPTOR_TAG))
     return NULL;

  /* Don't decode twice */
  if (dvbpsi_IsDescriptorDecoded(p_descriptor))
     return p_descriptor->p_decoded;

  /* Allocate memory */
  p_decoded = (dvbpsi_ssu_scheduling_dr_t *)malloc(sizeof(dvbpsi_ssu_scheduling_dr_t));
  if(!p_decoded) return NULL;

  /* Decode data and check the length */
  if(p_descriptor->i_length == 0) {
    free(p_decoded);
    return NULL;
  }

  p_data = p_descriptor->p_data;
  length = p_descriptor->i_length;
  p_decoded->i_start_date_time = ((uint64_t)p_data[0] << 32) +
  	((uint64_t)p_data[1] << 24) + ((uint64_t)p_data[2] << 16) +
	((uint64_t)p_data[3] << 8) +  p_data[4];
  p_decoded->i_end_date_time = ((uint64_t)p_data[5] << 32) +
  	((uint64_t)p_data[6] << 24) + ((uint64_t)p_data[7] << 16) +
	((uint64_t)p_data[8] << 8) +  p_data[9];

  p_decoded->b_final_availability = (p_data[10] & (0x1 << 7)) ? true : false;	
  p_decoded->b_periodicity_flag = (p_data[10] & (0x1 << 6)) ? true : false;
  p_decoded->i_period_unit = ((p_data[10] & (0x3 << 4)) >> 4);
  p_decoded->i_duration_unit = ((p_data[10] & (0x3 << 2)) >> 2);
  p_decoded->i_estimated_cycle_time_unit = (p_data[10] & 0x3);
  p_decoded->i_period = p_data[11];
  p_decoded->i_duration = p_data[12];
  p_decoded->i_estimated_cycle_time = p_data[13];  
  p_data += 14;
  length -= 14;
  
  p_decoded->i_private_data_byte_length = length;
  if (length)
	memcpy (p_decoded->p_private_data_byte, p_data, length);
  
  p_descriptor->p_decoded = (void*)p_decoded;

  return p_decoded;
}


/*****************************************************************************
 * dvbpsi_GenSsuSchedulingDr
 *****************************************************************************/
dvbpsi_descriptor_t *dvbpsi_GenSsuSchedulingDr(dvbpsi_ssu_scheduling_dr_t * p_decoded,
                                         bool b_duplicate)
{
    /* Create the descriptor */
    dvbpsi_descriptor_t *p_descriptor = dvbpsi_NewDescriptor(DESCRIPTOR_TAG, 1, NULL);
    if (!p_descriptor)
        return NULL;

    /* Encode data */

    return p_descriptor;
}


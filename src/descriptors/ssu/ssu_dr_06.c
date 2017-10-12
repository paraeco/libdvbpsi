/*****************************************************************************
 * ssu_dr_06.c
 * Copyright (C) 2001-2012 VideoLAN
 * $Id: 
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

#include "ssu_dr_06.h"

#define DESCRIPTOR_TAG		0x06

/*****************************************************************************
 * dvbpsi_DecodeSsuTargetSmartcardDr
 *****************************************************************************/
dvbpsi_ssu_target_smartcard_dr_t * dvbpsi_DecodeSsuTargetSmartcardDr(dvbpsi_descriptor_t * p_descriptor)
{
  dvbpsi_ssu_target_smartcard_dr_t * p_decoded;
  uint8_t *p_data;
  uint8_t length;

  /* Check the tag */
  if (!dvbpsi_CanDecodeAsDescriptor(p_descriptor, DESCRIPTOR_TAG))
     return NULL;

  /* Don't decode twice */
  if (dvbpsi_IsDescriptorDecoded(p_descriptor))
     return p_descriptor->p_decoded;

  /* Allocate memory */
  p_decoded = (dvbpsi_ssu_target_smartcard_dr_t *)malloc(sizeof(dvbpsi_ssu_target_smartcard_dr_t));
  if(!p_decoded) return NULL;

  /* Decode data and check the length */
  if(p_descriptor->i_length < 4)
  {
    free(p_decoded);
    return NULL;
  }

  p_data = p_descriptor->p_data;
  length = p_descriptor->i_length;
  p_decoded->i_super_ca_system_id = ((uint32_t)p_data[0] << 24) +
  	((uint32_t)p_data[1] << 16) + ((uint32_t)p_data[2] << 8) + (uint32_t)p_data[0];
  p_data ++;
  length --;
  
  p_decoded->i_private_data_byte_length = length;
  if (length)
	memcpy (p_decoded->p_private_data_byte, p_data, length);  

  p_descriptor->p_decoded = (void*)p_decoded;

  return p_decoded;
}


/*****************************************************************************
 * dvbpsi_GenSsuTargetSmartcardDr
 *****************************************************************************/
dvbpsi_descriptor_t *dvbpsi_GenSsuTargetSmartcardDr(dvbpsi_ssu_target_smartcard_dr_t * p_decoded,
                                         bool b_duplicate)
{
    /* Create the descriptor */
    dvbpsi_descriptor_t *p_descriptor = dvbpsi_NewDescriptor(DESCRIPTOR_TAG, 1, NULL);
    if (!p_descriptor)
        return NULL;

    /* Encode data */

    return p_descriptor;
}


/*****************************************************************************
 * ssu_dr_0a.c
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

#include "ssu_dr_0a.h"

#define DESCRIPTOR_TAG		0x0A

/*****************************************************************************
 * dvbpsi_DecodeSsuTargetIpv6AddressDr
 *****************************************************************************/
dvbpsi_ssu_target_ipv6_address_dr_t * dvbpsi_DecodeSsuTargetIpv6AddressDr(dvbpsi_descriptor_t * p_descriptor)
{
  dvbpsi_ssu_target_ipv6_address_dr_t * p_decoded;
  uint8_t *p_data;
  uint8_t length;

  /* Check the tag */
  if (!dvbpsi_CanDecodeAsDescriptor(p_descriptor, DESCRIPTOR_TAG))
     return NULL;

  /* Don't decode twice */
  if (dvbpsi_IsDescriptorDecoded(p_descriptor))
     return p_descriptor->p_decoded;

  /* Allocate memory */
  p_decoded = (dvbpsi_ssu_target_ipv6_address_dr_t *)malloc(sizeof(dvbpsi_ssu_target_ipv6_address_dr_t));
  if(!p_decoded) return NULL;

  /* Decode data and check the length */
  if(p_descriptor->i_length < 16)
  {
    free(p_decoded);
    return NULL;
  }

  p_data = p_descriptor->p_data;
  length = p_descriptor->i_length;
  p_decoded->i_ipv6_addr_mask[0] = p_data[0];
  p_decoded->i_ipv6_addr_mask[1] = p_data[1];
  p_decoded->i_ipv6_addr_mask[2] = p_data[2];
  p_decoded->i_ipv6_addr_mask[3] = p_data[3];
  p_decoded->i_ipv6_addr_mask[4] = p_data[4];
  p_decoded->i_ipv6_addr_mask[5] = p_data[5];
  p_decoded->i_ipv6_addr_mask[6] = p_data[6];
  p_decoded->i_ipv6_addr_mask[7] = p_data[7];
  p_decoded->i_ipv6_addr_mask[8] = p_data[8];
  p_decoded->i_ipv6_addr_mask[9] = p_data[9];
  p_decoded->i_ipv6_addr_mask[0xA] = p_data[0xA];
  p_decoded->i_ipv6_addr_mask[0xB] = p_data[0xB];
  p_decoded->i_ipv6_addr_mask[0xC] = p_data[0xC];
  p_decoded->i_ipv6_addr_mask[0xD] = p_data[0xD];
  p_decoded->i_ipv6_addr_mask[0xE] = p_data[0xE];
  p_decoded->i_ipv6_addr_mask[0xF] = p_data[0xF];

  p_data += 16;
  length -= 16;

  p_decoded->i_ipv6_addr_match_length = length;
  if (length)
	memcpy (p_decoded->p_ipv6_addr_match, p_data, length);  

  p_descriptor->p_decoded = (void*)p_decoded;

  return p_decoded;
}


/*****************************************************************************
 * dvbpsi_GenSsuTargetIpv6AddressDr
 *****************************************************************************/
dvbpsi_descriptor_t *dvbpsi_GenSsuTargetIpv6AddressDr(dvbpsi_ssu_target_ipv6_address_dr_t * p_decoded,
                                         bool b_duplicate)
{
    /* Create the descriptor */
    dvbpsi_descriptor_t *p_descriptor = dvbpsi_NewDescriptor(DESCRIPTOR_TAG, 1, NULL);
    if (!p_descriptor)
        return NULL;

    /* Encode data */

    return p_descriptor;
}


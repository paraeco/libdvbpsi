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

user_dr_d0.c

User Series Descriptor.

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

#include "user_dr_d0.h"


/*****************************************************************************
 * dvbpsi_DecodeUserSeriesDr
 *****************************************************************************/
dvbpsi_user_series_dr_t *
dvbpsi_DecodeUserSeriesDr (dvbpsi_descriptor_t * p_descriptor)
{
    dvbpsi_user_series_dr_t *p_decoded;
    uint8_t *buf = p_descriptor->p_data;

    /* Check the tag */
    if (!dvbpsi_CanDecodeAsDescriptor(p_descriptor, 0xd0))
        return NULL;

    /* Don't decode twice */
    if (dvbpsi_IsDescriptorDecoded(p_descriptor))
        return p_descriptor->p_decoded;

    /* Check length */
    if (p_descriptor->i_length < 2)
        return NULL;

    /* Allocate memory */
    p_decoded = (dvbpsi_user_series_dr_t *)
            malloc (sizeof (dvbpsi_user_series_dr_t));
    if (!p_decoded)
        return NULL;

    memset (p_decoded, 0, sizeof (dvbpsi_user_series_dr_t));
    p_descriptor->p_decoded = (void *) p_decoded;

    p_decoded->i_name_length = p_descriptor->i_length - 2;
    if (p_decoded->i_name_length)
        memcpy (p_decoded->i_name, buf, p_decoded->i_name_length);

    buf += p_decoded->i_name_length;
    p_decoded->i_number = ((uint16_t)buf[0] << 5) + (buf[1] >> 3);
    p_decoded->i_status = (buf[1] >> 1) & 0x03;
    p_decoded->b_last = (buf[1] & 0x01) ? true : false;

    return p_decoded;
}

/*****************************************************************************
 * dvbpsi_GenUserSeriesDr
 *****************************************************************************/
dvbpsi_descriptor_t* dvbpsi_GenUserSeriesDr(
                                        dvbpsi_user_series_dr_t* p_decoded,
                                        bool b_duplicate)
{
    uint8_t i_desc_length = p_decoded->i_name_length + 2;
    dvbpsi_descriptor_t* p_descriptor = dvbpsi_NewDescriptor(0xd0, i_desc_length, NULL);
    if (!p_descriptor)
        return NULL;

    uint8_t* p_data = p_descriptor->p_data;
    
    if (0 != p_decoded->i_name_length)
        memcpy(p_data, p_decoded->i_name, p_decoded->i_name_length);
    
    p_data += p_decoded->i_name_length;
    p_data[0] = (uint8_t)((p_decoded->i_number >> 5) & 0xFF);
    p_data[1] = (uint8_t)((p_decoded->i_number << 3) & 0xFF);
    p_data[1] |= (p_decoded->i_status & 0x03);
    if (p_decoded->b_last) 
        p_data[1] |= 0x01;
    else
        p_data[1] &= 0xFE;

    if (b_duplicate) {
        p_descriptor->p_decoded = dvbpsi_DuplicateDecodedDescriptor(p_decoded,
                                         sizeof(dvbpsi_user_series_dr_t));
    }

    return p_descriptor;
}

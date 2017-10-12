/*****************************************************************************
 * unt_private.h: private UNT structures
 *----------------------------------------------------------------------------
 * Copyright (C) 2001-2011 VideoLAN
 * $Id$
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
 *----------------------------------------------------------------------------
 *
 *****************************************************************************/

#ifndef _DVBPSI_UNT_PRIVATE_H_
#define _DVBPSI_UNT_PRIVATE_H_

/*****************************************************************************
 * dvbpsi_unt_decoder_t
 *****************************************************************************
 * UNT decoder.
 *****************************************************************************/
typedef struct dvbpsi_unt_decoder_s
{
    DVBPSI_DECODER_COMMON

    dvbpsi_unt_callback           pf_unt_callback;
    void *                        p_cb_data;

    /* */
    dvbpsi_unt_t                  current_unt;
    dvbpsi_unt_t                  *p_building_unt;

} dvbpsi_unt_decoder_t;


/*****************************************************************************
 * dvbpsi_unt_sections_gather
 *****************************************************************************
 * Callback for the PSI decoder.
 *****************************************************************************/
void dvbpsi_unt_sections_gather(dvbpsi_t* p_dvbpsi,
                              dvbpsi_decoder_t* p_decoder,
                              dvbpsi_psi_section_t* p_section);

/*****************************************************************************
 * dvbpsi_unt_sections_decode
 *****************************************************************************
 * UNTdecoder.
 *****************************************************************************/
void dvbpsi_unt_sections_decode(dvbpsi_t* p_dvbpsi, dvbpsi_unt_t* p_unt,
                              dvbpsi_psi_section_t* p_section);

#endif

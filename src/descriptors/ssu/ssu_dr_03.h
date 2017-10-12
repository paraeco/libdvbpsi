/*****************************************************************************
 * ssu_dr_03.h
 * Copyright (C) 2001-2012 VideoLAN
 * $Id: ssu_dr_03.h,v 1.2 2002/05/10 23:50:36 bozo Exp $
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

/*!
 * \file <ssu_dr_03.h>
 * \author somebody
 * \brief Application interface for the SSU location descriptor
 * decoder and generator.
 *
 * Application interface for the SSU location descriptor
 * decoder and generator. This descriptor's definition can be found in
 * TS 102 006
 */

#ifndef _DVBPSI_SSU_DR_03_H_
#define _DVBPSI_SSU_DR_03_H_

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * dvbpsi_ssu_location_dr_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_ssu_location_dr_s
 * \brief SSU location descriptor structure.
 *
 * This structure is used to store a decoded SSU location descriptor.
 * (TS 102 006).
 */
/*!
 * \typedef struct dvbpsi_ssu_location_dr_s dvbpsi_ssu_location_dr_t
 * \brief dvbpsi_ssu_location_dr_t type definition.
 */
typedef struct dvbpsi_ssu_location_dr_s
{
  uint16_t	i_data_broadcast_id;                   /*!< data_broadcast_id */
  uint16_t	i_association_tag;                     /*!< association_tag */
  uint8_t       i_private_data_byte_length;            /*!< length of private_data_byte */
  uint8_t       p_private_data_byte[256];              /*!< p_private_data_byte */

} dvbpsi_ssu_location_dr_t;


/*****************************************************************************
 * dvbpsi_DecodeSsuLocationDr
 *****************************************************************************/
/*!
 * \fn dvbpsi_ssu_location_dr_t * dvbpsi_DecodeSsuLocationDr(
                                        dvbpsi_descriptor_t * p_descriptor)
 * \brief SSU location descriptor decoder.
 * \param p_descriptor pointer to the descriptor structure
 * \return a pointer to a new SSU location descriptor structure which
 * contains the decoded data.
 */
dvbpsi_ssu_location_dr_t* dvbpsi_DecodeSsuLocationDr(dvbpsi_descriptor_t * p_descriptor);


/*****************************************************************************
 * dvbpsi_GenSsuLocationDr
 *****************************************************************************/
/*!
 * \fn dvbpsi_descriptor_t * dvbpsi_GenSsuLocationDr(
                        dvbpsi_ssu_location_dr_t * p_decoded, bool b_duplicate)
 * \brief SSU location descriptor generator.
 * \param p_decoded pointer to a decoded SSU location descriptor structure
 * \param b_duplicate if true then duplicate the p_decoded structure into
 * the descriptor
 * \return a pointer to a new descriptor structure which contains encoded data.
 */
dvbpsi_descriptor_t * dvbpsi_GenSsuLocationDr(dvbpsi_ssu_location_dr_t * p_decoded,
                                          bool b_duplicate);


#ifdef __cplusplus
}
#endif

#endif


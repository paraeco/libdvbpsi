/*****************************************************************************
 * ssu_dr_09.h
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

/*!
 * \file <ssu_dr_09.h>
 * \author somebody
 * \brief Application interface for the SSU target IP address descriptor
 * decoder and generator.
 *
 * Application interface for the SSU target IP address descriptor
 * decoder and generator. This descriptor's definition can be found in
 * TS 102 006
 */

#ifndef _DVBPSI_SSU_DR_09_H_
#define _DVBPSI_SSU_DR_09_H_

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * dvbpsi_ssu_target_ip_address_dr_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_ssu_target_ip_address_dr_s
 * \brief SSU target IP address descriptor structure.
 *
 * This structure is used to store a decoded SSU target IP address descriptor.
 * (TS 102 006).
 */
/*!
 * \typedef struct dvbpsi_ssu_target_ip_address_dr_s dvbpsi_ssu_target_ip_address_dr_t
 * \brief dvbpsi_ssu_target_ip_address_dr_t type definition.
 */
typedef struct dvbpsi_ssu_target_ip_address_dr_s
{
  uint8_t	i_ip_addr_mask[4];                 /*!< IP_addr_mask */
  uint8_t       i_ip_addr_match_length;            /*!< length of ip_addr_match */
  uint8_t       p_ip_addr_match[256];              /*!< ip_addr_match */

} dvbpsi_ssu_target_ip_address_dr_t;


/*****************************************************************************
 * dvbpsi_DecodeSsuTargetIpAddressDr
 *****************************************************************************/
/*!
 * \fn dvbpsi_ssu_target_ip_address_dr_t * dvbpsi_DecodeSsuTargetIpAddressDr(
                                        dvbpsi_descriptor_t * p_descriptor)
 * \brief SSU target IP address descriptor decoder.
 * \param p_descriptor pointer to the descriptor structure
 * \return a pointer to a new SSU target IP address descriptor structure which
 * contains the decoded data.
 */
dvbpsi_ssu_target_ip_address_dr_t* dvbpsi_DecodeSsuTargetIpAddressDr(dvbpsi_descriptor_t * p_descriptor);


/*****************************************************************************
 * dvbpsi_GenSsuTargetIpAddressDr
 *****************************************************************************/
/*!
 * \fn dvbpsi_descriptor_t * dvbpsi_GenSsuTargetIpAddressDr(
                        dvbpsi_ssu_target_ip_address_dr_t * p_decoded, bool b_duplicate)
 * \brief SSU target IP address descriptor generator.
 * \param p_decoded pointer to a decoded SSU target IP address descriptor structure
 * \param b_duplicate if true then duplicate the p_decoded structure into the descriptor
 * \return a pointer to a new descriptor structure which contains encoded data.
 */
dvbpsi_descriptor_t * dvbpsi_GenSsuTargetIpAddressDr(dvbpsi_ssu_target_ip_address_dr_t * p_decoded,
                                          bool b_duplicate);


#ifdef __cplusplus
}
#endif

#endif


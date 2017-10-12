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

user_dr_93.h

User-defined Logical Channel Number Descriptor.

*/

/*!
 * \file user_dr_93.h
 * \author 
 * \brief Decode Logical Channel Number Descriptor.
 */

#ifndef _DVBPSI_USER_DR_93_H_
#define _DVBPSI_USER_DR_93_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * dvbpsi_user_lcn_dr_s
 *****************************************************************************/
/*!
 * \struct dvbpsi_user_lcn_dr_s
 * \brief User-defined Logical Channel Number descriptor structure.
 *
 * This structure is used to store a decoded user-defined Logical Channel Number
 * descriptor.
 */
/*!
 * \typedef struct dvbpsi_user_lcn_dr_s dvbpsi_user_lcn_dr_t
 * \brief dvbpsi_user_lcn_dr_t type definition.
 */
typedef struct dvbpsi_user_lcn_dr_s
{
    uint8_t i_total;  /*!< number of [sid, lcn] pairs*/
    struct {
    	uint16_t sid;  /* service ID */
	uint16_t lcn;  /* logical channel number */ 
    } pair[64];
} dvbpsi_user_lcn_dr_t;


/*****************************************************************************
 * dvbpsi_DecodeUserLogicalChannelNumberDr
 *****************************************************************************/
/*!
 * \fn dvbpsi_user_lcn_dr_t * dvbpsi_DecodeUserLogicalChannelNumberDr(
                                        dvbpsi_descriptor_t * p_descriptor)
 * \brief User-defined logical channel number descriptor decoder.
 * \param p_descriptor pointer to the descriptor structure
 * \return a pointer to a new user-defined logical channel number descriptor structure
 * which contains the decoded data.
 */
dvbpsi_user_lcn_dr_t
    *dvbpsi_DecodeUserLogicalChannelNumberDr (dvbpsi_descriptor_t * p_descriptor);

/*****************************************************************************
 * dvbpsi_GenUserLogicalChannelNumberDr
 *****************************************************************************/
/*!
 * \fn dvbpsi_descriptor_t * dvbpsi_GenUserLogicalChannelNumberDr(
                        dvbpsi_user_series_dr_t * p_decoded, bool b_duplicate)
 * \brief User-defined logical channel number descriptor generator.
 * \param p_decoded pointer to a decoded user-defined logical channel number descriptor
 * structure
 * \param b_duplicate if true then duplicate the p_decoded structure into
 * the descriptor
 * \return a pointer to a new descriptor structure which contains encoded data.
 */
dvbpsi_descriptor_t* dvbpsi_GenUserLogicalChannelNumberDr(
                                        dvbpsi_user_lcn_dr_t* p_decoded,
                                        bool b_duplicate);


#ifdef __cplusplus
}
#endif

#endif

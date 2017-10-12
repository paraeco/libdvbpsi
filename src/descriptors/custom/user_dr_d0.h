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

user_dr_d0.h

User-defined Series Descriptor.

*/

/*!
 * \file user_dr_d0.h
 * \author 
 * \brief Decode Series Descriptor.
 */

#ifndef _DVBPSI_USER_DR_D0_H_
#define _DVBPSI_USER_DR_D0_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * dvbpsi_user_series_dr_s
 *****************************************************************************/
/*!
 * \struct dvbpsi_user_series_dr_s
 * \brief User-defined series descriptor structure.
 *
 * This structure is used to store a decoded user-defined series
 * descriptor.
 */
/*!
 * \typedef struct dvbpsi_user_series_dr_s dvbpsi_user_series_dr_t
 * \brief dvbpsi_user_series_dr_t type definition.
 */
typedef struct dvbpsi_user_series_dr_s
{
    uint8_t i_name_length;  /*!< length of the i_name array*/
    uint8_t i_name[256];  /*!< episode name */
    uint16_t i_number;  /*!< episode number */
    uint8_t i_status;  /*!< content status, 0=normal, 1=premier, 2=finale */
    bool b_last;  /*!< indicates the last episode */

} dvbpsi_user_series_dr_t;


/*****************************************************************************
 * dvbpsi_DecodeUserSeriesDr
 *****************************************************************************/
/*!
 * \fn dvbpsi_user_series_dr_t * dvbpsi_DecodeUserSeriesDr(
                                        dvbpsi_descriptor_t * p_descriptor)
 * \brief User-defined series descriptor decoder.
 * \param p_descriptor pointer to the descriptor structure
 * \return a pointer to a new user-defined series descriptor structure
 * which contains the decoded data.
 */
dvbpsi_user_series_dr_t
    *dvbpsi_DecodeUserSeriesDr (dvbpsi_descriptor_t * p_descriptor);

/*****************************************************************************
 * dvbpsi_GenUserSeriesDr
 *****************************************************************************/
/*!
 * \fn dvbpsi_descriptor_t * dvbpsi_GenUserSeriesDr(
                        dvbpsi_user_series_dr_t * p_decoded, bool b_duplicate)
 * \brief User-defined series descriptor generator.
 * \param p_decoded pointer to a decoded user-defined series descriptor
 * structure
 * \param b_duplicate if true then duplicate the p_decoded structure into
 * the descriptor
 * \return a pointer to a new descriptor structure which contains encoded data.
 */
dvbpsi_descriptor_t* dvbpsi_GenUserSeriesDr(
                                        dvbpsi_user_series_dr_t* p_decoded,
                                        bool b_duplicate);


#ifdef __cplusplus
}
#endif

#endif

/*****************************************************************************
 * unt.h
 * Copyright (C) 2001-2011 VideoLAN
 * $Id$
 *
 * Authors:
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
 * \file <unt.h>
 * \author
 * \brief Application interface for the UNT decoder and the UNT generator.
 *
 * Application interface for the UNT(Update Notification Table)
 * decoder and the UNT generator. New decoded UNT tables are sent by
 * callback to the application.
 */

#ifndef _DVBPSI_UNT_H_
#define _DVBPSI_UNT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	UNT_COMMON_DESCRIPTOR,
	UNT_TARGET_DESCRIPTOR,
	UNT_OPERATIONAL_DESCRIPTOR
} unt_descriptor_type_t;

/*****************************************************************************
 * dvbpsi_unt_platform_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_unt_platform_s
 * \brief UNT platform structure.
 *
 * This structure is used to store a decoded UNT platform description.
 * (ETSI TS 102 006).
 */
/*!
 * \typedef struct dvbpsi_unt_platform_s dvbpsi_unt_platform_t
 * \brief dvbpsi_unt_platform_t type definition.
 */
typedef struct dvbpsi_unt_platform_s
{
  dvbpsi_descriptor_t *p_first_target_descriptor;	/*!< target descriptor list */
  dvbpsi_descriptor_t *p_first_operational_descriptor;	/*!< operational descriptor list */ 
  struct dvbpsi_unt_platform_s *p_next;			/*!< next element of the list */

} dvbpsi_unt_platform_t;

/*****************************************************************************
 * dvbpsi_unt_subdescriptor_s
 *****************************************************************************/
/*!
 * \struct dvbpsi_unt_subdescriptor_s
 * \brief UNT subdescriptor structure.
 *
 * This structure is used to store a decoded UNT subdescriptor descriptor.
 * (ISO/IEC 13818-6 section 6.1).
 */
/*!
 * \typedef struct dvbpsi_unt_subdescriptor_s dvbpsi_unt_subdescriptor_t
 * \brief dvbpsi_unt_subdescriptor_s type definition.
 */
typedef struct dvbpsi_unt_subdescriptor_s
{
  uint8_t type;					/*!< subdescriptor type */
  uint8_t length;				/*!< */
  uint8_t *value;				/*!< */

  struct dvbpsi_unt_subdescriptor_s *p_next;	/*!< next element of the list */

} dvbpsi_unt_subdescriptor_t;

/*****************************************************************************
 * dvbpsi_unt_compatibility_descriptor_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_unt_compatibility_descriptor_s
 * \brief UNT compatibility descriptor structure.
 *
 * This structure is used to store a decoded UNT compatibility descriptor.
 * (ISO/IEC 13818-6 section 6.1).
 */
/*!
 * \typedef struct dvbpsi_unt_compatibility_descriptor_s dvbpsi_unt_compatibility_descriptor_t
 * \brief dvbpsi_unt_compatibility_descriptor_t type definition.
 */
typedef struct dvbpsi_unt_compatibility_descriptor_s
{
  uint8_t descriptorType;			/*!< descriptor type */
  uint8_t descriptorLength;
  uint8_t specifierType;			/*!< */
  uint32_t specifierData;			/*!< */
  uint16_t model;
  uint16_t version;
  uint8_t subDescriptorCount;
  /* subdescriptor */
  uint8_t *subDescriptorData;
  dvbpsi_descriptor_t *p_first_descriptor;	/*!< subdescriptor list */
  /* next */
  struct dvbpsi_unt_compatibility_descriptor_s *p_next;		/*!< next element of the list */

} dvbpsi_unt_compatibility_descriptor_t;

/*****************************************************************************
 * dvbpsi_unt_compatibility_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_unt_compatibility_s
 * \brief UNT compatibility structure.
 *
 * This structure is used to store a decoded UNT compatibility descriptor.
 * (ISO/IEC 13818-6 section 6.1).
 */
/*!
 * \typedef struct dvbpsi_unt_compatibility_s dvbpsi_unt_compatibility_t
 * \brief dvbpsi_unt_compatibility_t type definition.
 */
typedef struct dvbpsi_unt_compatibility_s
{
  uint16_t compatibilityDescriptorLength;
  uint16_t descriptorCount;
  dvbpsi_unt_compatibility_descriptor_t *p_first_compatibility_descriptor;	/*!< compatibility descriptor list */
  dvbpsi_unt_platform_t *p_first_platform;		 			/*!< platform description list */
  /* next */
  struct dvbpsi_unt_compatibility_s *p_next;		/*!< next element of the list */

} dvbpsi_unt_compatibility_t;

/*****************************************************************************
 * dvbpsi_unt_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_unt_s
 * \brief UNT structure.
 *
 * This structure is used to store a decoded UNT.
 * (TS 102 006).
 */
/*!
 * \typedef struct dvbpsi_unt_s dvbpsi_unt_t
 * \brief dvbpsi_unt_t type definition.
 */
typedef struct dvbpsi_unt_s
{
    uint8_t                   i_table_id;         /*!< table id */
    uint16_t                  i_extension;        /*!< subtable id */

    /* Subtable specific */
    uint8_t                   i_version;          /*!< version_number */
    bool                      b_current_next;     /*!< current_next_indicator */

    uint8_t                  i_action_type;
    uint8_t                  i_oui_hash;
    uint32_t                 i_oui;
    uint8_t                  i_processing_order;

    dvbpsi_descriptor_t *p_first_descriptor;				/*!< common descriptor list */
    dvbpsi_unt_compatibility_t *p_first_compatibility;		 	/*!< compatibility description list */
    #if 0
    uint32_t			i_crc;
    #endif

} __attribute__((packed)) dvbpsi_unt_t;

/*****************************************************************************
 * dvbpsi_unt_callback
 *****************************************************************************/
/*!
 * \typedef void (* dvbpsi_unt_callback)(void* p_cb_data,
                                         dvbpsi_unt_t* p_new_unt)
 * \brief Callback type definition.
 */
typedef void (* dvbpsi_unt_callback)(void* p_cb_data, dvbpsi_unt_t* p_new_unt);

/*****************************************************************************
 * dvbpsi_unt_attach
 *****************************************************************************/
/*!
 * \fn bool dvbpsi_unt_attach(dvbpsi_t* p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,
                            dvbpsi_unt_callback pf_callback, void* p_cb_data)
 * \brief Creation and initialization of a UNT decoder.
 * \param p_dvbpsi dvbpsi handle pointing to Subtable demultiplexor to which the decoder is attached.
 * \param i_table_id Table ID, usually 0x70
 * \param i_extension Table ID extension, unused in the UNT
 * \param pf_callback function to call back on new UNT.
 * \param p_cb_data private data given in argument to the callback.
 * \return true on success, false on failure
 */
bool dvbpsi_unt_attach(dvbpsi_t* p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,
                       dvbpsi_unt_callback pf_callback, void* p_cb_data);

/*****************************************************************************
 * dvbpsi_unt_detach
 *****************************************************************************/
/*!
 * \fn int dvbpsi_unt_detach(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension)
 * \brief Destroy a UNT decoder.
 * \param p_dvbpsi Subtable demultiplexor to which the decoder is attached.
 * \param i_table_id Table ID, usually 0x70
 * \param i_extension Table ID extension, unused in the UNT
 * \return nothing.
 */
void dvbpsi_unt_detach(dvbpsi_t* p_dvbpsi, uint8_t i_table_id,
                      uint16_t i_extension);

/*****************************************************************************
 * dvbpsi_unt_init/dvbpsi_unt_new
 *****************************************************************************/
/*!
 * \fn void dvbpsi_unt_init(dvbpsi_unt_t* p_unt, uint8_t i_table_id, uint16_t i_extension,
                            uint8_t i_version, bool b_current_next, uint64_t i_utc_time);
 * \brief Initialize a user-allocated dvbpsi_unt_t structure.
 * \param p_unt pointer to the UNT structure
 * \param i_table_id Table ID, usually 0x70
 * \param i_extension Table ID extension, unused in the UNT
 * \param i_version SDT version
 * \param b_current_next current next indicator
 * \return nothing.
 */
void dvbpsi_unt_init(dvbpsi_unt_t* p_unt, uint8_t i_table_id, uint16_t i_extension,
                     uint8_t i_version, bool b_current_next);

/*!
 * \fn dvbpsi_unt_t *dvbpsi_unt_new(uint8_t i_table_id, uint16_t i_extension,
                            uint8_t i_version, bool b_current_next, uint64_t i_utc_time);
 * \brief Allocate and initialize a new dvbpsi_unt_t structure.
 * \param i_table_id Table ID, usually 0x70
 * \param i_extension Table ID extension, unused in the UNT
 * \param i_version SDT version
 * \param b_current_next current next indicator
 * \param oui OUI
 * \param processing_order processing order
 * \return p_unt pointer to the UNT structure
 */
dvbpsi_unt_t *dvbpsi_unt_new(uint8_t i_table_id, uint16_t i_extension, uint8_t i_version,
                             bool b_current_next, uint32_t oui, uint8_t processing_order);

/*****************************************************************************
 * dvbpsi_unt_empty/dvbpsi_unt_delete
 *****************************************************************************/
/*!
 * \fn void dvbpsi_unt_empty(dvbpsi_unt_t* p_unt)
 * \brief Clean a dvbpsi_unt_t structure.
 * \param p_unt pointer to the UNT structure
 * \return nothing.
 */
void dvbpsi_unt_empty(dvbpsi_unt_t* p_unt);

/*!
 * \fn dvbpsi_unt_delete(dvbpsi_unt_t* p_unt)
 * \brief Clean and free a dvbpsi_unt_t structure.
 * \param p_unt pointer to the UNT structure
 * \return nothing.
 */
void dvbpsi_unt_delete(dvbpsi_unt_t* p_unt);

/*****************************************************************************
 * dvbpsi_unt_descriptor_add
 *****************************************************************************/
/*!
 * \fn dvbpsi_descriptor_t* dvbpsi_unt_descriptor_add(dvbpsi_unt_t* p_unt,
                                                      uint8_t i_tag,
                                                      uint8_t i_length,
                                                      uint8_t* p_data)
 * \brief Add a descriptor in the TOT.
 * \param p_unt pointer to the TOT structure
 * \param i_tag descriptor's tag
 * \param i_length descriptor's length
 * \param p_data descriptor's data
 * \return a pointer to the added descriptor.
 */
dvbpsi_descriptor_t* dvbpsi_unt_descriptor_add(dvbpsi_unt_t* p_unt,
                                               uint8_t i_tag, uint8_t i_length,
                                               uint8_t* p_data);

/*****************************************************************************
 * dvbpsi_unt_compatibility_add
 *****************************************************************************/
/*!
 * \fn dvbpsi_unt_compatibility_t* dvbpsi_unt_compatibility_add(dvbpsi_unt_t* p_unt)
 * \brief Add an compatibility structure in the UNT.
 * \param p_unt pointer to the UNT structure
 * \return a pointer to the added compatibility structure.
 */
dvbpsi_unt_compatibility_t* dvbpsi_unt_compatibility_add (dvbpsi_unt_t* p_unt);

dvbpsi_unt_compatibility_descriptor_t* dvbpsi_unt_compatibility_descriptor_add (dvbpsi_unt_compatibility_t* p_compatibility);

void dvbpsi_DeleteCompatibilityDescriptors(dvbpsi_unt_compatibility_descriptor_t* p_compatibility_descriptor);

/*****************************************************************************
 * dvbpsi_unt_platform_add
 *****************************************************************************/
/*!
 * \fn dvbpsi_unt_platform_t* dvbpsi_unt_platform_add (dvbpsi_unt_compatibility_t* p_compatibility)
 * \brief Add an platform structure in the dvbpsi_unt_compatibility_t structure.
 * \param p_compatibility pointer to the dvbpsi_unt_compatibility_t structure
 * \return a pointer to the added platform structure.
 */
dvbpsi_unt_platform_t* dvbpsi_unt_platform_add (dvbpsi_unt_compatibility_t* p_compatibility);

dvbpsi_descriptor_t* dvbpsi_unt_target_descriptor_add (
	dvbpsi_unt_platform_t *p_platform,
	uint8_t i_tag,
	uint8_t i_length,
	uint8_t* p_data );

dvbpsi_descriptor_t* dvbpsi_unt_operational_descriptor_add (
	dvbpsi_unt_platform_t *p_platform,
	uint8_t i_tag,
	uint8_t i_length,
	uint8_t* p_data );

/*****************************************************************************
 * dvbpsi_unt_sections_generate
 *****************************************************************************/
/*!
 * \fn dvbpsi_psi_section_t* dvbpsi_unt_sections_generate(dvbpsi_t *p_dvbpsi, dvbpsi_unt_t* p_unt)
 * \brief UNT generator
 * \param p_dvbpsi handle to dvbpsi with attached decoder
 * \param p_unt UNT structure
 * \return a pointer to the list of generated PSI sections.
 *
 * Generate UNT sections based on the dvbpsi_unt_t structure.
 */
dvbpsi_psi_section_t* dvbpsi_unt_sections_generate(dvbpsi_t* p_dvbpsi, dvbpsi_unt_t* p_unt);

#ifdef __cplusplus
}
#endif

#endif

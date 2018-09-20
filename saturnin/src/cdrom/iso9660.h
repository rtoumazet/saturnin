// 
// iso9660.h
// Saturnin
//
// Copyright (c) 2018 Renaud Toumazet
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file:	iso9660.h
///
/// \brief: Structures encapsulating a CD's TOC informations
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace saturnin {
namespace cdrom {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct AspiTocTrack
///
/// \brief  Aspi TOC Track data structure.
///
/// \author Runik
/// \date   08/03/2011
////////////////////////////////////////////////////////////////////////////////////////////////////

struct AspiTocTrack
{
    uint8_t reserved;
    uint8_t adr_ctrl;
    uint8_t trackno;
    uint8_t reserved1;
    uint8_t addr[4];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct AspiToc
///
/// \brief  Aspi TOC data structure.
///
/// \author Runik
/// \date   08/03/2011
////////////////////////////////////////////////////////////////////////////////////////////////////

struct AspiToc
{
    uint8_t size[2];
    uint8_t first;
    uint8_t last;
    AspiTocTrack track[100];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	VolumeDateTime
///
/// \brief	VolumeDateTime data structure.
///
/// \author	Runik
/// \date	08/03/2011
////////////////////////////////////////////////////////////////////////////////////////////////////

struct VolumeDateTime
{
    uint8_t year;     ///< Number of years since 1900
    uint8_t month;    ///< Month of the year from 1 to 12
    uint8_t day;      ///< Day of the Month from 1 to 31
    uint8_t hour;     ///< Hour of the day from 0 to 23
    uint8_t minute;   ///< Minute of the hour from 0 to 59
    uint8_t second;   ///< second of the minute from 0 to 59
    int8_t  zone;     ///< Offset from Greenwich Mean Time in number of 15 minute intervals from -48(West) to +52(East)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	DirectoryRecord
///
/// \brief	DirectoryRecord data structure.
///
/// \author	Runik
/// \date	08/03/2011
////////////////////////////////////////////////////////////////////////////////////////////////////

struct DirectoryRecord
{
    uint8_t directory_record_length;        ///< The directory record legnth
    uint8_t extended_attribute_record_length;  ///< Bytes - this field refers to the
                                            ///< Extended Attribute Record, which provides
                                            ///< additional information about a file to
                                            ///< systems that know how to use it. Since
                                            ///< few systems use it, we will not discuss
                                            ///< it here. Refer to ISO 9660:1988 for
                                            ///< more information.
    int64_t location_of_extent;             ///< This is the Logical Block Number of the
                                            ///< first Logical Block allocated to the file
    int64_t data_length;                    ///< Length of the file section in bytes
    VolumeDateTime recording_date_and_time;    ///<
    uint8_t file_flags;                     ///< One Byte, each bit of which is a Flag:
                                            ///< bit
                                            ///< 0 File is Hidden if this bit is 1
                                            ///< 1 Entry is a Directory if this bit is 1
                                            ///< 2 Entry is an Associated file is this bit is 1
                                            ///< 3 Information is structured according to
                                            ///<   the extended attribute record if this
                                            ///<   bit is 1
                                            ///< 4 Owner, group and permissions are
                                            ///<   specified in the extended attribute
                                            ///<   record if this bit is 1
                                            ///< 5 Reserved (0)
                                            ///< 6 Reserved (0)
                                            ///< 7 File has more than one directory record
                                            ///<   if this bit is 1
    uint8_t file_unit_size;                 ///< This field is only valid if the file is
                                            ///< recorded in interleave mode
                                            ///< Otherwise this field is (00)
    uint8_t interleave_gap_size;            ///< This field is only valid if the file is
                                            ///< recorded in interleave mode
                                            ///< Otherwise this field is (00)
    int32_t volume_sequence_number;         ///< The ordinal number of the volume in the Volume
                                            ///< Set on which the file described by the
                                            ///< directory record is recorded.
    uint8_t file_identifier_length;         ///< Length of File Identifier (LEN_FI)
    uint8_t file_identifier[1];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PathTableRecord
///
/// \brief	PathTableRecord data structure.
///
/// \author	Runik
/// \date	08/03/2011
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PathTableRecord
{
    uint8_t directory_identifier_length;        ///< Length of Directory Identifier (LEN_DI)
    uint8_t extended_attribute_record_length;   ///< If an Extended Attribute Record is
                                                ///< recorded, this is the length in Bytes.
                                                ///< Otherwise, this is (00)
    uint32_t location_of_extent;                ///<   Logical Block Number of the first Logical
                                                ///< Block allocated to the Directory
    uint16_t parent_directory_number;           ///<  The record number in the Path Table for
                                                ///< the parent directory of this directory
    uint8_t directory_identifier[1];            ///< This field is the same as in the Directory
                                                ///< Record
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PrimaryVolumeDescriptor
///
/// \brief	PrimaryVolumeDescriptor data structure.
///
/// \author	Runik
/// \date	08/03/2011
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PrimaryVolumeDescriptor
{
    uint8_t volume_descriptor_type;
    uint8_t standard_identifier[5];				    ///< CD001
    uint8_t volume_descriptor_version;
    uint8_t unused;
    uint8_t system_identifier[32];
    uint8_t volume_identifier[32];
    uint8_t unused2[8];
    int64_t volume_space_size;				        ///< Number of logical blocks in the Volume
    uint8_t unused3[32];
    uint32_t volume_set_size;						///< The assigned Volume Set size of the Volume
    uint32_t volume_sequence_number;				///< The ordinal number of the volume in
                                                        ///< the Volume Set
    uint32_t logical_block_size;					///< The size in bytes of a Logical Block
    int64_t  path_table_size;					    ///< Length in bytes of the path table
    uint32_t location_of_type_lpath_table;			///< Logical Block Number of first Block allocated
                                                    ///< to the Type L Path Table
    uint32_t location_of_optional_type_lpath_table;	///< 0 if Optional Path Table was not recorded,
                                                    ///< otherwise, Logical Block Number of first
                                                    ///< Block allocated to the Optional Type L
                                                    ///< Path Table
    uint32_t  location_of_type_mpath_table;			///< Logical Block Number of first Block
                                                    ///< allocated to the Type M
    uint32_t  location_of_optional_type_mpath_table;///< 0 if Optional Path Table was not
                                                    ///< recorded, otherwise, Logical Path Table,
                                                    ///< Block Number of first Block allocated to the
                                                    ///< Type M Path Table.
    DirectoryRecord directory_record_for_root_directory;///< This is the actual directory record for
                                                    ///< the top of the directory structure
    uint8_t volume_set_identifier[128];			    ///< Name of the multiple volume set of which
                                                    ///< this volume is a member.
    uint8_t publisher_identifier[128];			    ///< Identifies who provided the actual data
                                                    ///< contained in the files. acharacters allowed.
    uint8_t data_preparer_identifier[128];			///< Identifies who performed the actual
                                                    ///< creation of the current volume.
    uint8_t application_identifier[128];            ///< Identifies the specification of how the
                                                    ///< data in the files are recorded.
    uint8_t copyright_file_identifier[37];			///< Identifies the file in the root directory
                                                    ///< that contains the copyright notice for
                                                    ///< this volume
    uint8_t abstract_file_identifier[37];		    ///< Identifies the file in the root directory
                                                    ///< that contains the abstract statement for
                                                    ///< this volume
    uint8_t bibliographic_file_identifier[37];		///< Identifies the file in the root directory
                                                    ///< that contains bibliographic records.
    uint8_t volume_creation_date_and_time[17];		///< Date and time at which the volume was created
    uint8_t volume_modification_date_and_time[17];  ///< Date and time at which the volume was
                                                    ///< last modified
    uint8_t volume_expiration_date_and_time[17];	///< Date and Time at which the information in
                                                    ///< the volume may be considered obsolete.
    uint8_t volume_effective_date_and_time[17];		///< Date and Time at which the information
                                                    ///< in the volume may be used
    uint8_t file_structure_version;				    ///< 1
    uint8_t reserved_for_future_standardization;	///< 0
    uint8_t application_use[512];					///< This field is reserved for application use.
                                                    ///< Its content is not specified by ISO-9660.
    uint8_t reserved_for_future_standardization2[653]; ///< 0
};

}
}
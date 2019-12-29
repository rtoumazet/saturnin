// 
// cdrom.h
// Saturnin
//
// Copyright (c) 2003 Renaud Toumazet
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
// 

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	cdrom.h
///
/// \brief	Declares the cdrom class. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//#include <cstdint>
#include <vector>
#include <string>

#include "iso9660.h"
#include "scsi.h"

namespace saturnin {
namespace cdrom {
// not used ?
///// \name CD drive status
////@{
//const uint8_t STAT_BUSY = 0x00; ///< Busy. 
//const uint8_t STAT_PAUSE = 0x01;	///< Paused. 
//const uint8_t STAT_STANDBY = 0x02;	///< Stand-by. 
//const uint8_t STAT_PLAY = 0x03;	///< Playing. 
//const uint8_t STAT_SEEK = 0x04;	///< Seeking. 
//const uint8_t STAT_SCAN = 0x05;	///< Scanning. 
//const uint8_t STAT_OPEN = 0x06;	///< Drive is open. 
//const uint8_t STAT_NODISC = 0x07;	///< No disc inserted. 
//const uint8_t STAT_RETRY = 0x08;	///< Retrying. 
//const uint8_t STAT_ERROR = 0x09;	///< Error. 
//const uint8_t STAT_FATAL = 0x0A;	///< Fatal error. 
//const uint8_t STAT_PERI = 0x20; ///< Periodical response
//const uint8_t STAT_TRNS = 0x40; ///< Transfer request
//const uint8_t STAT_WAIT = 0x80; ///< Waiting
//const uint8_t STAT_REJECT = 0xFF;	///< Command rejected. 
////@}

/// \name Local memory registers
//@{
const uint32_t LOCAL_HIRQReg = 0x90008;	///< HIrq Register. 
const uint32_t LOCAL_HIRQMaskReg = 0x9000C;	///< HIrq Mask Register. 
const uint32_t LOCAL_CR1 = 0x90018;	///< Command Reg 1. 
const uint32_t LOCAL_CR2 = 0x9001C;	///< Command Reg 2. 
const uint32_t LOCAL_CR3 = 0x90020;	///< Command Reg 3. 
const uint32_t LOCAL_CR4 = 0x90024;	///< Command Reg 4. 
const uint32_t LOCAL_TOC_DATA_PTR = 0x98000;	///< TOC data pointer. 
const uint32_t LOCAL_FETCH_DATA_PTR = 0x18000;	///< Fetch data pointer. 
//@}

/// \name Status flags
//@{
const uint16_t CMOK = 0x0001;	///< System ready for a new command / System processing the last command. 
const uint16_t DRDY = 0x0002;	///< Data transfer setup (complete/not setup). 
const uint16_t CSCT = 0x0004;	///< Sector status (stored / discarded). 
const uint16_t BFUL = 0x0008;	///< CD buffer status (full / not full). 
const uint16_t PEND = 0x0010;	///< CD play status. 
const uint16_t DCHG = 0x0020;	///< Disk change status (changed/not changed). 
const uint16_t ESEL = 0x0040;	///< Soft reset or selector status (finished or not). 
const uint16_t EHST = 0x0080;	///< Host I/O status (finished or not). 
const uint16_t ECPY = 0x0100;	///< Sector copy or move status (finished or not). 
const uint16_t EFLS = 0x0200;	///< CD block file system status (finished or not). 
const uint16_t SCDQ = 0x0400;	///< Subcode Q decoding for current sector (decoded/not completed). 
//@{

/// \name CD type flags
//@{
const uint8_t FLAG_CDROM = 0x8;	///< CD-ROM flag. 
const uint8_t FLAG_CDDA = 0x0;	///< CDDA flag. 
//@}

/// \name Play mode
//@{
const uint8_t CDC_PM_DFL = 0x00;
const uint8_t CDC_PM_REP_NOCHG = 0x7f;
const uint8_t CDC_PM_PIC_NOCHG = 0x80;
const uint8_t CDC_PM_NOCHG = 0xFF;
//@}

/// \name Periodic response update cycle in ms
//@{
const float STD_PLAY_SPEED_PERIOD = 13.3f;	///< Standard play speed. 
const float DBL_PLAY_SPEED_PERIOD = 6.7f;	///< Double play speed. 
const float STANDBY_PERIOD = 16.7f;	///< Standby period. 
const uint8_t STD_PLAY_SPEED = 0;
const uint8_t DBL_PLAY_SPEED = 1;
const uint8_t STANDBY = 2;
//@}

/// \name Retry frequency
//@{
const uint8_t RETRY_FREQ_INFINITE = -1;
const uint8_t RETRY_FREQ_DATA_OUT_NO_RETRY = -2;
const uint8_t RETRY_FREQ_NO_RETRY = 0;
//@]

/// \name Play modes
//@{
const uint8_t PLAY_MODE_FAD = 0;	///< FAD mode. 
const uint8_t PLAY_MODE_TRACK = 1;	///< Track mode. 
//@}

const uint8_t MAX_SELECTORS = 24;	///< Selectors (filter+buffer partition) number. 
const uint8_t MAX_SECTORS = 200;	///< Number of sectors that can be stored. 
const uint16_t sector_size = 2352; 	///< Sector size in bytes. 

const uint8_t NO_FREE_SECTOR = -1; 	///< No free sector remaining in buffer. 

/// \name Valid sector lengths
//@{
const uint8_t SLEN_2048 = 0;
const uint8_t SLEN_2336 = 1;
const uint8_t SLEN_2340 = 2;
const uint8_t SLEN_2352 = 3;
//@}

const uint8_t FILTER_NOT_CONNECTED = -1;	///< No filter connexion. 

const uint32_t INVALID_FAD = 0xFFFFFF;	///< Invalid FAD. 

/// \name Number of cycles to read a sector
//@{
const uint32_t SECTOR_READ_1X = 439560;
const uint32_t SECTOR_READ_2X = 219580;
//@}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	Sector
///
/// \brief	Defines a sector.
///
/// \author	Runik
/// \date	01/03/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Sector
{
    uint8_t data[sector_size];	///< Sector data. 
    uint32_t size; 	///< Size of user data held. 
    uint32_t FAD; 	///< Sector FAD. 

   /// \name Sub header
   //@{
    uint8_t fileNumber;
    uint8_t channelNumber;
    uint8_t subMode;
    uint8_t codingInfo;
    //@}

    uint32_t trackNum; ///< Track number to which the sector belongs (avoids useless computing).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	Filter
///
/// \brief	Defines a filter.
///
/// \author	Runik
/// \date	01/03/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Filter
{
    /// \name Filter connections
    //@{
    uint32_t trueOutput;
    uint32_t falseOutput;
    uint8_t mode; ///< Filter mode.
   //@}

   // filter conditions
   /// \name FAD
   //@{
    uint32_t FAD; ///< Filter FAD start.
    uint32_t FADRange; ///< FAD range.
   //@}

   /// \name Sub header
   //@{
    uint8_t fileNumber;
    uint8_t channelNumber;
    uint8_t subModeMask;
    uint8_t subModeCompValue;
    uint8_t codingInfoMask;
    uint8_t codingInfoCompValue;
    //@}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	BufferPartition
///
/// \brief	Defines a buffer partition.
///
/// \author	Runik
/// \date	01/03/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
struct BufferPartition
{
    uint32_t	size; ///< Partition size in sectors.
    Sector*	        sectors[MAX_SECTORS]; ///< Sectors in sector buffer used by this partition.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	DirFileInfos
///
/// \brief	Directory/file info.
/// Encapsulates infos on file/directory.
///
/// \author	Runik
/// \date	01/03/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
struct DirFileInfos
{
    uint32_t fileNumber;
    uint32_t LSN;
    uint32_t dataLength;
    uint8_t flags;
    uint8_t directoryName;
    std::string  name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	CdromAccessMethod
///
/// \brief	Values that represent CdromAccessMethod. 
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CdromAccessMethod {
    aspi = 0,	///< Aspi mode access. 
    spti = 1	///< SPTI mode access. 
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Cdrom
///
/// \brief  Cdrom class.
///
/// \author Runik
/// \date   01/03/2010
////////////////////////////////////////////////////////////////////////////////////////////////////

class Cdrom
{
    public:
    static CdromAccessMethod      access_method; ///< Current CD-ROM access method.

    /// \name SCSI variables
    //@{
    //static ScsiDriveInfo            di_list[scsi_max_drives]; ///< List of drives available. 
    static std::vector<ScsiDriveInfo> di_list; ///< List of drives available. 
    static int8_t                   scsi_path;	              ///< Path. 
    static int8_t                   scsi_target;	          ///< Target. 
    static int8_t                   scsi_lun;	              ///< LUN. 
    static wchar_t                  scsi_letter;	          ///< Letter. 
    static ScsiToc	                toc_data;	              ///< TOC data. 
    static std::vector<std::string> scsi_drives_list;         ///< List of drives detected
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static uint8_t Cdrom::getDriveIndice(const int8_t path, const int8_t target, const int8_t lun);
    ///
    /// \brief  Gets drive indice from path/target/lun triplet.
    ///
    /// \author Runik
    /// \date   04/10/2018
    ///
    /// \param  path    scsi path.
    /// \param  target  scsi target.
    /// \param  lun     scsi lun.
    ///
    /// \return The drive indice.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static uint8_t getDriveIndice(const int8_t path, const int8_t target, const int8_t lun);

   
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn     uint8_t GetCdDriveStatus()
    ///
    /// \brief	Gets the CD drive status. 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \return	The CD drive status. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    uint8_t	GetCdDriveStatus();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void SetCdDriveStatus(std::uint8_t value)
    ///
    /// \brief	Sets the CD drive status. 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \param	value	Status. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetCdDriveStatus(uint8_t value);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void ExecuteCdBlockCommand(std::uint16_t value)
    ///
    /// \brief	Executes the CD block command. 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \param	value	Value of the command to execute. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void ExecuteCdBlockCommand(uint16_t value);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void SendStatus()
    ///
    /// \brief	Send drive status. 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SendStatus();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn Cdrom::Cdrom();
    ///
    /// \brief  Default constructor.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    Cdrom();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn Cdrom::Cdrom(bool);
    ///
    /// \brief  Constructor.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ///
    /// \param  parameter1  .
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    Cdrom(bool);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn virtual Cdrom::~Cdrom();
    ///
    /// \brief  Finaliser.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    virtual ~Cdrom();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void RefreshPeriod()
    ///
    /// \brief	Calls UpdatePeriod(). 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void RefreshPeriod();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Cdrom::SetSectorBurstSize(uint32_t size);
    ///
    /// \brief  Sets the number of sectors to read at a time.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ///
    /// \param  size    Number of sectors.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void SetSectorBurstSize(uint32_t size);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn     uint32_t GetSectorBurstSize()
    ///
    /// \brief	Gets the sector burst size. 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \return	Number of sectors. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t	GetSectorBurstSize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void InsertCd()
    ///
    /// \brief	Notifies that a CD has been inserted. 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void	InsertCd();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void RunCdBlock(std::int32_t cycles)
    ///
    /// \brief	Runs the CD block for the specified number of cycles. 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \param	cycles	Number of cycles to run. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void	RunCdBlock(uint32_t cycles);

    /// \name CD block memory accessors
    //@{
    void		WriteByte(uint32_t address, uint8_t data);
    void		WriteWord(uint32_t address, uint16_t data);
    void		WriteLong(uint32_t address, uint32_t data);
    uint8_t	    ReadByte(uint32_t address);
    uint16_t	ReadWord(uint32_t address);
    uint32_t	ReadLong(uint32_t address);
    //@}

    private:
    uint8_t	drive_status_;	///< CD drive status. 

    ::std::vector<DirFileInfos> filesOnCD; ///< File list present on CD.

   /// \name CD block registers
   //@{
    uint16_t    HIRQREQ;
    uint16_t    HIRQMSK;
    uint16_t    CR1;
    uint16_t    CR2;
    uint16_t    CR3;
    uint16_t    CR4;
    //@}

    Sector          sectorsBuffer[MAX_SECTORS]; ///< Sector buffer.
    Filter          filters[MAX_SELECTORS]; ///< Filters.
    BufferPartition	bufferPartitions[MAX_SELECTORS]; ///< Buffer partitions.

    uint32_t	driveSpeed; ///< 1x speed, 2x speed or standby.
    uint32_t	cyclesPerMs; ///< number of SH2 cycles per millisecond.
    uint32_t	standbyTime;	///< Santdby time. 
    uint32_t	responsePeriod;	///< Response period. 
    uint32_t	ECCFreq; ///< ECC frequency update.
    uint32_t	retryFreq; ///< Retry frequency when sector reading error occurs.

   /// \name Sector length accessors (in bytes)
   //@{
    uint32_t	getSectorLength;
    uint32_t	putSectorLength;
    //@}

    /// \name CD block status and report
    //@{
    uint8_t	status; ///< Status.
    uint8_t	flag; ///< Flag.
    uint8_t	repCnt; ///< Repeat frequency.
    uint8_t	ctrlAdr; ///< Control/adr byte of subcode Q.
    uint8_t	tno; ///< Track number of subcode Q.
    uint8_t	ino; ///< Index number of subcode Q.
    uint32_t    FAD; ///< Frame address.
    uint32_t	currentPlayMode; ///< Track mode or fad mode.
    uint32_t    playFADStart; ///< Initial position at the beginning of last play disk command.
    uint32_t    remainingFADs; ///< Remaining number of FADS to play in FAD mode.
   //@}

   ////////////////////////////////////////////////////////////////////////////////////////////////////
   /// \fn     uint32_t FindCurrentTrack()
   ///
   /// \brief	Searches for the current track. 
   ///
   /// \author	Runik
   /// \date	01/03/2010
   ///
   /// \return	Current track. 
   ////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t	FindCurrentTrack();

    bool			DeleteSectorDataExecuted;	///< To know if sectors must be deleted in sectors buffer. 
    uint32_t	numberOfSectorsToDelete; ///< Number of sectors to delete.
    uint32_t	posOfSectorsToDelete; ///< Position of sectors to delete.
    uint32_t	currentFilter; ///< Current filter connection.

   /// \name Current fetching parameters
   //@{
    uint32_t	fetchedBuffer;	///< Fetched buffer. 
    uint32_t	sectorNumInBuffer;	///< Sector number in buffer. 
    uint32_t	posInSector;	///< Posiiton in sector. 
    uint32_t	bytesTransfered; ///< Number of bytes transfered by the CD block.
    uint32_t	actualSize; ///< Actual size command variable.
    uint32_t	lastFetchedBuffer; ///< Last buffer partition fetched (for GetLastBufferDestination).
   //@}

   ////////////////////////////////////////////////////////////////////////////////////////////////////
   /// \fn     uint32_t FreeNumOfSectorsInBuffer()
   ///
   /// \brief	Free number of sectors in buffer. 
   ///
   /// \author	Runik
   /// \date	01/03/2010
   ///
   /// \return	The number of available sectors in sector buffer. 
   ////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t	FreeNumOfSectorsInBuffer();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn     uint32_t FindFreeSector()
    ///
    /// \brief	Searches for the first free sector. 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \return	The first free sector found, or -1 if no free sector available. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t	FindFreeSector();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void CdBlockReadSectors(std::int32_t num)
    ///
    /// \brief	Requests the reading of num sector at current FAD and outputs data to current filter. 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \param	num	Number of the sector to read. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void            CdBlockReadSectors(uint32_t num);
    uint32_t	sectorBurstSize;	///< Sector burst read size. 

   ////////////////////////////////////////////////////////////////////////////////////////////////////
   /// \fn	bool CdInserted()
   ///
   /// \brief	Checks if a CD is present in the CD drive. 
   ///
   /// \author	Runik
   /// \date	01/03/2010
   ///
   /// \return	true if a cd is inserted, false otherwise. 
   ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool    CdInserted();

    AspiToc*       AspiTOC; ///< ASPI TOC.

    uint8_t	saturnTOC[4 * 102]; ///< Current saturn game disk TOC.
    void            BuildSaturnTOC(); ///< Builds the TOC.

    uint8_t	filesInfos[12 * 256]; ///< Buffer holding saturn files infos.

   ////////////////////////////////////////////////////////////////////////////////////////////////////
   /// \fn	void BuildFileInfos(std::uint32_t fileId)
   ///
   /// \brief	Builds the Saturn file infos. 
   ///
   /// \author	Runik
   /// \date	04/03/2010
   ///
   /// \param	fileId	Identifier for the file. 
   ////////////////////////////////////////////////////////////////////////////////////////////////////
    void BuildFileInfos(uint32_t fileId);

    uint8_t*	dataBuffer; ///< Pointer to data read when file info or TOC is read.
    uint32_t	dataBufferSize; ///< Size of data held in buffer in word unit.
    uint32_t	posInDataBuffer; ///< Current word being read in data buffer (TOC or file infos).

   ////////////////////////////////////////////////////////////////////////////////////////////////////
   /// \fn	void BuildFileSystemTree()
   ///
   /// \brief	Builds the current CD file system directory tree.
   ///
   /// \author	Runik
   /// \date	04/03/2010
   ////////////////////////////////////////////////////////////////////////////////////////////////////
    void BuildFileSystemTree();

    bool writingCRRegs; ///< Prevents from executing periodic response while a command is written in CR register.
    bool firstReading; ///< Prevents from executing periodic response before initialisation string is read in CR registers.

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void UpdatePeriod()
    ///
    /// \brief	Updates the drive period. 
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdatePeriod();
    uint32_t	counter;	///< Counter. 
    uint32_t	executedCommands; ///< Number of commands executed.
    uint32_t	executedCommandsMax; ///< Number max of commands executed during periodic response.

   /// \name Current parameters for transfer from host to CD block
   //@{
    uint32_t	sectorNumInBufferPut;
    uint32_t	posInSectorPut;
    uint32_t	numOfSectorToPut;
    //@}
};

}
}



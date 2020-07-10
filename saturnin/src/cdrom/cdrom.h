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
#include <chrono>
#include <vector>
#include <string>

#include "cdrom_registers.h"
#include "iso9660.h"
#include "scsi.h"

// Forward declarations
namespace saturnin::core {
class EmulatorContext;
}

using saturnin::core::EmulatorContext;
using milli = std::chrono::duration<double, std::milli>;

namespace saturnin::cdrom {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CdDriveStatus
///
/// \brief  Values that represent CD drive status
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CdDriveStatus : u8 {
    busy                = 0x00,
    paused              = 0x01,
    standby             = 0x02,
    playing             = 0x03,
    seeking             = 0x04,
    scanning            = 0x05,
    drive_is_open       = 0x06,
    no_disc_inserted    = 0x07,
    retrying            = 0x08,
    error               = 0x09,
    fatal_error         = 0x0a,
    periodical_response = 0x20,
    transfer_request    = 0x40,
    waiting             = 0x80,
    command_rejected    = 0xff
};

/// \name Local memory registers
//@{
const u32 LOCAL_HIRQReg        = 0x90008; ///< HIrq Register.
const u32 LOCAL_HIRQMaskReg    = 0x9000C; ///< HIrq Mask Register.
const u32 LOCAL_CR1            = 0x90018; ///< Command Reg 1.
const u32 LOCAL_CR2            = 0x9001C; ///< Command Reg 2.
const u32 LOCAL_CR3            = 0x90020; ///< Command Reg 3.
const u32 LOCAL_CR4            = 0x90024; ///< Command Reg 4.
const u32 LOCAL_TOC_DATA_PTR   = 0x98000; ///< TOC data pointer.
const u32 LOCAL_FETCH_DATA_PTR = 0x18000; ///< Fetch data pointer.
//@}

/// \name Status flags
//@{
const u16 CMOK = 0x0001; ///< System ready for a new command / System processing the last command.
const u16 DRDY = 0x0002; ///< Data transfer setup (complete/not setup).
const u16 CSCT = 0x0004; ///< Sector status (stored / discarded).
const u16 BFUL = 0x0008; ///< CD buffer status (full / not full).
const u16 PEND = 0x0010; ///< CD play status.
const u16 DCHG = 0x0020; ///< Disk change status (changed/not changed).
const u16 ESEL = 0x0040; ///< Soft reset or selector status (finished or not).
const u16 EHST = 0x0080; ///< Host I/O status (finished or not).
const u16 ECPY = 0x0100; ///< Sector copy or move status (finished or not).
const u16 EFLS = 0x0200; ///< CD block file system status (finished or not).
const u16 SCDQ = 0x0400; ///< Subcode Q decoding for current sector (decoded/not completed).
//@{

/// \name CD type flags
//@{
const u8 FLAG_CDROM = 0x8; ///< CD-ROM flag.
const u8 FLAG_CDDA  = 0x0; ///< CDDA flag.
//@}

/// \name Play mode
//@{
const u8 CDC_PM_DFL       = 0x00;
const u8 CDC_PM_REP_NOCHG = 0x7f;
const u8 CDC_PM_PIC_NOCHG = 0x80;
const u8 CDC_PM_NOCHG     = 0xFF;
//@}

/// \name Periodic response update cycle in ms
//@{
constexpr auto periodic_response_period_standard{milli(13.3f)}; ///< Standard play speed period.
constexpr auto periodic_response_period_double{milli(6.7f)};    ///< Double play speed period.
constexpr auto periodic_response_period_standby{milli(16.7f)};  ///< Standby period.
//@}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PlayMode
///
/// \brief  Values that represent cd drive play mode
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CdDrivePlayMode : u8 { unknown = 0xff, standard_play_speed = 0, double_play_speed = 1, standby = 2 };

/// \name Retry frequency
//@{
const u8 RETRY_FREQ_INFINITE          = -1;
const u8 RETRY_FREQ_DATA_OUT_NO_RETRY = -2;
const u8 RETRY_FREQ_NO_RETRY          = 0;
//@]

/// \name Play modes
//@{
const u8 PLAY_MODE_FAD   = 0; ///< FAD mode.
const u8 PLAY_MODE_TRACK = 1; ///< Track mode.
//@}

const u8  MAX_SELECTORS = 24;   ///< Selectors (filter+buffer partition) number.
const u8  MAX_SECTORS   = 200;  ///< Number of sectors that can be stored.
const u16 sector_size   = 2352; ///< Sector size in bytes.

const u8 NO_FREE_SECTOR = -1; ///< No free sector remaining in buffer.

/// \name Valid sector lengths
//@{
const u8 SLEN_2048 = 0;
const u8 SLEN_2336 = 1;
const u8 SLEN_2340 = 2;
const u8 SLEN_2352 = 3;
//@}

const u8 FILTER_NOT_CONNECTED = -1; ///< No filter connexion.

const u32 INVALID_FAD = 0xFFFFFF; ///< Invalid FAD.

/// \name Number of cycles to read a sector
//@{
const u32 SECTOR_READ_1X = 439560;
const u32 SECTOR_READ_2X = 219580;
//@}

constexpr u32 file_info_size{12 * 256};
constexpr u32 saturn_toc_size{4 * 102};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	Sector
///
/// \brief	Defines a sector.
///
/// \author	Runik
/// \date	01/03/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Sector {
    std::array<u8, sector_size> data; ///< Sector data.
    u32                         size; ///< Size of user data held.
    u32                         FAD;  ///< Sector FAD.

    /// \name Sub header
    //@{
    u8 fileNumber;
    u8 channelNumber;
    u8 subMode;
    u8 codingInfo;
    //@}

    u32 trackNum; ///< Track number to which the sector belongs (avoids useless computing).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	Filter
///
/// \brief	Defines a filter.
///
/// \author	Runik
/// \date	01/03/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Filter {
    /// \name Filter connections
    //@{
    u32 trueOutput;
    u32 falseOutput;
    u8  mode; ///< Filter mode.
              //@}

    // filter conditions
    /// \name FAD
    //@{
    u32 FAD;      ///< Filter FAD start.
    u32 FADRange; ///< FAD range.
                  //@}

    /// \name Sub header
    //@{
    u8 fileNumber;
    u8 channelNumber;
    u8 subModeMask;
    u8 subModeCompValue;
    u8 codingInfoMask;
    u8 codingInfoCompValue;
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
struct BufferPartition {
    u32                              size;    ///< Partition size in sectors.
    std::array<Sector*, MAX_SECTORS> sectors; ///< Sectors in sector buffer used by this partition.
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
struct DirFileInfos {
    u32         fileNumber;
    u32         LSN;
    u32         dataLength;
    u8          flags;
    u8          directoryName;
    std::string name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	CdromAccessMethod
///
/// \brief	Values that represent CdromAccessMethod.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CdromAccessMethod {
    aspi = 0, ///< Aspi mode access.
    spti = 1  ///< SPTI mode access.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Cdrom
///
/// \brief  Cdrom class.
///
/// \author Runik
/// \date   01/03/2010
////////////////////////////////////////////////////////////////////////////////////////////////////

class Cdrom {
  public:
    //@{
    // Constructors / Destructors
    Cdrom() = delete;
    Cdrom(EmulatorContext* ec) : emulator_context_(ec){};
    Cdrom(const Cdrom&) = delete;
    Cdrom(Cdrom&&)      = delete;
    auto operator=(const Cdrom&) & -> Cdrom& = delete;
    auto operator=(Cdrom&&) & -> Cdrom& = delete;
    ~Cdrom()                            = default;
    //@}

    void initialize();

    static CdromAccessMethod access_method; ///< Current CD-ROM access method.

    /// \name SCSI variables
    //@{
    // static ScsiDriveInfo            di_list[scsi_max_drives]; ///< List of drives available.
    static std::vector<ScsiDriveInfo> di_list;          ///< List of drives available.
    static s8                         scsi_path;        ///< Path.
    static s8                         scsi_target;      ///< Target.
    static s8                         scsi_lun;         ///< LUN.
    static wchar_t                    scsi_letter;      ///< Letter.
    static ScsiToc                    toc_data;         ///< TOC data.
    static std::vector<std::string>   scsi_drives_list; ///< List of drives detected
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Cdrom::getDriveIndice(s8 path, s8 target, s8 lun) -> u8;
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
    /// \returns    The drive indice.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getDriveIndice(s8 path, s8 target, s8 lun) -> u8;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn     u8 GetCdDriveStatus()
    /////
    ///// \brief	Gets the CD drive status.
    /////
    ///// \author	Runik
    ///// \date	01/03/2010
    /////
    ///// \return	The CD drive status.
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // auto GetCdDriveStatus() -> u8;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn	void SetCdDriveStatus(std::u8 value)
    /////
    ///// \brief	Sets the CD drive status.
    /////
    ///// \author	Runik
    ///// \date	01/03/2010
    /////
    ///// \param	value	Status.
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // void SetCdDriveStatus(u8 value);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn	void ExecuteCdBlockCommand(std::u16 value)
    /////
    ///// \brief	Executes the CD block command.
    /////
    ///// \author	Runik
    ///// \date	01/03/2010
    /////
    ///// \param	value	Value of the command to execute.
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // void ExecuteCdBlockCommand(u16 value);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void RefreshPeriod()
    ///
    /// \brief	Calls UpdatePeriod().
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void refreshPeriod(){};

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn void Cdrom::SetSectorBurstSize(u32 size);
    /////
    ///// \brief  Sets the number of sectors to read at a time.
    /////
    ///// \author Runik
    ///// \date   01/03/2010
    /////
    ///// \param  size    Number of sectors.
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    // void SetSectorBurstSize(u32 size);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn     u32 GetSectorBurstSize()
    /////
    ///// \brief	Gets the sector burst size.
    /////
    ///// \author	Runik
    ///// \date	01/03/2010
    /////
    ///// \return	Number of sectors.
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // auto GetSectorBurstSize() -> u32;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn	void InsertCd()
    /////
    ///// \brief	Notifies that a CD has been inserted.
    /////
    ///// \author	Runik
    ///// \date	01/03/2010
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // void InsertCd();

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn	void RunCdBlock(std::int32_t cycles)
    /////
    ///// \brief	Runs the CD block for the specified number of cycles.
    /////
    ///// \author	Runik
    ///// \date	01/03/2010
    /////
    ///// \param	cycles	Number of cycles to run.
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // void RunCdBlock(u32 cycles);

    template<typename T>
    void writeRegisters(const u32 addr, const T data) {
        // rawWrite<T>(io_registers_, addr & sh2_memory_mask, data);
    }

    // 8 bits specialization
    template<>
    void writeRegisters<u8>(const u32 addr, const u8 data) {
        write8(addr, data);
    }

    // 16 bits specialization
    template<>
    void writeRegisters<u16>(const u32 addr, const u16 data) {
        write16(addr, data);
    }

    // 32 bits specialization
    template<>
    void writeRegisters<u32>(const u32 addr, const u32 data) {
        write32(addr, data);
    }

    template<typename T>
    auto readRegisters(const u32 addr) -> T {
        // return rawRead<T>(this->io_registers_, addr & sh2_memory_mask);
    }
    // 8 bits specialization
    template<>
    auto readRegisters<u8>(const u32 addr) -> u8 {
        return read8(addr);
    }
    // 16 bits specialization
    template<>
    auto readRegisters<u16>(const u32 addr) -> u16 {
        return read16(addr);
    }
    // 32 bits specialization
    template<>
    auto readRegisters<u32>(const u32 addr) -> u32 {
        return read32(addr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Cdrom::run(const u8 cycles);
    ///
    /// \brief  Runs the cdblock for the given number of cycles
    ///
    /// \author Renaud Toumazet
    /// \date   10/07/2020
    ///
    /// \param  cycles  The number of cycles to run.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void run(u8 cycles);

    auto getRegisters() -> std::vector<std::string>;

  private:
    /// \name CD block memory accessors
    //@{
    void               write8(u32 addr, u8 data);
    void               write16(u32 addr, u16 data);
    void               write32(u32 addr, u32 data);
    [[nodiscard]] auto read8(u32 addr) const -> u8;
    [[nodiscard]] auto read16(u32 addr) -> u16;
    [[nodiscard]] auto read32(u32 addr) const -> u32;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Cdrom::reset();
    ///
    /// \brief  Resets the cdrom module
    ///
    /// \author Runik
    /// \date   14/05/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void reset();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Cdrom::calculatePeriodicResponsePeriod()-> u32;
    ///
    /// \brief  Calculates the periodic response period, depending on the current SH2 clock and the drive speed
    ///
    /// \author Runik
    /// \date   05/07/2020
    ///
    /// \returns    The calculated periodic response period.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto calculatePeriodicResponsePeriod() -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn    void Cdrom::sendStatus();
    ///
    /// \brief Sends the status of the drive
    ///
    /// \author    Runik
    /// \date  05/07/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void sendStatus();

    //::std::vector<DirFileInfos> filesOnCD; ///< File list present on CD.

    ///// \name CD block registers
    ////@{
    //// u16 HIRQREQ{};
    // HirqRegister hirqreg_;
    // u16          HIRQMSK{};
    // u16          CR1{};
    // u16          CR2{};
    // u16          CR3{};
    // u16          CR4{};
    ////@}

    // std::array<Sector, MAX_SECTORS>            sectorsBuffer{};    ///< Sector buffer.
    // std::array<Filter, MAX_SELECTORS>          filters{};          ///< Filters.
    // std::array<BufferPartition, MAX_SELECTORS> bufferPartitions{}; ///< Buffer partitions.

    // u32 driveSpeed{};     ///< 1x speed, 2x speed or standby.
    // u32 cyclesPerMs{};    ///< number of SH2 cycles per millisecond.
    // u32 standbyTime{};    ///< Santdby time.
    // u32 responsePeriod{}; ///< Response period.
    // u32 ECCFreq{};        ///< ECC frequency update.
    // u32 retryFreq{};      ///< Retry frequency when sector reading error occurs.

    ///// \name Sector length accessors (in bytes)
    ////@{
    // u32 getSectorLength{};
    // u32 putSectorLength{};
    ////@}

    ///// \name CD block status and report
    ////@{
    // u8  status{};          ///< Status.
    // u8  flag{};            ///< Flag.
    // u8  repCnt{};          ///< Repeat frequency.
    // u8  ctrlAdr{};         ///< Control/adr byte of subcode Q.
    // u8  tno{};             ///< Track number of subcode Q.
    // u8  ino{};             ///< Index number of subcode Q.
    // u32 FAD{};             ///< Frame address.
    // u32 currentPlayMode{}; ///< Track mode or fad mode.
    // u32 playFADStart{};    ///< Initial position at the beginning of last play disk command.
    // u32 remainingFADs{};   ///< Remaining number of FADS to play in FAD mode.
    //                       //@}

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn     u32 FindCurrentTrack()
    /////
    ///// \brief	Searches for the current track.
    /////
    ///// \author	Runik
    ///// \date	01/03/2010
    /////
    ///// \return	Current track.
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // auto FindCurrentTrack() -> u32;

    // bool DeleteSectorDataExecuted{}; ///< To know if sectors must be deleted in sectors buffer.
    // u32  numberOfSectorsToDelete{};  ///< Number of sectors to delete.
    // u32  posOfSectorsToDelete{};     ///< Position of sectors to delete.
    // u32  currentFilter{};            ///< Current filter connection.

    ///// \name Current fetching parameters
    ////@{
    // u32 fetchedBuffer{};     ///< Fetched buffer.
    // u32 sectorNumInBuffer{}; ///< Sector number in buffer.
    // u32 posInSector{};       ///< Posiiton in sector.
    // u32 bytesTransfered{};   ///< Number of bytes transfered by the CD block.
    // u32 actualSize{};        ///< Actual size command variable.
    // u32 lastFetchedBuffer{}; ///< Last buffer partition fetched (for GetLastBufferDestination).
    //                         //@}

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn     u32 FreeNumOfSectorsInBuffer()
    /////
    ///// \brief	Free number of sectors in buffer.
    /////
    ///// \author	Runik
    ///// \date	01/03/2010
    /////
    ///// \return	The number of available sectors in sector buffer.
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // auto FreeNumOfSectorsInBuffer() -> u32;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn auto Cdrom::FindFreeSector() -> u32;
    /////
    ///// \brief  Searches for the first free sector.
    /////
    ///// \author Runik
    ///// \date   01/03/2010
    /////
    ///// \returns    The first free sector found, or -1 if no free sector available.
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    // auto FindFreeSector() -> u32;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn	void CdBlockReadSectors(std::int32_t num)
    /////
    ///// \brief	Requests the reading of num sector at current FAD and outputs data to current filter.
    /////
    ///// \author	Runik
    ///// \date	01/03/2010
    /////
    ///// \param	num	Number of the sector to read.
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // void CdBlockReadSectors(u32 num);
    // u32  sectorBurstSize{}; ///< Sector burst read size.

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn auto Cdrom::CdInserted() -> bool;
    /////
    ///// \brief  Checks if a CD is present in the CD drive.
    /////
    ///// \author Runik
    ///// \date   01/03/2010
    /////
    ///// \returns    true if a cd is inserted.
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    // auto CdInserted() -> bool;

    // AspiToc* AspiTOC{}; ///< ASPI TOC.

    // std::array<u8, saturn_toc_size> saturnTOC;        ///< Current saturn game disk TOC.
    // void                            BuildSaturnTOC(); ///< Builds the TOC.

    // std::array<u8, file_info_size> filesInfos; ///< Buffer holding saturn files infos.

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn	void BuildFileInfos(std::u32 fileId)
    /////
    ///// \brief	Builds the Saturn file infos.
    /////
    ///// \author	Runik
    ///// \date	04/03/2010
    /////
    ///// \param	fileId	Identifier for the file.
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // void BuildFileInfos(u32 fileId);

    // u8* dataBuffer{};      ///< Pointer to data read when file info or TOC is read.
    // u32 dataBufferSize{};  ///< Size of data held in buffer in word unit.
    // u32 posInDataBuffer{}; ///< Current word being read in data buffer (TOC or file infos).

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn	void BuildFileSystemTree()
    /////
    ///// \brief	Builds the current CD file system directory tree.
    /////
    ///// \author	Runik
    ///// \date	04/03/2010
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // void BuildFileSystemTree();

    // bool writingCRRegs{}; ///< Prevents from executing periodic response while a command is written in CR register.
    // bool firstReading{};  ///< Prevents from executing periodic response before initialisation string is read in CR registers.

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// \fn	void UpdatePeriod()
    /////
    ///// \brief	Updates the drive period.
    /////
    ///// \author	Runik
    ///// \date	01/03/2010
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // void UpdatePeriod();
    // u32  counter{};             ///< Counter.
    // u32  executedCommands{};    ///< Number of commands executed.
    // u32  executedCommandsMax{}; ///< Number max of commands executed during periodic response.

    ///// \name Current parameters for transfer from host to CD block
    ////@{
    // u32 sectorNumInBufferPut{};
    // u32 posInSectorPut{};
    // u32 numOfSectorToPut{};
    //@}

    EmulatorContext* emulator_context_; ///< Emulator context object.

    HirqStatusRegister hirq_status_reg_; ///< HIrq status register.
    HirqMaskRegister   hirq_mask_reg_;   ///< HIrq mask register.
    CommandRegister    cr1_;             ///< Command register 1.
    CommandRegister    cr2_;             ///< Command register 2.
    CommandRegister    cr3_;             ///< Command register 3.
    CommandRegister    cr4_;             ///< Command register 4.

    u16  elapsed_cycles_;                ///< The elapsed cycles
    bool is_command_running_{false};     ///< True if a command is being executed, ie command registers are written
    bool is_initialization_done_{false}; ///< Prevents writing to command registers while init string hasn't been read

    CdDriveStatus   cd_drive_status_;
    CdDrivePlayMode cd_drive_play_mode_;

    u8 max_number_of_commands_{}; ///< The maximum number of executable commands by the cd block
};

} // namespace saturnin::cdrom

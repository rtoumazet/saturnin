// 
// spti.cpp
// Saturnin
//
// Copyright (c) 2003-2016 Renaud Toumazet
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
#pragma warning(push, 3) 
#include <windows.h>
#include <memory>
#include <ntddscsi.h>
#include <ntddcdrm.h>
#pragma warning(pop) 

#include "cdrom.h"
#include "spti.h"
#include"..\locale.h"
#include"..\log.h"
#include "..\utilities.h"

namespace util = saturnin::utilities;

namespace saturnin {
namespace cdrom {

// to be removed when cdrom.cpp is added to the project
//SCSI_DriveInfo	CCdRom::diList[SCSI_MAX_DRIVES];
//int8_t			CCdRom::SCSI_Path = -1;
//int8_t			CCdRom::SCSI_Target = -1;
//int8_t			CCdRom::SCSI_Lun = -1;
//wchar_t			CCdRom::SCSI_Letter = 0;

/* static */
bool Spti::initialize() {
    return true;
}

/* static */
void Spti::shutdown() {

}

/* static */
uint8_t Spti::scanBus() {
    std::wstring path = L"C:\\";
    uint8_t index{};

    for (path[0] = 'C'; path[0] <= 'Z'; path[0]++) {
        if (GetDriveType(path.c_str()) == DRIVE_CDROM) {
            HANDLE drive_handle = Scsi::openDrive(path[0]);
            if (drive_handle != INVALID_HANDLE_VALUE) {
                Cdrom::di_list[index].letter = path.c_str(); // adding the drive to the list
                Spti::getAdapterAddress(drive_handle, Cdrom::di_list[index]); // getting the address of the current letter drive

                if ((Cdrom::di_list[index].path != -1) && (Cdrom::di_list[index].target!= -1) && (Cdrom::di_list[index].lun != -1)) {
                    Spti::inquiry(drive_handle, Cdrom::di_list[index]); // filling the current structure with inquiry data
                }
                CloseHandle(drive_handle);
                ++index; // moving to the next drive
            }
        }
    }
    return index; // number of drives is returned
}

/* static */
void Spti::test() {

}

/* static */
std::string Spti::readOneSector(const uint32_t& fad) {
    uint32_t            real_fad = fad - 150;
    constexpr uint32_t  sector_size = 2048;
    std::array<int8_t, sector_size> output_buffer{};

    SCSI_PASS_THROUGH_DIRECT input_buffer{};

    input_buffer.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    input_buffer.CdbLength = 12;
    input_buffer.DataTransferLength = sector_size;
    input_buffer.TimeOutValue = 60;
    input_buffer.DataBuffer = &output_buffer[0];
    input_buffer.SenseInfoLength = 0;
    input_buffer.PathId = Cdrom::scsi_path;
    input_buffer.TargetId = Cdrom::scsi_target;
    input_buffer.Lun = Cdrom::scsi_lun;
    input_buffer.SenseInfoOffset = 0; // offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
    input_buffer.DataIn = SCSI_IOCTL_DATA_IN;
    input_buffer.Cdb[0] = 0xBE;
    input_buffer.Cdb[2] = static_cast<uint8_t>(real_fad >> 24);
    input_buffer.Cdb[3] = static_cast<uint8_t>(real_fad >> 16);
    input_buffer.Cdb[4] = static_cast<uint8_t>(real_fad >> 8);
    input_buffer.Cdb[5] = static_cast<uint8_t>(real_fad);
    input_buffer.Cdb[6] = 0;
    input_buffer.Cdb[7] = 0;
    input_buffer.Cdb[8] = 1;     // one frame read
    input_buffer.Cdb[9] = 0x10;  // getting only main channel data

    HANDLE drive_handle = Scsi::openDrive(Cdrom::scsi_letter);
    uint32_t dummy{};
    if (!DeviceIoControl(drive_handle, IOCTL_STORAGE_CHECK_VERIFY,
                         NULL,
                         0,
                         NULL,
                         0,
                         reinterpret_cast<LPDWORD>(&dummy),
                         NULL)) {

        core::Log::warning("cdrom", core::tr("Drive isn't accessible"));

    }
    else {

        if (!DeviceIoControl(drive_handle,
                             IOCTL_SCSI_PASS_THROUGH_DIRECT,
                             &input_buffer,
                             sizeof(input_buffer),
                             &input_buffer,
                             sizeof(input_buffer),
                             reinterpret_cast<LPDWORD>(&dummy),
                             NULL)) {

            core::Log::warning("cdrom", util::getLastErrorMessage());
        }
    }

    CloseHandle(drive_handle);
    std::string sector_data(output_buffer.begin(), output_buffer.end());
    return sector_data;
}

/* static */
std::string Spti::readSector(const uint32_t& fad, const int32_t & sectors_number) {
    std::string sector_data{};
    for (uint8_t cnt = 0; cnt < sectors_number; ++cnt) {
        sector_data += readOneSector(fad);
    }

    return sector_data;
}

/* static */
void Spti::inquiry(const HANDLE& drive_handle, ScsiDriveInfo& di) {
    std::array<int8_t, 36> output_buffer{};
    

    SCSI_PASS_THROUGH_DIRECT input_buffer{};

    input_buffer.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
    input_buffer.PathId             = di.path;
    input_buffer.DataIn             = SCSI_IOCTL_DATA_IN;
    input_buffer.TargetId           = di.target;
    input_buffer.Lun                = di.lun;
    input_buffer.DataTransferLength = 36;
    input_buffer.DataBuffer         = &output_buffer[0];
    input_buffer.SenseInfoLength    = 0;
    input_buffer.CdbLength          = 6;
    input_buffer.Cdb[0]             = scsi_inquiry;
    input_buffer.Cdb[4]             = 36;
    input_buffer.TimeOutValue       = spti_timeout; 

    uint32_t dummy{};
    if (!DeviceIoControl(drive_handle,
                         IOCTL_SCSI_PASS_THROUGH_DIRECT,
                         &input_buffer,
                         sizeof(input_buffer),
                         &input_buffer,
                         sizeof(input_buffer),
                         reinterpret_cast<LPDWORD>(&dummy),
                         NULL)) {
        
        core::Log::warning("cdrom", util::getLastErrorMessage());

    }
    else {
        std::wstring temp(output_buffer.begin(), output_buffer.end());
        di.name = temp.substr(8, 28);
    }
}

/* static */
void Spti::getAdapterAddress(const HANDLE& drive_handle, ScsiDriveInfo& di) {
    //std::array<int8_t, 1024> v(1024);

    di.path   = -1;
    di.target = -1;
    di.lun    = -1; 
    if (drive_handle == 0) return;

    //pSA = reinterpret_cast<PSCSI_ADDRESS>(&v[0]);
    SCSI_ADDRESS psa{};
    psa.Length = sizeof(SCSI_ADDRESS);

    uint32_t status{};
    if (!DeviceIoControl(drive_handle, 
                         IOCTL_SCSI_GET_ADDRESS, 
                         NULL, 
                         0, 
                         (LPVOID)&psa, 
                         sizeof(SCSI_ADDRESS), 
                         reinterpret_cast<LPDWORD>(&status), 
                         NULL))  return;

    di.path   = psa.PortNumber;
    di.target = psa.TargetId;
    di.lun    = psa.Lun;
}

/* static */
bool Spti::readToc(ScsiToc& toc)
{
    uint32_t out_bytes{};
    CDROM_TOC cdrom_toc;
    HANDLE drive_handle = Scsi::openDrive(Cdrom::scsi_letter);
    if (!DeviceIoControl(drive_handle,
                         IOCTL_CDROM_READ_TOC, NULL, 0,
                         &cdrom_toc,
                         sizeof(cdrom_toc),
                         reinterpret_cast<LPDWORD>(&out_bytes),
                         (LPOVERLAPPED)NULL)) {
        return false;
    }

    toc = reinterpret_cast<ScsiToc&>(cdrom_toc);
    return true;
}
}
}
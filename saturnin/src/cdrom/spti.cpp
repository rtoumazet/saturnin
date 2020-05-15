//
// spti.cpp
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
#pragma warning(push, 3)
#include <windows.h>
#include <memory>
#include <ntddscsi.h>
#include <ntddcdrm.h>
#pragma warning(pop)

#include "cdrom.h"
#include "spti.h"
#include "..\locale.h"
#include "..\log.h"
#include "..\utilities.h"

namespace util = saturnin::utilities;

namespace saturnin::cdrom {

using core::Log;
using core::tr;

/* static */
auto Spti::initialize() -> bool {
    Cdrom::di_list.clear();
    Cdrom::di_list = Spti::scanBus();
    std::string full_drive_name{};
    for (auto& di : Cdrom::di_list) {
        full_drive_name = di.letter;
        full_drive_name += " ";
        full_drive_name += di.name;
        Cdrom::scsi_drives_list.push_back(full_drive_name);
    }

    return true;
}

/* static */
void Spti::shutdown() {}

/* static */
auto Spti::scanBus() -> std::vector<ScsiDriveInfo> {
    std::vector<ScsiDriveInfo> drives;
    std::string                path = "C:\\";

    ScsiDriveInfo unselected{};
    unselected.letter = "";
    unselected.path   = -1;
    unselected.target = -1;
    unselected.lun    = -1;
    unselected.name   = core::tr("Not selected");
    drives.push_back(unselected);

    for (path[0] = 'C'; path[0] <= 'Z'; ++path[0]) {
        if (GetDriveTypeA(path.c_str()) == DRIVE_CDROM) {
            HANDLE drive_handle = Scsi::openDrive(path[0]);
            if (drive_handle != INVALID_HANDLE_VALUE) {
                ScsiDriveInfo di{};
                di.letter = path;
                Spti::getAdapterAddress(drive_handle, di);
                Spti::inquiry(drive_handle, di);
                drives.push_back(di);

                CloseHandle(drive_handle);
            }
        }
    }
    return drives;
}

/* static */
void Spti::test() {}

/* static */
auto Spti::readOneSector(const uint32_t& fad) -> std::string {
    const u32                   real_fad{fad - 150};
    constexpr u32               sector_size{2048};
    constexpr u8                cdb_length{12};
    constexpr u8                timeout_value{60};
    constexpr u8                command_code{0xBE};
    constexpr u8                number_of_frames_to_read{1};
    constexpr u8                only_main_channel_data{0x10};
    std::array<s8, sector_size> output_buffer{};

    SCSI_PASS_THROUGH_DIRECT input_buffer{};

    input_buffer.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
    input_buffer.CdbLength          = cdb_length;
    input_buffer.DataTransferLength = sector_size;
    input_buffer.TimeOutValue       = timeout_value;
    input_buffer.DataBuffer         = &output_buffer[0];
    input_buffer.SenseInfoLength    = 0;
    input_buffer.PathId             = Cdrom::scsi_path;
    input_buffer.TargetId           = Cdrom::scsi_target;
    input_buffer.Lun                = Cdrom::scsi_lun;
    input_buffer.SenseInfoOffset    = 0; // offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
    input_buffer.DataIn             = SCSI_IOCTL_DATA_IN;
    input_buffer.Cdb[index_0]       = command_code;
    input_buffer.Cdb[index_2]       = static_cast<u8>(real_fad >> displacement_24);
    input_buffer.Cdb[index_3]       = static_cast<u8>(real_fad >> displacement_16);
    input_buffer.Cdb[index_4]       = static_cast<u8>(real_fad >> displacement_8);
    input_buffer.Cdb[index_5]       = static_cast<u8>(real_fad);
    input_buffer.Cdb[index_6]       = 0;
    input_buffer.Cdb[index_7]       = 0;
    input_buffer.Cdb[index_8]       = number_of_frames_to_read;
    input_buffer.Cdb[index_9]       = only_main_channel_data;

    HANDLE drive_handle = Scsi::openDrive(Cdrom::scsi_letter);
    u32    dummy{};
    if (DeviceIoControl(
            drive_handle, IOCTL_STORAGE_CHECK_VERIFY, nullptr, 0, nullptr, 0, reinterpret_cast<LPDWORD>(&dummy), nullptr)
        == 0) {
        Log::warning("cdrom", tr("Drive isn't accessible"));

    } else {
        if (DeviceIoControl(drive_handle,
                            IOCTL_SCSI_PASS_THROUGH_DIRECT,
                            &input_buffer,
                            sizeof(input_buffer),
                            &input_buffer,
                            sizeof(input_buffer),
                            reinterpret_cast<LPDWORD>(&dummy),
                            nullptr)
            == 0) {
            Log::warning("cdrom", util::getLastErrorMessage());
        }
    }

    CloseHandle(drive_handle);
    std::string sector_data(output_buffer.begin(), output_buffer.end());
    return sector_data;
}

/* static */
auto Spti::readSector(const u32& fad, const s32& nb) -> std::string {
    std::string sector_data{};
    for (u8 cnt = 0; cnt < nb; ++cnt) {
        sector_data += readOneSector(fad);
    }

    return sector_data;
}

/* static */
void Spti::inquiry(const HANDLE& h, ScsiDriveInfo& di) {
    constexpr u8                       output_buffer_size{36};
    constexpr u8                       cdb_length{6};
    std::array<s8, output_buffer_size> output_buffer{};

    SCSI_PASS_THROUGH_DIRECT input_buffer{};

    input_buffer.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
    input_buffer.PathId             = di.path;
    input_buffer.DataIn             = SCSI_IOCTL_DATA_IN;
    input_buffer.TargetId           = di.target;
    input_buffer.Lun                = di.lun;
    input_buffer.DataTransferLength = output_buffer_size;
    input_buffer.DataBuffer         = &output_buffer[0];
    input_buffer.SenseInfoLength    = 0;
    input_buffer.CdbLength          = cdb_length;
    input_buffer.Cdb[index_0]       = scsi_inquiry;
    input_buffer.Cdb[index_4]       = output_buffer_size;
    input_buffer.TimeOutValue       = spti_timeout;

    uint32_t dummy{};
    if (DeviceIoControl(h,
                        IOCTL_SCSI_PASS_THROUGH_DIRECT,
                        &input_buffer,
                        sizeof(input_buffer),
                        &input_buffer,
                        sizeof(input_buffer),
                        reinterpret_cast<LPDWORD>(&dummy),
                        nullptr)
        == 0) {
        Log::warning("cdrom", util::getLastErrorMessage());

    } else {
        std::string  temp(output_buffer.begin(), output_buffer.end());
        constexpr u8 start_pos{8};
        constexpr u8 end_pos{28};
        di.name = temp.substr(start_pos, end_pos);
    }
}

/* static */
void Spti::getAdapterAddress(const HANDLE& h, ScsiDriveInfo& di) {
    // std::array<int8_t, 1024> v(1024);

    di.path   = -1;
    di.target = -1;
    di.lun    = -1;
    if (h == nullptr) { return; }

    // pSA = reinterpret_cast<PSCSI_ADDRESS>(&v[0]);
    SCSI_ADDRESS psa{};
    psa.Length = sizeof(SCSI_ADDRESS);

    u32 status{};
    if (DeviceIoControl(h,
                        IOCTL_SCSI_GET_ADDRESS,
                        nullptr,
                        0,
                        (LPVOID)&psa,
                        sizeof(SCSI_ADDRESS),
                        reinterpret_cast<LPDWORD>(&status),
                        nullptr)
        == 0) {
        return;
    }

    di.path   = psa.PortNumber;
    di.target = psa.TargetId;
    di.lun    = psa.Lun;
}

/* static */
auto Spti::readToc(ScsiToc& toc) -> bool {
    u32       out_bytes{};
    CDROM_TOC cdrom_toc;
    HANDLE    drive_handle = Scsi::openDrive(Cdrom::scsi_letter);
    if (DeviceIoControl(drive_handle,
                        IOCTL_CDROM_READ_TOC,
                        nullptr,
                        0,
                        &cdrom_toc,
                        sizeof(cdrom_toc),
                        reinterpret_cast<LPDWORD>(&out_bytes),
                        (LPOVERLAPPED) nullptr)
        == 0) {
        return false;
    }

    toc = reinterpret_cast<ScsiToc&>(cdrom_toc);
    return true;
}
} // namespace saturnin::cdrom

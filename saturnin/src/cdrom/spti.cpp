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

#include <saturnin/src/pch.h>
#include <saturnin/src/cdrom/spti.h>
#include <istream>
#include <ntddscsi.h>
#include <ntddcdrm.h>
#include <saturnin/src/locale.h> // tr
#include <saturnin/src/log.h>    // Log
#include <saturnin/src/utilities.h>
#include <saturnin/src/cdrom/cdrom.h>
#include <saturnin/src/cdrom/scsi.h>

namespace util = saturnin::utilities;

namespace saturnin::cdrom {

using core::Log;
using core::Logger;
using core::tr;

/* static */
auto Spti::initialize() -> bool {
    Cdrom::di_list.clear();
    Cdrom::di_list       = Spti::scanBus();
    auto full_drive_name = std::string{};
    for (const auto& di : Cdrom::di_list) {
        full_drive_name = di.letter;
        full_drive_name += " ";
        full_drive_name += di.name;
        Cdrom::scsi_drives_list.push_back(full_drive_name);
    }

    return true;
}

/* static */
void Spti::shutdown() { // nothing yet
}

/* static */
auto Spti::scanBus() -> std::vector<ScsiDriveInfo> {
    auto drives = std::vector<ScsiDriveInfo>{};
    auto path   = std::string{"C:\\"};

    auto unselected   = ScsiDriveInfo{};
    unselected.letter = "";
    unselected.path   = -1;
    unselected.target = -1;
    unselected.lun    = -1;
    unselected.name   = tr("Not selected");
    drives.push_back(unselected);

    for (path[0] = 'C'; path[0] <= 'Z'; ++path[0]) {
        if (GetDriveTypeA(path.c_str()) == DRIVE_CDROM) {
            auto drive_handle = Scsi::openDrive(path[0]);
            if (drive_handle != INVALID_HANDLE_VALUE) {
                auto di   = ScsiDriveInfo{};
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
auto Spti::readOneSector(const u32& fad) -> std::string {
    constexpr auto local_sector_size        = u32{2048};
    constexpr auto cdb_length               = u8{12};
    constexpr auto timeout_value            = u8{60};
    constexpr auto command_code             = u8{0xBE};
    constexpr auto number_of_frames_to_read = u8{1};
    constexpr auto only_main_channel_data   = u8{0x10};
    const auto     real_fad                 = u32{fad - 150};
    auto           output_buffer            = std::array<s8, local_sector_size>{};

    auto input_buffer = SCSI_PASS_THROUGH_DIRECT{};

    input_buffer.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
    input_buffer.CdbLength          = cdb_length;
    input_buffer.DataTransferLength = local_sector_size;
    input_buffer.TimeOutValue       = timeout_value;
    input_buffer.DataBuffer         = &output_buffer[0];
    input_buffer.SenseInfoLength    = 0;
    input_buffer.PathId             = Cdrom::scsi_path;
    input_buffer.TargetId           = Cdrom::scsi_target;
    input_buffer.Lun                = Cdrom::scsi_lun;
    input_buffer.SenseInfoOffset    = 0;
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

    const auto drive_handle = Scsi::openDrive(Cdrom::scsi_letter);
    auto       dummy        = u32{};
    if (DeviceIoControl(drive_handle,
                        IOCTL_STORAGE_CHECK_VERIFY,
                        nullptr,
                        0,
                        nullptr,
                        0,
                        reinterpret_cast<LPDWORD>(&dummy),
                        nullptr)
        == 0) {
        Log::warning(Logger::cdrom, tr("Drive isn't accessible"));

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
            Log::warning(Logger::cdrom, util::getLastErrorMessage());
        }
    }

    CloseHandle(drive_handle);
    const auto sector_data = std::string(output_buffer.begin(), output_buffer.end());
    return sector_data;
}

/* static */
auto Spti::readSector(const u32& fad, const s32& nb) -> std::string {
    auto sector_data = std::string{};
    for (u8 cnt = 0; cnt < nb; ++cnt) {
        sector_data += readOneSector(fad);
    }
    return sector_data;
}

/* static */
void Spti::inquiry(const HANDLE& h, ScsiDriveInfo& di) {
    constexpr auto output_buffer_size = u8{36};
    constexpr auto cdb_length         = u8{6};
    auto           output_buffer      = std::array<s8, output_buffer_size>{};

    auto input_buffer = SCSI_PASS_THROUGH_DIRECT{};

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

    auto dummy = u32{};
    if (DeviceIoControl(h,
                        IOCTL_SCSI_PASS_THROUGH_DIRECT,
                        &input_buffer,
                        sizeof(input_buffer),
                        &input_buffer,
                        sizeof(input_buffer),
                        reinterpret_cast<LPDWORD>(&dummy),
                        nullptr)
        == 0) {
        Log::warning(Logger::cdrom, util::getLastErrorMessage());

    } else {
        const auto     temp      = std::string(output_buffer.begin(), output_buffer.end());
        constexpr auto start_pos = u8{8};
        constexpr auto end_pos   = u8{28};
        di.name                  = temp.substr(start_pos, end_pos);
    }
}

/* static */
void Spti::getAdapterAddress(const HANDLE& h, ScsiDriveInfo& di) {
    di.path   = -1;
    di.target = -1;
    di.lun    = -1;
    if (h == nullptr) { return; }

    auto psa   = SCSI_ADDRESS{};
    psa.Length = sizeof(SCSI_ADDRESS);

    auto status = u32{};
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
    auto       out_bytes    = u32{};
    auto       cdrom_toc    = CDROM_TOC{};
    const auto drive_handle = Scsi::openDrive(Cdrom::scsi_letter);
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

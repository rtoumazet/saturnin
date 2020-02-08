//
// scsi.cpp
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

#include "scsi.h"
#include "aspi.h"
#include "spti.h"

namespace saturnin::cdrom {

// Static declarations
std::function<bool(void)>                                   Scsi::initialize;
std::function<std::vector<ScsiDriveInfo>(void)>             Scsi::scanBus;
std::function<std::string(const uint32_t&, const int32_t&)> Scsi::readSector;
std::function<void(void)>                                   Scsi::shutdown;
std::function<bool(ScsiToc& toc_data)>                      Scsi::readToc;

HANDLE Scsi::openDrive(const wchar_t letter) {
    HANDLE       h          = 0;
    std::wstring drive_name = L"\\\\.\\";
    drive_name += letter;
    drive_name += L":";

    h = CreateFile(
        drive_name.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE)
        return NULL;

    return h;
}

void Scsi::settingUpAspiFunctions() {
    initialize = &Aspi::initialize;
    scanBus    = &Aspi::scanBus;
    readSector = &Aspi::readSector;
    shutdown   = &Aspi::shutdown;
    readToc    = &Aspi::readToc;
}

void Scsi::settingUpSptiFunctions() {
    initialize = &Spti::initialize;
    scanBus    = &Spti::scanBus;
    readSector = &Spti::readSector;
    shutdown   = &Spti::shutdown;
    readToc    = &Spti::readToc;
}

} // namespace saturnin::cdrom

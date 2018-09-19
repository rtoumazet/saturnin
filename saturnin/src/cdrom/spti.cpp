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
#include <ntddscsi.h>
#include <api/ntddcdrm.h>
#include <scsidefs.h>
#include <boost/lexical_cast.hpp>
#pragma warning(pop) 

#include "cdrom.h"
#include "spti.h"

namespace saturnin {
namespace cdrom {

// to be removed when cdrom.cpp is added to the project
//SCSI_DriveInfo	CCdRom::diList[SCSI_MAX_DRIVES];
//int8_t			CCdRom::SCSI_Path = -1;
//int8_t			CCdRom::SCSI_Target = -1;
//int8_t			CCdRom::SCSI_Lun = -1;
//wchar_t			CCdRom::SCSI_Letter = 0;

bool Spti::initialize() {
    return true;
}

void Spti::shutdown() {

}

int32_t Spti::scanBus()
{
    // Scan of the system letters to get cdrom info
    wstring path = L"C:\\";
    HANDLE hCd = NULL;
    int8_t byIndex = 0;

    for (path[0] = 'C'; path[0] <= 'Z'; path[0]++)
    {
        if (GetDriveType(path.c_str()) == DRIVE_CDROM) // only cdrom drives are kept
        {
            hCd = Scsi::openDrive(path[0]); // getting a handle to the current drive
            if (hCd != INVALID_HANDLE_VALUE)
            {
                CCdRom::diList[byIndex].letter = path.c_str(); // adding the drive to the list
                Spti::getAdapterAddress(hCd, CCdRom::diList[byIndex]); // getting the address of the current letter drive

                if ((CCdRom::diList[byIndex].PATH != -1) && (CCdRom::diList[byIndex].TARGET != -1) && (CCdRom::diList[byIndex].LUN != -1))
                {
                    Spti::inquiry(hCd, CCdRom::diList[byIndex]); // filling the current structure with inquiry data
                }

                CloseHandle(hCd); // closing handle to the drive

                byIndex++; // moving to the next drive
            }
        }
    }
    return byIndex; // number of drives is returned
}

void Spti::test()
{

}

std::string Spti::readOneSector(const uint32_t & dwFAD)
{
    // Reads one sector
    const uint32_t sectorSize = 2048;
    uint32_t dwRealFAD = dwFAD - 150;
    std::vector<int8_t> v(sectorSize);
    uint32_t Dummy;

    SCSI_PASS_THROUGH_DIRECT sptIOCTL;

    memset(&sptIOCTL, 0, sizeof(sptIOCTL));

    sptIOCTL.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    sptIOCTL.CdbLength = 12; // pSC->SRB_CDBLen;
    sptIOCTL.DataTransferLength = sectorSize; // pSC->SRB_BufLen;
    sptIOCTL.TimeOutValue = 60;
    sptIOCTL.DataBuffer = &v[0];
    sptIOCTL.SenseInfoLength = 0;
    sptIOCTL.PathId = CCdRom::SCSI_Path;
    sptIOCTL.TargetId = CCdRom::SCSI_Target; // pSC->SRB_Target;
    sptIOCTL.Lun = CCdRom::SCSI_Lun; // pSC->SRB_Lun;
    sptIOCTL.SenseInfoOffset = 0; // offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, ucSenseBuf);
    sptIOCTL.DataIn = SCSI_IOCTL_DATA_IN;
    sptIOCTL.Cdb[0] = 0xBE;
    sptIOCTL.Cdb[2] = static_cast<uint8_t>(dwRealFAD >> 24);
    sptIOCTL.Cdb[3] = static_cast<uint8_t>(dwRealFAD >> 16);
    sptIOCTL.Cdb[4] = static_cast<uint8_t>(dwRealFAD >> 8);
    sptIOCTL.Cdb[5] = static_cast<uint8_t>(dwRealFAD);
    sptIOCTL.Cdb[6] = 0;
    sptIOCTL.Cdb[7] = 0;
    sptIOCTL.Cdb[8] = 1;                                    // one frame read
    sptIOCTL.Cdb[9] = 0x10;                                 // getting only main channel data


    HANDLE hDrive = Scsi::openDrive(CCdRom::SCSI_Letter);

    if (!DeviceIoControl(hDrive, IOCTL_STORAGE_CHECK_VERIFY,
                         NULL,
                         0,
                         NULL,
                         0,
                         reinterpret_cast<LPDWORD>(&Dummy),
                         NULL))
    {
        //MessageBox(NULL, L"Drive isn't accessible", NULL, MB_OK);

        MessageBox(NULL, qApp->tr("Drive isn't accessible").toStdWString().c_str(), NULL, MB_OK);

    }
    else {

        if (!DeviceIoControl(hDrive,
                             IOCTL_SCSI_PASS_THROUGH_DIRECT,
                             &sptIOCTL,
                             sizeof(sptIOCTL),
                             &sptIOCTL,
                             sizeof(sptIOCTL),
                             reinterpret_cast<LPDWORD>(&Dummy),
                             NULL))

        {

            //wstring str = L"DeviceIOControl error=" + lexical_cast<wstring>(GetLastError());
            //MessageBox(NULL, str.c_str(), L"SPTI error / read sector", MB_OK);
            std::wstring str = qApp->tr("DeviceIOControl error=").toStdWString() + lexical_cast<wstring>(GetLastError());
            MessageBox(NULL, str.c_str(), qApp->tr("SPTI error / read sector").toStdWString().c_str(), MB_OK);
        }
    }

    std::string strReturn(v.begin(), v.end());
    CloseHandle(hDrive);
    return strReturn; // returning the string
}

std::string Spti::readSector(const uint32_t & dwFAD, const int32_t & iNb)
{
    // reads sectors one at a time and returns the result in a string

    std::string sector_data{};

    for (uint8_t cnt = 0; cnt < iNb; ++cnt) {
        sector_data += readOneSector(dwFAD);
    }

    return sector_data;
}

void Spti::inquiry(const HANDLE & hHdl, SCSI_DriveInfo & di)
{
    SCSI_PASS_THROUGH_DIRECT sptIOCTL;
    vector<int8_t> v(36);
    uint32_t dummy;

    memset(&sptIOCTL, 0, sizeof(SCSI_PASS_THROUGH_DIRECT));

    sptIOCTL.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    sptIOCTL.PathId = di.PATH;
    sptIOCTL.DataIn = SCSI_IOCTL_DATA_IN;
    sptIOCTL.TargetId = di.TARGET;
    sptIOCTL.Lun = di.LUN;
    sptIOCTL.DataTransferLength = 36;
    sptIOCTL.DataBuffer = &v[0];
    sptIOCTL.SenseInfoLength = 0;
    sptIOCTL.CdbLength = 6;
    sptIOCTL.Cdb[0] = SCSI_INQUIRY;
    sptIOCTL.Cdb[4] = 36;  // allocation length per szBuffer [ ]
    sptIOCTL.TimeOutValue = SPTI_TIMEOUT; // timeout is set to 5 secs

    if (!DeviceIoControl(hHdl,
                         IOCTL_SCSI_PASS_THROUGH_DIRECT,
                         &sptIOCTL,
                         sizeof(sptIOCTL),
                         &sptIOCTL,
                         sizeof(sptIOCTL),
                         reinterpret_cast<LPDWORD>(&dummy),
                         NULL))
    {
        //wstring str = L"DeviceIOControl error="+lexical_cast<wstring>(GetLastError());
        //MessageBox(NULL, str.c_str(), L"SPTI error / Inquiry", MB_OK);
        wstring str = qApp->tr("DeviceIOControl error=").toStdWString() + lexical_cast<wstring>(GetLastError());
        MessageBox(NULL, str.c_str(), qApp->tr("SPTI error / Inquiry").toStdWString().c_str(), MB_OK);

    }
    else {
        wstring temp(v.begin(), v.end());
        di.name = temp.substr(8, 28);
    }
}

void Spti::getAdapterAddress(const HANDLE & hHdl, SCSI_DriveInfo & diDrive)
{
    vector<int8_t> v(1024);
    PSCSI_ADDRESS pSA;
    uint32_t dwRet;

    diDrive.PATH = -1;
    diDrive.TARGET = -1;
    diDrive.LUN = -1; // initializing the data
    if (hHdl == 0) return;

    pSA = reinterpret_cast<PSCSI_ADDRESS>(&v[0]);
    pSA->Length = sizeof(SCSI_ADDRESS);

    if (!DeviceIoControl(hHdl, IOCTL_SCSI_GET_ADDRESS, NULL, 0, pSA, sizeof(SCSI_ADDRESS), reinterpret_cast<LPDWORD>(&dwRet), NULL))  return;

    // Address is returned
    diDrive.PATH = pSA->PortNumber;
    diDrive.TARGET = pSA->TargetId;
    diDrive.LUN = pSA->Lun;
}

bool Spti::readToc(SCSI_TOC& TOCData)
{
    uint32_t dwOutBytes;
    CDROM_TOC CdromTOC;
    // getting the handle to the current drive
    HANDLE hDrive = Scsi::openDrive(CCdRom::SCSI_Letter);
    // sending the command
    if (!DeviceIoControl(hDrive,
                         IOCTL_CDROM_READ_TOC, NULL, 0,
                         &CdromTOC,
                         sizeof(CdromTOC),
                         reinterpret_cast<LPDWORD>(&dwOutBytes),
                         (LPOVERLAPPED)NULL))
    {
        // error
        return false;
    }

    TOCData = reinterpret_cast<SCSI_TOC&>(CdromTOC);
    return true;
}
}
}
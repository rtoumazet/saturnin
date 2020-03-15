//
// aspi.cpp
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
//#include <boost/lexical_cast.hpp>
#include <string>
#pragma warning(pop)

#include "cdrom.h"
#include "aspi.h"
#include "iso9660.h"
#include "../utilities.h"

namespace saturnin::cdrom {

// static declarations
HINSTANCE Aspi::hWnAspi32;
// function<uint32_t(void)>	Aspi::pfGetAspi32SupportInfo;
// function<uint32_t(LPSRB)>	Aspi::pfSendAspi32Command;

auto Aspi::initialize() -> bool {
    ////hinstWNASPI32=LoadLibrary("WNASPI32");
    // hWnAspi32 = LoadLibrary(L"WNASPI32");
    // if (!hWnAspi32)
    //{
    //    MessageBox(NULL, GetString(L"ASPIMessageDllNotFound").c_str(), GetString(L"ASPIMessageCaption").c_str(), MB_ICONSTOP);
    //    return false;
    //}
    ////	pfGetASPI32SupportInfo=(unsigned long(_cdecl*)(void))GetProcAddress(hWnAspi32,"GetASPI32SupportInfo");
    // pfGetAspi32SupportInfo = GetProcAddress(hWnAspi32, "GetASPI32SupportInfo");
    // if (pfGetAspi32SupportInfo == NULL)
    //{
    //    MessageBox(NULL, GetString(L"AspiMessageGA32SINotFound").c_str(), GetString(L"ASPIMessageCaption").c_str(),
    //    MB_ICONSTOP); return false;
    //}

    ////pfSendAspi32Command=(unsigned long (_cdecl*)(LPSRB))GetProcAddress(hWnAspi32,"SendASPI32Command");
    ////pfSendAspi32Command = (unsigned long (_cdecl*)(LPSRB))GetProcAddress(hWnAspi32,"SendASPI32Command");
    // pfSendAspi32Command = reinterpret_cast<func>(GetProcAddress(hWnAspi32, "SendASPI32Command"));
    // if (pfSendAspi32Command == NULL)
    //{
    //    MessageBox(NULL, GetString(L"AspiMessageSA32CNotFound").c_str(), GetString(L"ASPIMessageCaption").c_str(), MB_ICONSTOP);
    //    return false;
    //}

    // uint32_t dwSupportInfo = pfGetAspi32SupportInfo();
    // uint8_t  byHaCount;
    // uint8_t  byASPIStatus;

    // byHaCount = static_cast<uint8_t>(dwSupportInfo);
    // byASPIStatus = static_cast<uint8_t>(dwSupportInfo >> 8);

    // switch (byASPIStatus)
    //{
    //    case SS_COMP:
    //        // ASPI support OK
    //        break;
    //    case SS_NO_ADAPTERS:
    //        MessageBox(NULL, GetString(L"AspiMessageNoSCSIDevice").c_str(), GetString(L"AspiMessageCaption").c_str(),
    //        MB_ICONSTOP); return false;
    //    case SS_NO_ASPI:
    //        MessageBox(NULL, GetString(L"AspiMessageNoAspiManager").c_str(), GetString(L"AspiMessageCaption").c_str(),
    //        MB_ICONSTOP); return false;
    //    case SS_ILLEGAL_MODE:
    //        MessageBox(NULL, GetString(L"AspiMessageNoRealMode").c_str(), GetString(L"AspiMessageCaption").c_str(),
    //        MB_ICONSTOP); return false;
    //    case SS_OLD_MANAGER:
    //        MessageBox(NULL, GetString(L"AspiMessageAspiTooOld").c_str(), GetString(L"AspiMessageCaption").c_str(),
    //        MB_ICONSTOP); return false;
    //    default:
    //        MessageBox(NULL, GetString(L"AspiMessageErrorInitialization").c_str(), GetString(L"AspiMessageCaption").c_str(),
    //        MB_ICONSTOP); return false;
    //}

    // Aspi::setTimeOut(); // timeout setting
    return true;
}

void Aspi::shutdown() {
    //    FreeLibrary(hWnAspi32);
}

auto Aspi::scanBus() -> std::vector<ScsiDriveInfo> {
    // int8_t byHaId;
    // int8_t byTarget;
    // SRB_GDEVBlock srbGDEVBlock;

    // int32_t iTotalDrives = 0;

    // for (byHaId = 0; byHaId < 8; byHaId++)
    //{
    //    for (byTarget = 0; byTarget < 8; byTarget++)
    //    {
    //        memset(&srbGDEVBlock, 0, sizeof(SRB_GDEVBlock));
    //        srbGDEVBlock.SRB_Cmd = SC_GET_DEV_TYPE;
    //        srbGDEVBlock.SRB_HaId = byHaId;
    //        srbGDEVBlock.SRB_Target = byTarget;
    //        //pfSendASPI32Command((LPSRB)&srbGDEVBlock);
    //        pfSendAspi32Command(static_cast<LPSRB>(&srbGDEVBlock));
    //        if (srbGDEVBlock.SRB_Status != SS_COMP) continue;
    //        if (srbGDEVBlock.SRB_DeviceType == DTYPE_CDROM)
    //        {
    //            // A CD-ROM exists at HA/ID/LUN = byHaId/byTarget/0.
    //            CCdRom::diList[iTotalDrives].PATH = byHaId;
    //            CCdRom::diList[iTotalDrives].TARGET = byTarget;
    //            CCdRom::diList[iTotalDrives].LUN = 0;
    //            // getting data from the drive
    //            Aspi::inquiry(CCdRom::diList[iTotalDrives]);

    //            CCdRom::diList[iTotalDrives].letter = lexical_cast<wstring>(static_cast<int32_t>(byHaId));
    //            CCdRom::diList[iTotalDrives].letter += L":";
    //            CCdRom::diList[iTotalDrives].letter += lexical_cast<wstring>(static_cast<int32_t>(byTarget));
    //            CCdRom::diList[iTotalDrives].letter += L":0";

    //            iTotalDrives++;
    //        }
    //    }
    //}
    // return iTotalDrives;
    return std::vector<ScsiDriveInfo>{};
}

void Aspi::inquiry(ScsiDriveInfo& di) {
    // vector<int8_t> v(36);
    // SRB_ExecSCSICmd srbExec;

    // memset(&srbExec, 0, sizeof(SRB_ExecSCSICmd));
    // srbExec.SRB_Cmd = SC_EXEC_SCSI_CMD;
    // srbExec.SRB_HaId = di.PATH;
    // srbExec.SRB_Flags = SRB_DIR_IN;
    // srbExec.SRB_Target = di.TARGET;
    // srbExec.SRB_Lun = di.LUN;
    // srbExec.SRB_BufLen = 36;
    // srbExec.SRB_BufPointer = reinterpret_cast<uint8_t*>(&v[0]);;
    // srbExec.SRB_SenseLen = SENSE_LEN;
    // srbExec.SRB_CDBLen = 6;
    // srbExec.CDBByte[0] = SCSI_INQUIRY;
    // srbExec.CDBByte[4] = 36;  // allocation length per szBuffer [ ]

    // pfSendAspi32Command(static_cast<LPSRB>(&srbExec));

    // while (srbExec.SRB_Status == SS_PENDING) Sleep(10);

    // if (srbExec.SRB_Status != SS_COMP)
    //{
    //    MessageBox(NULL, GetString(L"AspiMessageInquiryError").c_str(), GetString(L"ASPIMessageCaption").c_str(), MB_ICONSTOP);
    //}
    // else {
    //    wstring temp(v.begin(), v.end()); // putting the buffer in a string
    //    di.name = temp.substr(8, 28); // getting data past null characters
    //}
    ////delete[] byBuffer;
}

auto Aspi::readSector(const uint32_t& dwFAD, const int32_t& iNb) -> std::string {
    //// CDROM fad
    // uint32_t dwRealFAD = dwFAD - 150;
    // vector<int8_t> v(2048 * iNb);

    // uint32_t dwASPIStatus;
    // SRB_ExecSCSICmd srbExec;

    // memset(&srbExec, 0, sizeof(SRB_ExecSCSICmd));
    // srbExec.SRB_Cmd = SC_EXEC_SCSI_CMD;
    // srbExec.SRB_HaId = CCdRom::SCSI_Path;
    // srbExec.SRB_Flags = SRB_DIR_IN;
    // srbExec.SRB_Target = CCdRom::SCSI_Target;
    // srbExec.SRB_Lun = CCdRom::SCSI_Lun;
    // srbExec.SRB_BufLen = 2048 * iNb;
    // srbExec.SRB_BufPointer = reinterpret_cast<uint8_t*>(&v[0]);
    // srbExec.SRB_SenseLen = SENSE_LEN;
    // srbExec.SRB_CDBLen = 12;
    // srbExec.CDBByte[0] = 0xBE;
    // srbExec.CDBByte[2] = static_cast<uint8_t>(dwRealFAD >> 24);
    // srbExec.CDBByte[3] = static_cast<uint8_t>(dwRealFAD >> 16);
    // srbExec.CDBByte[4] = static_cast<uint8_t>(dwRealFAD >> 8);
    // srbExec.CDBByte[5] = static_cast<uint8_t>(dwRealFAD);
    // srbExec.CDBByte[6] = static_cast<uint8_t>(iNb >> 16);// read number frames
    // srbExec.CDBByte[7] = static_cast<uint8_t>(iNb >> 8);
    // srbExec.CDBByte[8] = static_cast<uint8_t>(iNb);
    // srbExec.CDBByte[9] = 0x10;

    // dwASPIStatus = pfSendAspi32Command((LPSRB)&srbExec);

    // while (srbExec.SRB_Status == SS_PENDING) {
    //    Sleep(50);
    //}

    // string str_return(v.begin(), v.end());
    // return str_return; // returning the string
    std::string str{};

    return str;
}

void Aspi::setTimeOut() {
    //#ifdef _LOGS
    //    //pLog->CdBlockWrite("Setting ASPI timeout");
    //#endif
    //    SRB_GetSetTimeouts srbTimeout;
    //    memset(&srbTimeout, 0, sizeof(SRB_GetSetTimeouts));
    //    srbTimeout.SRB_Cmd = SC_GETSET_TIMEOUTS;
    //    srbTimeout.SRB_HaId = CCdRom::SCSI_Path;
    //    srbTimeout.SRB_Flags = SRB_DIR_OUT;
    //    srbTimeout.SRB_Target = CCdRom::SCSI_Target;
    //    srbTimeout.SRB_Lun = CCdRom::SCSI_Lun;
    //    srbTimeout.SRB_Timeout = 5 * 2; // 5 seconds
    //    pfSendAspi32Command(static_cast<LPSRB>(&srbTimeout));
    //
    //    while (srbTimeout.SRB_Status == SS_PENDING) Sleep(10);
    //
    //    if (srbTimeout.SRB_Status == SS_COMP)
    //    {
    //#ifdef _LOGS
    //        //pLog->CdBlockWrite("Timeout set to 5 second");
    //#endif
    //    }
    //    else
    //    {
    //#ifdef _LOGS
    //        //pLog->CdBlockWrite("Error!");
    //        //pLog->CdBlockWrite("srbTimeout.SRB_Status= 0x",srbTimeout.SRB_Status);
    //#endif
    //    }
}

void Aspi::resetBus() {
    //    HANDLE heventSRB;
    //    SRB_BusDeviceReset srbReset;
    //
    //    heventSRB = CreateEvent(NULL, TRUE, FALSE, NULL);
    //    if (!heventSRB) {
    //#ifdef _LOGS
    //        //pLog->CdBlockWrite("Error : ASPI event not created!");
    //#endif
    //    }
    //    else {
    //
    //        memset(&srbReset, 0, sizeof(SRB_BusDeviceReset));
    //        srbReset.SRB_Cmd = SC_RESET_DEV;
    //        srbReset.SRB_HaId = CCdRom::SCSI_Path;
    //        srbReset.SRB_Flags = SRB_EVENT_NOTIFY;
    //        srbReset.SRB_Target = CCdRom::SCSI_Target;
    //        srbReset.SRB_PostProc = (LPVOID)heventSRB;
    //
    //        ResetEvent(heventSRB);
    //        pfSendAspi32Command(static_cast<LPSRB>(&srbReset));
    //
    //        if (srbReset.SRB_Status == SS_PENDING) {
    //            WaitForSingleObject(heventSRB, 500);
    //        }
    //        if (srbReset.SRB_Status == SS_COMP)
    //        {
    //#ifdef _LOGS
    //            //pLog->CdBlockWrite("Reset OK!");
    //#endif
    //        }
    //        else
    //        {
    //#ifdef _LOGS
    //            //pLog->CdBlockWrite("Error!");
    //            //pLog->CdBlockWrite("srbReset.SRB_Status= 0x",srbReset.SRB_Status);
    //            //pLog->CdBlockWrite("srbReset.SRB_HaStat= 0x",srbReset.SRB_HaStat);
    //            //pLog->CdBlockWrite("srbReset.SRB_TargStat= 0x",srbReset.SRB_TargStat);
    //#endif
    //        }
    //        CloseHandle(heventSRB);
    //    }
}

auto Aspi::readToc(ScsiToc& toc_data) -> bool {
    ////pLog->CdBlockWrite("Trying to read CD TOC..");
    ////BYTE TOCBuffer[1024];
    // uint8_t TOCBuffer[1024];

    // SRB_ExecSCSICmd srbExec;
    // memset(&srbExec, 0, sizeof(SRB_ExecSCSICmd));

    // srbExec.SRB_Cmd = SC_EXEC_SCSI_CMD;
    // srbExec.SRB_HaId = CCdRom::SCSI_Path;
    // srbExec.SRB_Flags = SRB_DIR_IN;
    // srbExec.SRB_Target = CCdRom::SCSI_Target;
    // srbExec.SRB_Lun = CCdRom::SCSI_Lun;
    // srbExec.SRB_BufLen = 1024;
    // srbExec.SRB_BufPointer = TOCBuffer;
    // srbExec.SRB_SenseLen = SENSE_LEN;
    // srbExec.SRB_CDBLen = 10;
    // srbExec.CDBByte[0] = SCSI_READ_TOC;
    // srbExec.CDBByte[2] = 0x00;
    // srbExec.CDBByte[7] = sizeof(Aspi_TOC) >> 8;
    // srbExec.CDBByte[8] = sizeof(Aspi_TOC) & 0xFF;
    // pfSendAspi32Command((LPSRB)&srbExec);

    // while (srbExec.SRB_Status == SS_PENDING) Sleep(10);

    // if (srbExec.SRB_Status == SS_COMP)
    //{
    //    TOCData = reinterpret_cast<SCSI_TOC &>(TOCBuffer);
    //    return true;
    //}
    // else
    //{
    //    //pLog->CdBlockWrite("*TOC reading failed!*");
    //    return false;
    //}
    return true;
}

} // namespace saturnin::cdrom

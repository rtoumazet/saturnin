//
// cdrom.cpp
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
#include <saturnin/src/cdrom/cdrom.h>
#include <windows.h>
#include <istream>
#include <saturnin/src/emulator_context.h>
#include <saturnin/src/locale.h>
#include <saturnin/src/log.h> // Log
#include <saturnin/src/smpc.h>
#include <saturnin/src/utilities.h> // toUnderlying
#include <saturnin/src/cdrom/scsi.h>

namespace saturnin::cdrom {

using core::Log;
using core::Logger;
using core::tr;

// Static variables initialization
CdromAccessMethod Cdrom::access_method = CdromAccessMethod::spti;

// ScsiDriveInfo	    Cdrom::di_list[scsi_max_drives];
std::vector<ScsiDriveInfo> Cdrom::di_list;
s8                         Cdrom::scsi_path   = -1;
s8                         Cdrom::scsi_target = -1;
s8                         Cdrom::scsi_lun    = -1;
wchar_t                    Cdrom::scsi_letter = 0;

ScsiToc Cdrom::toc_data;

std::vector<std::string> Cdrom::scsi_drives_list = {};

//--------------------------------------------------------------------------------------------------------------
// PUBLIC section
//--------------------------------------------------------------------------------------------------------------

/* static */
auto Cdrom::getDriveIndice(const s8 path, const s8 target, const s8 lun) -> u8 {
    const auto it = std::find_if(di_list.begin(), di_list.end(), [path, target, lun](const ScsiDriveInfo& di) {
        return (di.path == path) && (di.target == target) && (di.lun == lun);
    });
    return static_cast<u8>(it - di_list.begin()); // returns the indice
}

// CCdRom::CCdRom()
//{
//	EmuState::pMem->countryCode = 0xC; // Europe by default
//
//	Scsi::initialize();
//
//	// CD-BLOCK registers initialization
//	HIRQREQ=0x07D3;
//	HIRQMSK=0xFFFF;
//	CR1='C';
//	CR2=('D'<< 8)|'B';
//	CR3=('L'<< 8)|'O';
//	CR4=('C'<< 8)|'K';
//
//	writingCRRegs=false;
//	firstReading=true;
//
//	counter=0;
//	driveSpeed = STANDBY;
//	executedCommandsMax = 60;
//	standbyTime=180;// 180s default standby time
//	ECCFreq=1;// max of 1 time in real time
//	retryFreq=RETRY_FREQ_NO_RETRY;// by default no retry when sector read error
//	getSectorLength=2048;
//	putSectorLength=2048;
//
//	flag=0;
//	repCnt=0;
//	ctrlAdr=0;
//	tno=1;
//	ino=1;
//	FAD=0x96;
//
//	if(EmuState::currentHardwareMode == SATURN){
//		(CdInserted()) ? cdDriveStatus=STAT_PAUSE:cdDriveStatus=STAT_NODISC;
//	}else{
//		cdDriveStatus = STAT_NODISC;
//	}
//
//	currentFilter=FILTER_NOT_CONNECTED;
//
//	bytesTransfered=0;
//	posInDataBuffer=0;
//	dataBuffer=saturnTOC;
//	dataBufferSize=0;
//	actualSize=0;
//
//	sectorBurstSize=1;
//	fetchedBuffer=0;
//	lastFetchedBuffer=0;
//	sectorNumInBuffer=0;
//	posInSector=0;
//	DeleteSectorDataExecuted=false;
//	numberOfSectorsToDelete=0;
//	posOfSectorsToDelete=0;
//
//	// sectors initialisation
//	for (int32_t i=0;i<MAX_SECTORS;i++)
//	{
//		memset(sectorsBuffer[i].data,0,SECTOR_SIZE);
//		sectorsBuffer[i].size=0;
//		sectorsBuffer[i].FAD=INVALID_FAD;
//		sectorsBuffer[i].fileNumber=0;
//		sectorsBuffer[i].channelNumber=0;
//		sectorsBuffer[i].subMode=0;
//		sectorsBuffer[i].codingInfo=0;
//	}
//
//	// selectors initialisation
//	for (int32_t i=0;i<MAX_SELECTORS;i++)
//	{
//		// filters
//		filters[i].trueOutput=i;
//		filters[i].falseOutput=FILTER_NOT_CONNECTED;
//		filters[i].mode=0;
//		filters[i].FAD=0;
//		filters[i].FADRange=0;
//		filters[i].fileNumber=0;
//		filters[i].channelNumber=0;
//		filters[i].subModeMask=0;
//		filters[i].subModeCompValue=0;
//		filters[i].codingInfoMask=0;
//		filters[i].codingInfoCompValue=0;
//
//		// buffer partitions
//		for (int32_t j=0;j<MAX_SECTORS;j++) bufferPartitions[i].sectors[j]=0;
//		bufferPartitions[i].size=0;
//	}
//
//	cyclesPerMs = 28571;
//	UpdatePeriod();
//
//	if(EmuState::currentHardwareMode == SATURN) InsertCd(); // CD is inserted only in saturn mode
//
//	executedCommands = 0;
//}
//
//
void Cdrom::executeCommand() {
    switch (cr1_.get(CommandRegister::command)) {
        case Command::get_status: getStatus(); break;
        case Command::get_hardware_info:
            getHardwareInfo();
            break;
            //		case 0x2: // Get TOC
            //			// Status(8) | Upper byte of word count(8)
            //			// Lower word of word count(16) (Max 204 words)
            //			// 0x0000
            //			// 0x0000
            //			cdDriveStatus|=STAT_TRNS;
            //			CR1=cdDriveStatus<<8 | 0x00;
            //			CR2=0x00CC;
            //			CR3=0x0000;
            //			CR4=0x0000;
            //
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get TOC=- executed");
            //			#endif
            //
            //			// building the saturn TOC
            //			Scsi::readToc(TOCData);
            //			//AspiTOC=(Aspi_TOC*)&TOCData;
            //			AspiTOC=reinterpret_cast<Aspi_TOC*>(&TOCData);
            //
            //			BuildSaturnTOC();
            //
            //			dataBuffer = saturnTOC;
            //			bytesTransfered = 0;
            //			posInDataBuffer = 0;
            //			dataBufferSize = 0xCC;
            //
            //			// flags setup
            //			HIRQREQ|=CMOK|DRDY;
            //
            //			break;
            //		case 0x3: // Get Session Info
            //			// Status(8) | 0x00
            //			// 0x0000
            //			// Upper word of sesson info(16)
            //			// Lower word of sesson info(16)
            //
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get Session Info=- executed");
            //			#endif
            //
            //			// only supports single session CD
            //			switch (CR1&0xFF)
            //			{
            //				case 0:
            //					{
            //						// lead out is 2 sec after the last track
            //						int32_t fad=(AspiTOC->track[AspiTOC->last].addr[1]<<16)|
            //								(AspiTOC->track[AspiTOC->last].addr[1]<<8)|
            //								AspiTOC->track[AspiTOC->last].addr[3];
            //						fad+=150;
            //						CR1=static_cast<uint16_t>(cdDriveStatus<<8);
            //						CR2=0x0000;
            //						CR3=0x0100|static_cast<uint16_t>(fad>>16);
            //						CR4=static_cast<uint16_t>(fad);
            //					}
            //					break;
            //				case 1:
            //					CR1=cdDriveStatus<<8;
            //					CR2=0x0000;
            //					CR3=0x0100;
            //					CR4=0x0000;
            //					break;
            //				default:
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("Invalid session number");
            //					#endif
            //					CR1=cdDriveStatus<<8;
            //					CR2=0x0000;
            //					CR3=0xFFFF;
            //					CR4=0xFFFF;
            //					break;
            //			}
            //
            //
            //			// flags setup
            //			HIRQREQ|=CMOK;
            //			break;
            //		case 0x4: // Initialize CD System
            //			// Status(8) | Flags(4) | Rep Cnt(4)
            //			// Ctrl Addr(8) | Track No(8)
            //			// Index No(8) | Upper Byte of curre,t FAD (8)
            //			// Lower word of current FAD
            //
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Initialize CD System=- executed");
            //
            //			if(!(CR1 & 0x80))
            //			{
            //				EmuState::pLog->CdBlockWrite("change initialization flag");
            //				(CR1&0x1) ? EmuState::pLog->CdBlockWrite("software reset"):EmuState::pLog->CdBlockWrite("no software
            // reset"); 				(CR1&0x2) ? EmuState::pLog->CdBlockWrite("decode subcode
            // RW"):EmuState::pLog->CdBlockWrite("no
            // decode subcode RW"); 				(CR1&0x4) ? EmuState::pLog->CdBlockWrite("no recognize mode 2
            // subheader"):EmuState::pLog->CdBlockWrite("recognize
            // mode 2 subheader"); 				(CR1&0x8) ? EmuState::pLog->CdBlockWrite("retry form 2
            // read"):EmuState::pLog->CdBlockWrite("no retry form 2 read"); 				(CR1&0x80) ?
            // EmuState::pLog->CdBlockWrite("no change"):EmuState::pLog->CdBlockWrite("change"); }else
            // EmuState::pLog->CdBlockWrite("don't change initialization flag"); #endif
            //
            //			switch(CR1 & 0x3)
            //			{
            //			case 0:
            //			case 2:
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("Drive speed : 2X");
            //				#endif
            //				driveSpeed = DBL_PLAY_SPEED;
            //				UpdatePeriod();
            //				break;
            //			case 1:
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("Drive speed : 1X");
            //				#endif
            //				driveSpeed = STD_PLAY_SPEED;
            //				UpdatePeriod();
            //				break;
            //			default:
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("Invalid drive speed");
            //				#endif
            //				break;
            //			}
            //
            //			// standby time
            //			switch (CR2)
            //			{
            //				case 0:
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("Default standby time (180s)");
            //					#endif
            //					standbyTime=180;
            //					break;
            //				case 0xFFFF:
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("No standby time change");
            //					#endif
            //					break;
            //				default:
            //					standbyTime=CR2;
            //					//log("new standby time: %ds\n",standbyTime);
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("New standby time : ",standbyTime);
            //					#endif
            //					break;
            //			}
            //
            //			// ECC setup
            //			switch (CR4>>8)
            //			{
            //				case 0:
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("Default ECC setup frequency (1 time)");
            //					#endif
            //					ECCFreq=1;
            //					break;
            //				case 0x80:
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("No ECC setup");
            //					#endif
            //					ECCFreq=0;
            //					break;
            //				case 0xFF:
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("No ECC setup frequency change");
            //					#endif
            //					break;
            //				default:
            //					ECCFreq=CR4>>8;
            //					if (ECCFreq<2) ECCFreq=2;
            //					else if (ECCFreq>6) ECCFreq=6;
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("ECC setup frequency changed : ",ECCFreq);
            //					#endif
            //					break;
            //			}
            //
            //			// retry frequency
            //			if (!(CR4&0xF0))
            //			{
            //				if (!(CR4&0x0F)) {
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("Stops data output without retrying when error");
            //					#endif
            //					retryFreq=RETRY_FREQ_NO_RETRY; }
            //				else {
            //					retryFreq=CR4&0xF;
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("No data output with new retry value when error");
            //					#endif
            //				}
            //			}
            //			else if (CR4&0x80)
            //			{
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("Infinite retry frequency");
            //				#endif
            //				retryFreq=RETRY_FREQ_INFINITE;
            //			}
            //			else if (CR4&0x40)
            //			{
            //				if (CR4==0x40) {
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("Data output without retry when error");
            //					#endif
            //					retryFreq=RETRY_FREQ_DATA_OUT_NO_RETRY; }
            //				else {
            //					retryFreq=CR4&0xF;
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("Data output with new retry value when error : ", retryFreq);
            //					#endif
            //				}
            //			}
            //			#ifdef _LOGS
            //			else if ((CR4&0xFF)==0xFF) EmuState::pLog->CdBlockWrite("No retry frequency change");
            //			#endif
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			SendStatus();
            //			break;
            //		case 0x5: // Open CD Tray
            //			// Status(8) | Flags(4) | Rep Cnt(4)
            //			// Ctrl Addr(8) | Track No(8)
            //			// Index No(8) | Upper Byte of current FAD (8)
            //			// Lower word of current FAD
            //
            //			// flags setup
            //			//HIrqReg = EmuState::pMem->ReadWord(0x25890008);
            //			//HIrqReg |= CMOK; // CMOK
            //			//EmuState::pMem->WriteWord(0x25890008,HIrqReg,EmuState::pSh2m->DisplayPC());
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockU("-=Open CD Tray=-");
            //			EmuState::pLog->CdBlockWrite("-=Open CD Tray=- executed (UNIMPLEMENTED)");
            //			#endif
            //			break;
        case Command::end_data_transfer: {
            endDataTransfer();
            break;
        }
            //		case 0x10: // Play Disc
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Play Disc=- executed");
            //			#endif
            //
            //			if (CR1==0x1000)
            //			{
            //				// track mode
            //				currentPlayMode=PLAY_MODE_TRACK;
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("track mode");
            //				EmuState::pLog->CdBlockU("track mode");
            //				#endif
            //
            //				HIRQREQ&=~SCDQ;
            //				HIRQREQ&=~CSCT;
            //				HIRQREQ&=~PEND;
            //				HIRQREQ&=~DRDY;
            //				cdDriveStatus &=~STAT_TRNS;
            //
            //			}
            //			else
            //			{
            //				// FAD mode
            //				currentPlayMode=PLAY_MODE_FAD;
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("FAD mode\n");
            //				#endif
            //				uint32_t newFAD=((CR1&0x7F)<<16)|CR2;
            //				uint32_t numOfFAD=CR4;
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("FAD: 0x",newFAD);
            //				EmuState::pLog->CdBlockWrite("number of FADs to play through: ",numOfFAD);
            //				#endif
            //
            //				if (numOfFAD==0xFFFF) MessageBox(NULL,L"unimplemented",L"unimplemented",MB_ICONWARNING);
            //
            //				if (numOfFAD)
            //				{
            //					// play
            //					cdDriveStatus=STAT_PLAY;
            //					flag=FLAG_CDROM;
            //
            //					HIRQREQ&=~SCDQ;
            //					HIRQREQ&=~CSCT;
            //					HIRQREQ&=~PEND;
            //					HIRQREQ&=~DRDY;
            //
            //					remainingFADs=numOfFAD;
            //					FAD=newFAD;
            //					playFADStart=FAD;
            //				}
            //				else
            //				{
            //					// pause
            //					cdDriveStatus=STAT_PAUSE;
            //
            //					HIRQREQ&=~SCDQ;
            //					HIRQREQ&=~CSCT;
            //					HIRQREQ|=PEND;
            //				}
            //			}
            //
            //			uint8_t playMode;
            //			playMode=CR3>>8;
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("play mode: 0x",playMode);
            //			#endif
            //
            //			if (playMode==CDC_PM_NOCHG)
            //			{
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("no change in play mode");
            //				#endif
            //			}
            //			else
            //			{
            //				switch (playMode&0x7F)
            //				{
            //					case 0x00:
            //						#ifdef _LOGS
            //						EmuState::pLog->CdBlockWrite("no repeat (plays only one time)");
            //						#endif
            //						repCnt=0;
            //						break;
            //					case 0x0F:
            //						#ifdef _LOGS
            //						EmuState::pLog->CdBlockWrite("infinite repetition");
            //						#endif
            //						repCnt=0xF;
            //						break;
            //					case CDC_PM_REP_NOCHG:
            //						#ifdef _LOGS
            //						EmuState::pLog->CdBlockWrite("doesn't change maximum repeat count");
            //						#endif
            //						break;
            //					default:
            //						#ifdef _LOGS
            //						EmuState::pLog->CdBlockWrite("repeat count: ",playMode&0xF);
            //						#endif
            //						repCnt=playMode&0xF;
            //						break;
            //				}
            //				// pickup move?
            //				#ifdef _LOGS
            //				if (playMode&CDC_PM_PIC_NOCHG)
            //				{
            //					EmuState::pLog->CdBlockWrite("doesn't move pickup to start position");
            //				}
            //				else
            //				{
            //					EmuState::pLog->CdBlockWrite("play again");
            //				}
            //				#endif
            //			}
            //
            //			HIRQREQ|=CMOK;
            //			SendStatus();
            //			break;
            //		case 0x11: // Disc Seek
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Disc Seek=- executed");
            //			#endif
            //
            //			if ((CR1&0xFF)==0xFF)
            //			{
            //				// pause
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("pause");
            //				#endif
            //				cdDriveStatus=STAT_PAUSE;
            //				uint32_t  newFad=((CR1&0x7F)<<16)|CR2;
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("fad mode");
            //				EmuState::pLog->CdBlockWrite("fad: 0x",newFad);
            //				#endif
            //				if (newFad==0x7FFFFF)
            //				{
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("seek home directory");
            //					#endif
            //					//FAD=150;
            //					FAD = 0;
            //					// debug
            //					tno=1;
            //				}
            //				else FAD=newFad;
            //				// stop current transfert if any
            //				if (remainingFADs)
            //				{
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("cancel current play!");
            //					#endif
            //					remainingFADs=0;
            //				}
            //				HIRQREQ|=PEND|DRDY;
            //				HIRQREQ|=EHST|EFLS;
            //				cdDriveStatus |=STAT_TRNS;
            //			}
            //			else if (CR1==0x1100)
            //			{
            //				cdDriveStatus=STAT_PAUSE;
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("track mode");
            //				EmuState::pLog->CdBlockWrite("start track: ",CR2>>8);
            //				EmuState::pLog->CdBlockWrite("start index: ",CR2&0xFF);
            //				#endif
            //
            //				tno=CR2>>8;
            //				// set corresponding fad
            //				uint32_t newFAD;
            //				newFAD=saturnTOC[(tno-1)*4+1];
            //				newFAD<<=8;
            //				newFAD|=saturnTOC[(tno-1)*4+2];
            //				newFAD<<=8;
            //				newFAD|=saturnTOC[(tno-1)*4+3];
            //
            //				FAD=newFAD;
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("start track: 0x",newFAD);
            //				#endif
            //			}
            //			else
            //			{
            //				cdDriveStatus=STAT_PAUSE;
            //				uint32_t newFad=((CR1&0x7F)<<16)|CR2;
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("fad mode");
            //				EmuState::pLog->CdBlockWrite("fad: 0x",newFad);
            //				#endif
            //				if (newFad==0x7FFFFF)
            //				{
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("seek home directory");
            //					#endif
            //					FAD=150;
            //				}
            //				else FAD=newFad;
            //				// stop current transfert if any
            //				if (remainingFADs)
            //				{
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("cancel current play!");
            //					#endif
            //					remainingFADs=0;
            //				}
            //				HIRQREQ|=PEND|DRDY;
            //				HIRQREQ|=EHST|EFLS;
            //				cdDriveStatus |=STAT_TRNS;
            //			}
            //
            //			HIRQREQ|=CMOK;
            //			SendStatus();
            //			break;
            //		case 0x12: // CD Scan
            //			MessageBox(NULL,L"CDScan",L"unimplemented",MB_ICONWARNING);
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockU("-=Cd Scan=-");
            //			EmuState::pLog->CdBlockWrite("-=Cd Scan=- executed");
            //			#endif
            //			break;
            //		case 0x20: // Get Current Subcode Q / RW
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get Current Subcode Q/RW=- executed");
            //			#endif
            //			if((CR2&0xFF)==0x01)
            //			{ // Subcode RW
            //				MessageBox(NULL,L"Executing GetCurrentSubcodeRW\n",L"not implemented",MB_ICONWARNING);
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockU("-=Get Current Subcode RW=-");
            //				#endif
            //			}else{ // Subcode Q
            //				cdDriveStatus|=STAT_TRNS;
            //				CR1=cdDriveStatus<<8;
            //				CR2=5;
            //				CR3=0;
            //				CR4=0;
            //
            //				HIRQREQ|=CMOK|DRDY;
            //			}
            //
            //			break;
            //		case 0x30: // Set CD Device Connection
            //			currentFilter=CR3>>8;
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("CD device connection set to filter n°",currentFilter);
            //			#endif
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			SendStatus();
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Set CD Device Connection=- executed");
            //			#endif
            //			break;
            //		case 0x31: // Get CD Device Connection
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get CD Device Connection=- executed");
            //			EmuState::pLog->CdBlockU("-=Get CD Device Connection=-");
            //			#endif
            //			break;
            //		case 0x32: // Get Last Buffer Destination
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get Last Buffer Destination=- executed");
            //			#endif
            //
            //			CR1=cdDriveStatus<<8;
            //			CR2=0;
            //			CR3=static_cast<uint16_t>(lastFetchedBuffer<<8);
            //			CR4=0;
            //
            //			//HIRQREQ|=CMOK|ESEL;
            //			HIRQREQ|=CMOK;
            //			break;
            //		case 0x40: // Set Filter Range
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Set Filter Range=- executed");
            //			#endif
            //
            //			uint8_t filterNum;
            //			filterNum=CR3>>8;
            //
            //			filters[filterNum].FAD=((CR1&0xFF)<<16)|CR2;
            //			filters[filterNum].FADRange=((CR3&0xFF)<<16)|CR4;
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			SendStatus();
            //			break;
            //		case 0x41: // Get Filter Range
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get Filter Range=- executed");
            //			EmuState::pLog->CdBlockU("-=Get Filter Range=-");
            //			#endif
            //			break;
            //		case 0x42: // Set Filter Subheader Conditions
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Set Filter Subheader Conditions=- executed");
            //			#endif
            //
            //			filterNum=CR3>>8;
            //
            //			filters[filterNum].fileNumber=CR3&0xFF;
            //			filters[filterNum].channelNumber=CR1&0xFF;
            //			filters[filterNum].subModeMask=CR2>>8;
            //			filters[filterNum].subModeCompValue=CR4>>8;
            //			filters[filterNum].codingInfoMask=CR2&0xFF;
            //			filters[filterNum].codingInfoCompValue=CR4&0xFF;
            //
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("conditions:");
            //			EmuState::pLog->CdBlockWrite("fileNumber: 0x",filters[filterNum].fileNumber);
            //			EmuState::pLog->CdBlockWrite("channelNumber: 0x",filters[filterNum].channelNumber);
            //			EmuState::pLog->CdBlockWrite("subModeMask: 0x",filters[filterNum].subModeMask);
            //			EmuState::pLog->CdBlockWrite("subModeCompValue: 0x",filters[filterNum].subModeCompValue);
            //			EmuState::pLog->CdBlockWrite("codingInfoMask: 0x",filters[filterNum].codingInfoMask);
            //			EmuState::pLog->CdBlockWrite("codingInfoCompValue: 0x",filters[filterNum].codingInfoCompValue);
            //			#endif
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			SendStatus();
            //			break;
            //		case 0x43: // Get Filter Subheader Conditions
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get Filter Subheader Conditions=- executed");
            //			EmuState::pLog->CdBlockU("-=Get Filter Subheader Conditions=-");
            //			#endif
            //			break;
            //		case 0x44: // Set Filter Mode
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Set Filter Mode=- executed");
            //			#endif
            //			uint8_t mode;
            //			mode=CR1&0xFF;
            //			filterNum=CR3>>8;
            //
            //			if (mode&0x80)
            //			{
            //				// initialise filter conditions
            //				filters[filterNum].trueOutput=filterNum;
            //				filters[filterNum].falseOutput=FILTER_NOT_CONNECTED;
            //				filters[filterNum].mode=0;
            //				filters[filterNum].FAD=0;
            //				filters[filterNum].FADRange=0;
            //				filters[filterNum].fileNumber=0;
            //				filters[filterNum].channelNumber=0;
            //				filters[filterNum].subModeMask=0;
            //				filters[filterNum].subModeCompValue=0;
            //				filters[filterNum].codingInfoMask=0;
            //				filters[filterNum].codingInfoCompValue=0;
            //			}
            //			else filters[filterNum].mode=mode;
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			SendStatus();
            //			break;
            //		case 0x45: // Get Filter Mode
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Set Filter Mode=- executed");
            //			EmuState::pLog->CdBlockU("-=Set Filter Mode=-");
            //			#endif
            //			break;
            //		case 0x46: // Set Filter Connection
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Set Filter Connection=- executed");
            //			#endif
            //			uint8_t buffNum,falsec;
            //			filterNum=CR3>>8;
            //			buffNum=CR2>>8;
            //			falsec=CR2&0xFF;
            //
            //			if (CR1&0x1) filters[filterNum].trueOutput=buffNum;
            //			if (CR1&0x1) filters[filterNum].falseOutput=falsec;
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			SendStatus();
            //			break;
            //		case 0x47: // Get Filter Connection
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockU("-=Get Filter Connection=-");
            //			EmuState::pLog->CdBlockWrite("-=Get Filter Connection=- executed");
            //			#endif
            //			break;
            //		case 0x48: // Reset Selector
            //			// Status(8) | Flags(4) | Rep Cnt(4)
            //			// Ctrl Addr(8) | Track No(8)
            //			// Index No(8) | Upper Byte of curre,t FAD (8)
            //			// Lower word of current FAD
            //
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Reset Selector=- executed");
            //			#endif
            //
            //			uint8_t resetFlag;
            //			resetFlag=CR1&0xFF;
            //
            //			if (resetFlag==0)
            //			{
            //				// all specified buffer partition data is cleared
            //				int32_t buffPart=CR3>>8;
            //
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("Clearing buffer partition data of buffer n°",buffPart);
            //				#endif
            //
            //				for (int32_t j=0;j<MAX_SECTORS;j++)
            //				{
            //					if (bufferPartitions[buffPart].sectors[j])
            //					{
            //						// we erase corresponding sector in the sector buffer
            //						#ifdef _LOGS
            //						EmuState::pLog->CdBlockWrite("Erasing in sector buffer sector n°",j);
            //						#endif
            //
            //						memset(bufferPartitions[buffPart].sectors[j]->data,0,SECTOR_SIZE);
            //						bufferPartitions[buffPart].sectors[j]->size=0;
            //						bufferPartitions[buffPart].sectors[j]->FAD=INVALID_FAD;
            //						bufferPartitions[buffPart].sectors[j]->fileNumber=0;
            //						bufferPartitions[buffPart].sectors[j]->subMode=0;
            //						bufferPartitions[buffPart].sectors[j]->codingInfo=0;
            //
            //						bufferPartitions[buffPart].sectors[j]=NULL;
            //						bufferPartitions[buffPart].size--;
            //					}
            //				}
            //			}
            //			else
            //			{
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("Buffer partition number is ignored");
            //				#endif
            //
            //				if (resetFlag&0x4)
            //				{
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("Initialize data of all buffer partitions");
            //					#endif
            //					for (int32_t i=0;i<MAX_SELECTORS;i++)
            //					{
            //						for (int32_t j=0;j<MAX_SECTORS;j++) bufferPartitions[i].sectors[j]=NULL;
            //						bufferPartitions[i].size=0;
            //					}
            //					for (int32_t j=0;j<MAX_SECTORS;j++)
            //					{
            //						memset(sectorsBuffer[j].data,0,SECTOR_SIZE);
            //						sectorsBuffer[j].size=0;
            //						sectorsBuffer[j].FAD=INVALID_FAD;
            //						sectorsBuffer[j].fileNumber=0;
            //						sectorsBuffer[j].subMode=0;
            //						sectorsBuffer[j].codingInfo=0;
            //					}
            //				}
            //				if (resetFlag&0x8) EmuState::pLog->CdBlockWrite("Initialize data of all buffer partitions output
            // connectors (unsupported)");// ?? 				if (resetFlag&0x10)
            //				{
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("Initialize all filter conditions");
            //					#endif
            //					for (int32_t i=0;i<MAX_SELECTORS;i++)
            //					{
            //						filters[i].FAD=0;
            //						filters[i].FADRange=0;
            //						filters[i].fileNumber=0;
            //						filters[i].channelNumber=0;
            //						filters[i].subModeMask=0;
            //						filters[i].subModeCompValue=0;
            //						filters[i].codingInfoMask=0;
            //						filters[i].codingInfoCompValue=0;
            //					}
            //				}
            //				#ifdef _LOGS
            //				if (resetFlag&0x20) EmuState::pLog->CdBlockWrite("Initialize all filter input connectors
            //(unsupported)");//
            //?? 				#endif 				if (resetFlag&0x40)
            //				{
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("Initialize true output connectors of all filters");
            //					#endif
            //					for (int32_t i=0;i<MAX_SELECTORS;i++) filters[i].trueOutput=i;
            //				}
            //				if (resetFlag&0x80)
            //				{
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("Initialize false output connectors of all filters");
            //					#endif
            //					for (int32_t i=0;i<MAX_SELECTORS;i++) filters[i].falseOutput=FILTER_NOT_CONNECTED;
            //				}
            //			}
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			SendStatus();
            //			break;
            //		case 0x50: // Get CD Block size
            //			CR1=cdDriveStatus<<8;
            //			CR2=static_cast<uint16_t>(FreeNumOfSectorsInBuffer());
            //			CR3=0x18<<8;
            //			CR4=0xC8;
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			//HIRQREQ|=CMOK;
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get CD Block Size=- executed");
            //			EmuState::pLog->CdBlockWrite("number of free sectors: 0x",CR2);
            //			#endif
            //			break;
            //		case 0x51: // Get buffer size
            //			int32_t buffNum2;
            //			buffNum2=CR3>>8;
            //
            //			CR1=cdDriveStatus<<8;
            //			CR2=0x0000;
            //			CR3=0x0000;
            //			CR4=static_cast<uint16_t>(bufferPartitions[buffNum2].size);
            //
            //			if(CR4) cdDriveStatus |= STAT_TRNS;
            //			//HIRQREQ|=CMOK|ESEL;
            //			HIRQREQ|=CMOK;
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get buffer size=- executed");
            //			EmuState::pLog->CdBlockWrite("buffer n° 0x",buffNum2);
            //			EmuState::pLog->CdBlockWrite("size = 0x",bufferPartitions[buffNum2].size);
            //			#endif
            //			break;
            //		case 0x52: // Calculate actual size
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Calculate actual size=- executed");
            //			#endif
            //
            //			uint16_t sectorPosInBuffer;
            //			uint8_t buffer;
            //			uint16_t numberOfSectors;
            //
            //			sectorPosInBuffer=CR2;
            //			buffer=CR3>>8;
            //			numberOfSectors=CR4;
            //
            //			if (sectorPosInBuffer==0xFFFF) MessageBox(NULL,L"CalculateActualSize:
            // sectorPosInBuffer==0xFFFF",L"unimplemented",MB_ICONWARNING); 			if (numberOfSectors==0xFFFF)
            // MessageBox(NULL,L"CalculateActualSize: numberOfSectors==0xFFFF",L"unimplemented",MB_ICONWARNING);
            //
            //			actualSize=0;
            //			/*for (int32_t i=sectorPosInBuffer;i<(sectorPosInBuffer+numberOfSectors);i++)
            //			{
            //				if (bufferPartitions[buffer].sectors[i]) actualSize+=bufferPartitions[buffer].sectors[i]->size;
            //			}*/
            //
            //			// Is the range included in one partition
            //			int32_t i;
            //			if((sectorPosInBuffer+numberOfSectors) <= bufferPartitions[buffer].size){
            //				for(i=sectorPosInBuffer; i<(sectorPosInBuffer+numberOfSectors);i++){
            //					if(bufferPartitions[buffer].sectors[i]) actualSize+=bufferPartitions[buffer].sectors[i]->size;
            //				}
            //			}else{
            //				// Range exceeds one partition
            //				#ifdef _LOGS
            //				EmuState::pLog->CdBlockWrite("Range exceeded !!!");
            //				EmuState::pLog->CdBlockU("Range exceeded !!!");
            //				#endif
            //				if((sectorPosInBuffer+numberOfSectors) <= bufferPartitions[buffer].size){
            //					for(i=sectorPosInBuffer; i<(sectorPosInBuffer+numberOfSectors);i++){
            //						if(bufferPartitions[buffer].sectors[i]) actualSize+=bufferPartitions[buffer].sectors[i]->size;
            //					}
            //				}
            //			}
            //
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("buffer partition number: 0x",buffer);
            //			EmuState::pLog->CdBlockWrite("sector position: 0x",sectorPosInBuffer);
            //			EmuState::pLog->CdBlockWrite("sector number: 0x",numberOfSectors);
            //			EmuState::pLog->CdBlockWrite("actual size: 0x",actualSize);
            //			#endif
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			SendStatus();
            //			break;
            //		case 0x53: // Get actual block size
            //			CR1=cdDriveStatus<<8;
            //			CR1|=((actualSize/2)>>16)&0xFF;
            //			//CR2=(actualSize/2)&0xFFFF;
            //			CR2=static_cast<uint16_t>(actualSize/2);
            //			CR3=0;
            //			CR4=0;
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get actual block size=- executed");
            //			#endif
            //			break;
            //		case 0x54: // Get sector info (added 09/01/2005)
            //			uint8_t sectorNum;
            //			sectorNum = CR2 & 0xFF;
            //			uint8_t bufferNum;
            //			bufferNum = CR3>>8;
            //
            //			CR1=cdDriveStatus<<8;
            //			CR1|=bufferPartitions[bufferNum].sectors[sectorNum]->FAD>>16;
            //			CR2=(uint16_t)(bufferPartitions[bufferNum].sectors[sectorNum]->FAD & 0xFFFF);
            //			CR3=bufferPartitions[bufferNum].sectors[sectorNum]->fileNumber << 8 |
            // bufferPartitions[bufferNum].sectors[sectorNum]->channelNumber;
            // CR4=bufferPartitions[bufferNum].sectors[sectorNum]->subMode
            // << 8 | bufferPartitions[bufferNum].sectors[sectorNum]->codingInfo;
            //
            //			HIRQREQ|=CMOK|ESEL;
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get sector info=- executed");
            //			#endif
            //			break;
            //		case 0x55: // Execute FAD Search
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockU("-=Execute FAD Search=-");
            //			EmuState::pLog->CdBlockWrite("-=Execute FAD Search=- executed (UNIMPLEMENTED)");
            //			#endif
            //			break;
            //		case 0x56: // Get FAD Search Results
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockU("-=Get FAD Search Results=-");
            //			EmuState::pLog->CdBlockWrite("-=Get FAD Search Results=- executed  (UNIMPLEMENTEE)");
            //			#endif
            //			break;
            //		case 0x60: // Set Sector Length
            //			// Status(8) | Flags(4) | Rep Cnt(4)
            //			// Ctrl Addr(8) | Track No(8)
            //			// Index No(8) | Upper Byte of curre,t FAD (8)
            //			// Lower word of current FAD
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Set Sector Length=- executed");
            //			#endif
            //
            //			// get length
            //			switch (CR1&0xFF)
            //			{
            //				case SLEN_2048:
            //					getSectorLength=2048;
            //					break;
            //				case SLEN_2336:
            //					getSectorLength=2336;
            //					MessageBox(NULL,L"sector length set to 2336",L"unimplemented",MB_ICONWARNING);
            //					break;
            //				case SLEN_2340:
            //					getSectorLength=2340;
            //					MessageBox(NULL,L"sector length set to 2340",L"unimplemented",MB_ICONWARNING);
            //					break;
            //				case SLEN_2352:
            //					getSectorLength=2352;
            //					MessageBox(NULL,L"sector length set to 2352",L"unimplemented",MB_ICONWARNING);
            //					break;
            //				case 0xFF:
            //					break;
            //			}
            //			// set length
            //			switch (CR2>>8)
            //			{
            //				case SLEN_2048:
            //					putSectorLength=2048;
            //					break;
            //				case SLEN_2336:
            //					putSectorLength=2336;
            //					break;
            //				case SLEN_2340:
            //					putSectorLength=2340;
            //					break;
            //				case SLEN_2352:
            //					putSectorLength=2352;
            //					break;
            //				case 0xFF:
            //					break;
            //			}
            //			EmuState::pLog->CdBlockWrite("Get sector length: ",getSectorLength);
            //			EmuState::pLog->CdBlockWrite("put sector length: ",putSectorLength);
            //
            //			// flags setup
            //			HIRQREQ|=CMOK|ESEL;
            //			SendStatus();
            //			break;
            //		case 0x61: // Get Sector Data
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get Sector Data=- executed");
            //			#endif
            //
            //			int32_t sectorNumber;
            //
            //			sectorNumInBuffer=CR2;
            //			sectorNumber=CR4;
            //			fetchedBuffer=CR3>>8;
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("buffer partition number:0x",fetchedBuffer);
            //			EmuState::pLog->CdBlockWrite("sector position:0x",sectorNumInBuffer);
            //			EmuState::pLog->CdBlockWrite("sector number:0x",sectorNumber);
            //			#endif
            //
            //			// initiate data transfert from CD block to host
            //			posInSector=0;
            //			bytesTransfered=0;
            //
            //			cdDriveStatus|=STAT_TRNS;
            //			HIRQREQ|=CMOK|EHST|DRDY;// DRDY??
            //			SendStatus();
            //			break;
            //		case 0x62: // Delete Sector Data
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Delete Sector Data=- executed");
            //			#endif
            //
            //			sectorNumInBuffer=CR2;
            //			sectorNumber=CR4;
            //			fetchedBuffer=CR3>>8;
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("buffer partition number:0x",fetchedBuffer);
            //			EmuState::pLog->CdBlockWrite("sector position:0x",sectorNumInBuffer);
            //			EmuState::pLog->CdBlockWrite("sector number:0x",sectorNumber);
            //			#endif
            //
            //			// data must be deleted after
            //			numberOfSectorsToDelete=sectorNumber;
            //			posOfSectorsToDelete=sectorNumInBuffer;
            //
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("*deleting sectors in buffer partition n°0x",fetchedBuffer);
            //			EmuState::pLog->CdBlockWrite("old buffer size (in sectors): 0x",bufferPartitions[fetchedBuffer].size);
            //			EmuState::pLog->CdBlockWrite("position of sector to delete in buffer: 0x",posOfSectorsToDelete);
            //			EmuState::pLog->CdBlockWrite("number of sectors to delete: 0x",numberOfSectorsToDelete);
            //			#endif
            //			int32_t oldSize;
            //			oldSize=bufferPartitions[fetchedBuffer].size;
            //
            //			for (int32_t j=posOfSectorsToDelete; j<(posOfSectorsToDelete+numberOfSectorsToDelete); j++)
            //			{
            //				if (j>MAX_SECTORS)
            //				{
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("end of buffer partition reached");
            //					#endif
            //					break;
            //				}
            //				if (bufferPartitions[fetchedBuffer].sectors[j])
            //				{
            //					#ifdef _LOGS
            //					EmuState::pLog->CdBlockWrite("deleting in buffer partion sector n°",j);
            //					#endif
            //					bufferPartitions[fetchedBuffer].sectors[j]->size=0;
            //					bufferPartitions[fetchedBuffer].sectors[j]=NULL;
            //					bufferPartitions[fetchedBuffer].size--;
            //				}
            //				else break;
            //			}
            //
            //			// we shift the sectors in buffer partition if necessary
            //			if (numberOfSectorsToDelete<oldSize)
            //			{
            //				// we did not erase all the sectors
            //				// we check if there a hole in buffer partition
            //				EmuState::pLog->CdBlockWrite("all sectors haven't been deleted, checking for hole");
            //				int32_t holeStart=0;
            //				for (;holeStart<MAX_SECTORS;holeStart++)
            //				{
            //					if (!bufferPartitions[fetchedBuffer].sectors[holeStart]) break;
            //				}
            //				// holeStart contains the position of the hole
            //				// let's check if the hole is positioned before the end of used sectors
            //				if (holeStart<bufferPartitions[fetchedBuffer].size)
            //				{
            //					//log("there is a hole at sector n°%d\n",holeStart);
            //					// let's get the position of the end of the hole
            //					int32_t holeEnd=holeStart;
            //					for (;holeEnd<MAX_SECTORS;holeEnd++)
            //					{
            //						if (bufferPartitions[fetchedBuffer].sectors[holeEnd]) break;
            //					}
            //					// we shift the sectors
            //					int32_t k=0;
            //					//for (i=0;(i+holeEnd)<MAX_SECTORS;i++)
            //					for (k=0; (k+holeEnd)<(holeEnd+bufferPartitions[fetchedBuffer].size); k++)
            //					{
            //						if ((k+holeEnd)>MAX_SECTORS) break;
            //						bufferPartitions[fetchedBuffer].sectors[holeStart+k]=bufferPartitions[fetchedBuffer].sectors[holeEnd+k];
            //						//EmuState::pLog->CdBlockWrite("sector n°%d shifted to sector n°%d\n",holeEnd+i,holeStart+i);
            //					}
            //					for (;k<MAX_SECTORS;k++)
            //					{
            //						bufferPartitions[fetchedBuffer].sectors[k]=NULL;
            //					}
            //				}
            //			}
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("new buffer size: 0x",bufferPartitions[fetchedBuffer].size);
            //			#endif
            //			// buffer should not be full
            //			//HIRQREQ&=~BFUL;
            //
            //			HIRQREQ|=CMOK|EHST; //  DRDY ??
            //			SendStatus();
            //			break;
            //		case 0x63: // Get Then Delete Sector Data
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get Then Delete Sector Data=- executed");
            //			#endif
            //
            //			sectorNumInBuffer=CR2;
            //			sectorNumber=CR4;
            //			fetchedBuffer=CR3>>8;
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("Buffer partition number: ",fetchedBuffer);
            //			EmuState::pLog->CdBlockWrite("sector position: ",sectorNumInBuffer);
            //			EmuState::pLog->CdBlockWrite("sector number: ",sectorNumber);
            //			#endif
            //
            //			// initiate data transfert from CD block to host
            //			posInSector=0;
            //			bytesTransfered=0;
            //
            //			// data must be deleted after
            //			DeleteSectorDataExecuted=true;
            //			numberOfSectorsToDelete=sectorNumber;
            //			posOfSectorsToDelete=sectorNumInBuffer;
            //
            //			HIRQREQ|=CMOK|EHST|DRDY;
            //			cdDriveStatus|=STAT_TRNS;
            //			SendStatus();
            //			break;
            //		case 0x65: // Copy Sector Data
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockU("-=Copy Sector Data=-");
            //			EmuState::pLog->CdBlockWrite("-=Copy Sector Data=- executed (UNIMPLEMENTED)");
            //			#endif
            //			break;
            //		case 0x66: // Move Sector Data
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockU("-=Move Sector Data=-");
            //			EmuState::pLog->CdBlockWrite("-=Move Sector Data=- executed (UNIMPLEMENTED)");
            //			#endif
            //			break;
        case Command::get_copy_error:
            getCopyError();
            break;
            //		case 0x70: // Change Directory
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Change Directory=- executed");
            //			#endif
            //			int32_t fileId;
            //			fileId=((CR3&0xFF)<<16)|CR4;
            //
            //			switch (fileId)
            //			{
            //				case 0xFFFFFF:
            //					// move to root directory
            //					SendStatus();
            //					break;
            //				default:
            //					MessageBox(NULL,L"ChangeDirectory: change to another directory than root
            // directory",L"warning",MB_ICONWARNING); 					break;
            //			}
            //
            //			HIRQREQ|=CMOK|EFLS;
            //			SendStatus();
            //			break;
            //		case 0x71: // Read Directory
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockU("-=Read Directory=-");
            //			EmuState::pLog->CdBlockWrite("-=Read Directory=- executed (UNIMPLEMENTED)");
            //			#endif
            //			break;
            //		case 0x72: // Get File System Scope
            //			CR1=cdDriveStatus<<8;
            //			CR2=0x0063;
            //			CR3=0x0100;
            //			CR4=0x0002;
            //
            //			HIRQREQ|=CMOK|EFLS;
            //
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get File System Scope=- executed");
            //			#endif
            //			break;
            //		case 0x73: // Get File Info
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Get File Info=- executed");
            //			#endif
            //
            //			//uint32_t fileId;
            //			fileId=((CR3&0xFF)<<16)|CR4;
            //
            //			if (fileId==0xFFFFFF)
            //			{
            //				MessageBox(NULL,L"GetFileInfo: return all 256 files info",L"unimplemented",MB_ICONWARNING);
            //			}
            //			else
            //			{
            //				BuildFileInfos(fileId);
            //				dataBufferSize=6;
            //			}
            //
            //			bytesTransfered=0;
            //			posInDataBuffer=0;
            //			dataBuffer=filesInfos;
            //
            //			cdDriveStatus|=STAT_TRNS;
            //			CR1=cdDriveStatus<<8;
            //			CR2=6;
            //			CR3=0;
            //			CR4=0;
            //
            //			HIRQREQ|=CMOK|DRDY;
            //			break;
            //		case 0x74: // Read File
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=Read File=- executed");
            //			#endif
            //
            //			uint32_t offset;
            //			offset=((CR1&0xFF)<<16)|CR2;
            //			//uint32_t fileId;
            //			fileId=((CR3&0xFF)<<16)|CR4;
            //			uint8_t filter;
            //			filter=CR3>>8;
            //
            //			// we must set play mode parameters
            //			currentPlayMode=PLAY_MODE_FAD;
            //			FAD=filesOnCD[fileId].LSN;
            //			remainingFADs=filesOnCD[fileId].dataLength/0x800+1;
            //			playFADStart=FAD;
            //
            //			cdDriveStatus=STAT_PLAY;
            //			flag=FLAG_CDROM;
            //
            //			//HIRQREQ|=CMOK|EFLS;
            //			HIRQREQ|=CMOK;
            //			SendStatus();
            //			break;
        case Command::abort_file: abortFile(); break;
        case Command::get_device_authentication_status:
            getDeviceAuthenticationStatus();
            break;
            //		case 0xE0:
            //			HIRQREQ|=CMOK|EFLS;
            //			SendStatus();
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=E0=- executed");
            //			#endif
            //
            //			break;
            //		case 0x93:
            //			CR1=(cdDriveStatus<<8)|0x0001;
            //			CR2=0x0101;
            //			CR3=0x0001;
            //			CR4=0x0001;
            //
            //			HIRQREQ|=CMOK|0x0800;// mpeg enabled?
            //
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("-=0x93=- executed");
            //			#endif
            //
            //			break;
        default:
            //			#ifdef _LOGS
            //			EmuState::pLog->CdBlockWrite("Unknown command : ",CR1>>8);
            //			EmuState::pLog->CdBlockU("Unknown command : ",CR1>>8);
            //			#endif
            Log::unimplemented("Cdblock command {:04x}", cr1_.get(CommandRegister::command));
            break;
    }
    executed_commands_++;

    //	// SH2 command is reset
    //	//EmuState::pSh2m->executerCommandeCdBlock = false;

    if (executed_commands_ > max_number_of_commands_) hirq_status_reg_.reset(HirqStatusRegister::cmok);
}
//
//
// void CCdRom::SetCdDriveStatus(uint8_t value)
//{
//	cdDriveStatus = value;
//}
//
// uint8_t CCdRom::GetCdDriveStatus()
//{
//	return cdDriveStatus;
//}
//
auto Cdrom::isCdInserted() -> bool {
    // verified by trying to read the TOC
    // bool bReturn = Scsi::readToc(TOCData);
    //// AspiTOC=(Aspi_TOC*)&TOCData;
    // AspiTOC = reinterpret_cast<Aspi_TOC*>(&TOCData);
    // return bReturn;
    return false;
}
//
// void CCdRom::InsertCd()
//{
//	// if a CD is present we notify the host and display the CD infos
//	if (!CdInserted()) return;
//
//	string strGameHeader = "";
//	// first sector is read
//	strGameHeader = Scsi::readSector(150,1);
//	// Country code
//	switch(strGameHeader[0x40])
//	{
//		case 'J':
//			EmuState::pMem->countryCode = 0x1;
//			break;
//		case 'T':
//			EmuState::pMem->countryCode = 0x2;
//			break;
//		case 'U':
//			EmuState::pMem->countryCode = 0x4;
//			break;
//		case 'E':
//			EmuState::pMem->countryCode = 0xC;
//			break;
//		default:
//			EmuState::pMem->countryCode = 0x4;
//			break;
//	}
//
//	if(strGameHeader[0x0]== 0x53) {
//		// We only keep the game name
//		strGameHeader.erase(strGameHeader.begin(), strGameHeader.begin() + 0x60);
//		strGameHeader.erase(strGameHeader.begin() + 0x70, strGameHeader.end());
//
//		EmuState::pMem->SetCurrentGameHeader(strGameHeader);
//	}
//	// build file system tree
//	BuildFileSystemTree();
//
//	HIRQREQ|=DCHG;
//}
//
// void CCdRom::BuildSaturnTOC()
//{
//	int32_t i;
//	// tracks
//	for (i=0;i<(AspiTOC->last-AspiTOC->first+1);i++)
//	{
//		// ctrl|adr
//		saturnTOC[4*i]=(AspiTOC->track[i].adr_ctrl<<4)|(AspiTOC->track[i].adr_ctrl>>4);
//		// track start frame address
//		uint32_t fad=(AspiTOC->track[i].addr[1]<<16)|(AspiTOC->track[i].addr[2]<<8)|AspiTOC->track[i].addr[3];
//		fad+=150;
//		saturnTOC[4*i+1]=static_cast<uint8_t>(fad>>16);
//		saturnTOC[4*i+2]=static_cast<uint8_t>(fad>>8);
//		saturnTOC[4*i+3]=static_cast<uint8_t>(fad);
//	}
//
//	// unused tracks
//	for (;i<99;i++) saturnTOC[4*i]=saturnTOC[4*i+1]=saturnTOC[4*i+2]=saturnTOC[4*i+3]=0xFF;
//
//	// first/last track infos
//	// first
//	saturnTOC[4*99]=(AspiTOC->track[AspiTOC->first-1].adr_ctrl<<4)|(AspiTOC->track[AspiTOC->first-1].adr_ctrl>>4);
//	saturnTOC[4*99+1]=AspiTOC->first;
//	saturnTOC[4*99+2]=0;// PSEC ??
//	saturnTOC[4*99+3]=0;// PFRAME ??
//	// last
//	saturnTOC[4*100]=(AspiTOC->track[AspiTOC->last-1].adr_ctrl<<4)|(AspiTOC->track[AspiTOC->last-1].adr_ctrl>>4);
//	saturnTOC[4*100+1]=AspiTOC->last;
//	saturnTOC[4*100+2]=0;// PSEC ??
//	saturnTOC[4*100+3]=0;// PFRAME ??
//
//	// read out infos
//	saturnTOC[4*101]=(AspiTOC->track[AspiTOC->last].adr_ctrl<<4)|(AspiTOC->track[AspiTOC->last].adr_ctrl>>4);
//	uint32_t fad=(AspiTOC->track[AspiTOC->last].addr[1]<<16)|
//						(AspiTOC->track[AspiTOC->last].addr[1]<<8)|
//						AspiTOC->track[AspiTOC->last].addr[3];
//	fad+=150;
//	saturnTOC[4*101+1]=static_cast<uint8_t>(fad>>16);
//	saturnTOC[4*101+2]=static_cast<uint8_t>(fad>>8);
//	saturnTOC[4*101+3]=static_cast<uint8_t>(fad);
//}
//
// void CCdRom::BuildFileSystemTree()
//{
//	string strBuff;
//
//	// first clean directory tree
//	filesOnCD.clear();
//
//	// get some parameters
//	strBuff = Scsi::readSector(150+16,1);
//
//	// file table location
//	uint32_t ulFileTableFAD=0;
//	//ulFileTableFAD=*(uint32_t *)strBuff.substr(158,4).c_str();
//	//Packed32 fad = {strBuff[158],strBuff[158+1],strBuff[158+2],strBuff[158+3]};
//	std::array <int8_t,4> fad = {strBuff[158],strBuff[158+1],strBuff[158+2],strBuff[158+3]};
//	//ulFileTableFAD = fad.val32;
//	ulFileTableFAD = swap_endian<uint32_t>(RawRead32(fad, 0));
//
//	// file table length
//	uint32_t ulFileTableLength=0;
//	//ulFileTableLength=*(uint32_t *)strBuff.substr(166,4).c_str();
//	//Packed32 length = {strBuff[166],strBuff[166+1],strBuff[166+2],strBuff[166+3]};
//	//ulFileTableLength = length.val32;
//	std::array <int8_t,4> length = {strBuff[166],strBuff[166+1],strBuff[166+2],strBuff[166+3]};
//    ulFileTableLength = swap_endian<uint32_t>(RawRead32(length, 0));
//
//
//    // file list
//	int32_t sectors_number = ulFileTableLength/2048;
//	strBuff = Scsi::readSector(150+ulFileTableFAD,sectors_number);
//	int32_t iCurrent = 0;
//	int32_t iPrev = 0;
//	//uint32_t loc;
//	//uint32_t dataLength;
//	bool bDone=false;
//	int32_t num=0;
//
//	while (!bDone)
//	{
//		if(strBuff[iCurrent])
//		{
//			DirFileInfos infos;
//
//			infos.fileNumber=num;
//
//			uint8_t ucLEN_FI = strBuff[iCurrent+32]; // getting the file identifier length
//			infos.name = strBuff.substr(iCurrent+33,ucLEN_FI);
//
//			infos.directoryName=strBuff[iCurrent+32+ucLEN_FI];
//
//			//loc=*(uint32_t *)strBuff.substr(iCurrent+2,4).c_str();
//			//Packed32 loc = { strBuff[iCurrent+2], strBuff[iCurrent+3], strBuff[iCurrent+4], strBuff[iCurrent+5]};
//			//infos.LSN=loc.val32+150;
//			std::array <int8_t,4> loc = { strBuff[iCurrent+2], strBuff[iCurrent+3], strBuff[iCurrent+4], strBuff[iCurrent+5]};
//            infos.LSN = swap_endian<uint32_t>(RawRead32(loc, 0)) + 150;
//
//			//dataLength=*(uint32_t *)strBuff.substr(iCurrent+10,4).c_str();
//			//Packed32 data_length = { strBuff[iCurrent + 10], strBuff[iCurrent + 11], strBuff[iCurrent + 12], strBuff[iCurrent +
// 13]};
//			//infos.dataLength = data_length.val32;
//			std::array <int8_t,4> data_length = { strBuff[iCurrent + 10], strBuff[iCurrent + 11], strBuff[iCurrent + 12],
// strBuff[iCurrent + 13]};
//            infos.dataLength = swap_endian<uint32_t>(RawRead32(data_length, 0));
//
//
//			infos.flags=strBuff[iCurrent+25];
//
//			filesOnCD.push_back(infos);
//
//			num++;
//		}
//		else
//		{
//			iCurrent = iPrev + 2048;
//			iPrev = iCurrent;
//			if (iCurrent>=static_cast<int32_t>(ulFileTableLength)) bDone=true;
//			//if (iCurrent>=ulFileTableLength) break;
//		}
//		iCurrent+=strBuff[iCurrent];
//	}
//	#ifdef _LOGS
//	EmuState::pLog->CdBlockWrite("total number of files: ",filesOnCD.size());
//	#endif
//}
//
//

//
// void CCdRom::CdBlockReadSectors(int32_t num)
//{
//	if (FindFreeSector()==NO_FREE_SECTOR)
//	{
//		// full!
//		#ifdef _LOGS
//		EmuState::pLog->CdBlockWrite("*CD block buffer is full!*");
//		#endif
//
//		cdDriveStatus=STAT_PAUSE|STAT_TRNS;
//		HIRQREQ|=BFUL;
//		HIRQREQ|=EHST;
//	}
//	else
//	{
//		// not full, we read one sector if in play range
//		if (remainingFADs)
//		{
//			// read num sectors
//			int32_t n=0;
//			int32_t freeSector=FindFreeSector();
//
//			string strBuff;
//			while ((n<num)&&(freeSector!=NO_FREE_SECTOR)&&remainingFADs)
//			{
//				// read one sector data
//				strBuff = Scsi::readSector(FAD,1);
//				memcpy_s(sectorsBuffer[freeSector].data,2352,strBuff.c_str(),2048);
//				sectorsBuffer[freeSector].size=2048;
//
//				// we bind the sector with the current buffer partition connected with the current filter
//				int32_t posInBufferPartition=0;
//				for (;posInBufferPartition<MAX_SECTORS;posInBufferPartition++)
//				{
//					if (bufferPartitions[filters[currentFilter].trueOutput].sectors[posInBufferPartition]==NULL) break;
//				}
//				bufferPartitions[filters[currentFilter].trueOutput].sectors[posInBufferPartition]=&sectorsBuffer[freeSector];
//				bufferPartitions[filters[currentFilter].trueOutput].size=posInBufferPartition+1;
//
//				#ifdef _LOGS
//				EmuState::pLog->CdBlockWrite("remaining FADs to play: ",remainingFADs);
//				#endif
//
//				FAD++;
//				remainingFADs--;
//
//				// one sector has been read
//				HIRQREQ|=CSCT;
//
//				freeSector=FindFreeSector();
//				n++;
//			}
//
//			// check if no more sectors have to be read
//			if (remainingFADs==0)
//			{
//				#ifdef _LOGS
//				EmuState::pLog->CdBlockWrite("**CD reading is stopped**");
//				#endif
//				// stops reading
//				//cdDriveStatus=STAT_PAUSE;
//				cdDriveStatus=STAT_TRNS|STAT_PAUSE;
//				HIRQREQ|=PEND|DRDY;
//				HIRQREQ|=EHST|EFLS;
//				HIRQREQ&=~BFUL;
//			}
//			else
//			{
//				cdDriveStatus=STAT_PLAY;
//				HIRQREQ|=DRDY;
//				HIRQREQ&=~EFLS;
//				HIRQREQ&=~EHST;
//			}
//		}
//		else
//		{
//			#ifdef _LOGS
//			EmuState::pLog->CdBlockWrite("**CD reading is stopped**");
//			#endif
//			// stops reading
//			cdDriveStatus=STAT_PAUSE|STAT_TRNS;
//			HIRQREQ|=PEND|DRDY;
//			HIRQREQ|=EHST|EFLS;
//			HIRQREQ&=~BFUL;
//		}
//	}
//}
//
// int32_t CCdRom::FindCurrentTrack()
//{
//	uint32_t realFAD=FAD-150;
//
//	for (int32_t i=0;i<(AspiTOC->last-AspiTOC->first+1);i++)
//	{
//		// track start frame address
//		uint32_t fad1=(AspiTOC->track[i].addr[1]<<16)|(AspiTOC->track[i].addr[2]<<8)|AspiTOC->track[i].addr[3];
//		uint32_t fad2=(AspiTOC->track[i+1].addr[1]<<16)|(AspiTOC->track[i+1].addr[2]<<8)|AspiTOC->track[i+1].addr[3];
//		if ((realFAD>=fad1)&&(realFAD<fad2)) return i+1;
//	}
//	return 0xFF;
//}
//
// int32_t CCdRom::FindFreeSector()
//{
//	for (int32_t i=0;i<MAX_SECTORS;i++)
//	{
//		if (sectorsBuffer[i].size==0) return i;
//	}
//	return NO_FREE_SECTOR;
//}
//
//

void Cdrom::initialize() {
    Log::info(Logger::cdrom, tr("CD-ROM initialization"));
    reset();
}

auto Cdrom::read8(const u32 addr) const -> u8 {
    //	address&=0xFFFFF;
    //
    switch (addr) {
        case fetch_data_pointer_address:
        case fetch_data_pointer_address + 1:
        case fetch_data_pointer_address + 2:
        case fetch_data_pointer_address + 3:
            //			{
            //				uint8_t data=0;
            //				if (posInSector>=2048)
            //				{
            //					posInSector=0;
            //					sectorNumInBuffer++;
            //				}
            //				if (bufferPartitions[fetchedBuffer].sectors[sectorNumInBuffer])
            //				{
            //					data|=bufferPartitions[fetchedBuffer].sectors[sectorNumInBuffer]->data[posInSector];
            //					bytesTransfered++;
            //					posInSector++;
            //					lastFetchedBuffer=fetchedBuffer;
            //				}
            //				return data;
            //			}
            break;

        default: Log::warning(Logger::cdrom, "Unmapped access {:#010x}", addr); return 0;
    }

    return 0;
} // namespace saturnin::cdrom

auto Cdrom::read16(const u32 addr) -> u16 {
    switch (addr) {
        case hirq_register_address:
            Log::debug(Logger::cdrom, "HIrqReg={:#06x}", hirq_status_reg_.toU32());
            return hirq_status_reg_.toU32();
        case hirq_mask_register_address: return hirq_mask_reg_.toU32();
        case command_register_1_address: return cr1_.toU32();
        case command_register_2_address: return cr2_.toU32();
        case command_register_3_address: return cr3_.toU32();
        case command_register_4_address:
            // firstReading = false;
            is_initialization_done_ = true;
            return cr4_.toU32();
        case toc_data_pointer_address:
            //			{
            //				uint16_t data=0;
            //				// reading in data buffer
            //				if ((uint32_t)posInDataBuffer<dataBufferSize)
            //				{
            //					data=dataBuffer[posInDataBuffer*2]<<8|dataBuffer[posInDataBuffer*2+1];
            //				}
            //				posInDataBuffer++;
            //				bytesTransfered+=2;
            //				return data;
            //			}
            //			break;
        default: Log::warning(Logger::cdrom, "Unmapped access {:#010x}", addr); return 0;
    }
    return 0;
}

auto Cdrom::read32(const u32 addr) const -> u32 {
    switch (addr) {
        case fetch_data_pointer_address:
            //			{
            //				uint32_t data=0;
            //				if (posInSector>=2048)
            //				{
            //					posInSector=0;
            //					sectorNumInBuffer++;
            //				}
            //				if (bufferPartitions[fetchedBuffer].sectors[sectorNumInBuffer])
            //				{
            //					data=bufferPartitions[fetchedBuffer].sectors[sectorNumInBuffer]->data[posInSector];
            //					data<<=8;
            //					data|=bufferPartitions[fetchedBuffer].sectors[sectorNumInBuffer]->data[posInSector+1];
            //					data<<=8;
            //					data|=bufferPartitions[fetchedBuffer].sectors[sectorNumInBuffer]->data[posInSector+2];
            //					data<<=8;
            //					data|=bufferPartitions[fetchedBuffer].sectors[sectorNumInBuffer]->data[posInSector+3];
            //					bytesTransfered+=4;
            //					posInSector+=4;
            //					lastFetchedBuffer=fetchedBuffer;
            //				}
            //				return data;
            //			}
            break;

        default: Log::warning(Logger::cdrom, "Unmapped access {:#010x}", addr); return 0;
    }
    return 0;
}

void Cdrom::write8(const u32 addr, const u8 data) {
    Log::warning(Logger::cdrom, "Unmapped write access {:#010x} {:#04x}", addr, data);
}

void Cdrom::write16(const u32 addr, const u16 data) {
    switch (addr) {
        case hirq_register_address: hirq_status_reg_ &= data; break;
        case hirq_mask_register_address: hirq_mask_reg_.set(HirqMaskRegister::all_bits, data); break;
        case command_register_1_address:
            cr1_.set(CommandRegister::all_bits, data);
            is_command_being_initialized_ = true;
            break;
        case command_register_2_address:
            cr2_.set(CommandRegister::all_bits, data);
            is_command_being_initialized_ = true;
            break;
        case command_register_3_address:
            cr3_.set(CommandRegister::all_bits, data);
            is_command_being_initialized_ = true;
            break;
        case command_register_4_address:
            cr4_.set(CommandRegister::all_bits, data);
            is_command_being_initialized_ = false;
            executeCommand();
            break;
        default: Log::warning(Logger::cdrom, "Unmapped write access {:#010x} {:#04x}", addr, data); break;
    }
}

void Cdrom::write32(const u32 addr, const u32 data) {
    switch (addr) {
        case fetch_data_pointer_address:
            //			{
            //				int32_t freeSector=0;
            //				if (posInSectorPut>=2048)
            //				{
            //					// current sector in buffer partition is full, add a new sector
            //					posInSectorPut=0;
            //					sectorNumInBufferPut++;
            //
            //					if ((freeSector==NO_FREE_SECTOR))//&&(sectorNumInBufferPut<numOfSectorToPut))
            //					{
            //						// full!
            //
            //						status=STAT_PAUSE|STAT_TRNS;
            //						HIRQREQ|=BFUL|DRDY;
            //						HIRQREQ&=~EFLS;
            //						//HIRQREQ&=~EHST;
            //						HIRQREQ|=EHST;
            //					}
            //					else
            //					{
            //						// we bind the sector with the current buffer partition connected with the current filter
            //						int32_t posInBufferPartition=0;
            //						for (;posInBufferPartition<MAX_SECTORS;posInBufferPartition++)
            //						{
            //							if (bufferPartitions[fetchedBuffer].sectors[posInBufferPartition]==NULL) break;
            //						}
            //						bufferPartitions[fetchedBuffer].sectors[posInBufferPartition]=&sectorsBuffer[freeSector];
            //						bufferPartitions[fetchedBuffer].size=posInBufferPartition+1;
            //					}
            //				}
            //				if (freeSector!=NO_FREE_SECTOR)
            //				{
            //					bufferPartitions[fetchedBuffer].sectors[sectorNumInBufferPut]->data[posInSectorPut]=static_cast<uint8_t>(data>>24);
            //					bufferPartitions[fetchedBuffer].sectors[sectorNumInBufferPut]->data[posInSectorPut+1]=static_cast<uint8_t>(data>>16);
            //					bufferPartitions[fetchedBuffer].sectors[sectorNumInBufferPut]->data[posInSectorPut+2]=static_cast<uint8_t>(data>>8);
            //					bufferPartitions[fetchedBuffer].sectors[sectorNumInBufferPut]->data[posInSectorPut+3]=static_cast<uint8_t>(data);
            //
            //					bytesTransfered+=4;
            //					posInSectorPut+=4;
            //					lastFetchedBuffer=fetchedBuffer;
            //
            //					HIRQREQ|=CSCT;
            //				}
            //				else
            //				{
            //					//log("put long data over the end of the buffer partition, sectorNumInBufferPut: %d,
            // numOfSectorToPut: %d\n",
            //					//	sectorNumInBufferPut,numOfSectorToPut);
            //				}
            //			}
            break;
        default: Log::warning(Logger::cdrom, "Unmapped write access {:#010x} {:#04x}", addr, data); break;
    }
}

void Cdrom::run(const u8 cycles) {
    // Periodic response musn't be issued before the initialisation string is read from CR registers
    if (!is_initialization_done_) { return; }

    elapsed_cycles_ -= cycles;
    if (elapsed_cycles_ <= 0) {
        Log::debug(Logger::cdrom, "Sending periodic response");
        executed_commands_ = 0;

        hirq_status_reg_.set(HirqStatusRegister::cmok, Cmok::ready);

        elapsed_cycles_ += periodic_response_duration_;

        //		// Sector reading
        //		if((cdDriveStatus & STAT_PLAY) == STAT_PLAY){
        //			if(currentPlayMode == PLAY_MODE_FAD){
        //				// Sector reading
        //				#ifdef _LOGS
        //				EmuState::pLog->CdBlockWrite("#read one sector");
        //				#endif
        //
        //				CdBlockReadSectors(1);
        //
        //			}else if(currentPlayMode == PLAY_MODE_TRACK){
        //				//EmuState::pLog->CdBlockWrite("PLAY MODE TRACK (NOT IMPLEMENTED)");
        //			}
        //		}else if((cdDriveStatus & STAT_PAUSE) == STAT_PAUSE){
        //			if(FindFreeSector()==NO_FREE_SECTOR){
        //				// Buffer is full
        //				HIRQREQ|=BFUL;
        //				//HIRQREQ|=BFUL|DRDY;
        //				//HIRQREQ&=~EFLS;
        //				HIRQREQ|=EHST;
        //				cdDriveStatus|=STAT_TRNS;
        //			}else{
        //				// has play to be resumed ?
        //				if(remainingFADs){
        //					cdDriveStatus = STAT_PLAY|STAT_TRNS;
        //					HIRQREQ|=DRDY;
        //					HIRQREQ&=~EFLS;
        //					HIRQREQ&=~EHST;
        //				}
        //				HIRQREQ&=~BFUL;
        //			}
        //		}

        // Periodic response is not sent while a command is being initialized
        if (is_command_being_initialized_) { return; }

        sendStatus();
        cr1_ |= (utilities::toUnderlying(CdDriveStatus::periodical_response) << displacement_8);

        // periodic response timing is the same as SCDQ update timing
        hirq_status_reg_.set(HirqStatusRegister::scdq, Scdq::subcode_q_decoded);

        //		if (FindFreeSector()==NO_FREE_SECTOR){
        //
        //				// buffer is full
        //				//HIRQREQ|=BFUL|DRDY;
        //				HIRQREQ|=BFUL;
        //				//HIRQREQ&=~EFLS;
        //				HIRQREQ|=EHST;
        //				cdDriveStatus|=STAT_TRNS;
        //				CR1|=STAT_TRNS<<8;
        //		}
    }
}

auto Cdrom::getRegisters() -> std::vector<std::string> {
    std::vector<std::string> registers;
    registers.emplace_back(fmt::format("HIrq Status Register : {:#06x}", hirq_status_reg_.toU16()));
    registers.emplace_back(fmt::format("HIrq Mask Register : {:#06x}", hirq_mask_reg_.toU16()));
    registers.emplace_back(fmt::format("Command Register 1 : {:#06x}", cr1_.toU16()));
    registers.emplace_back(fmt::format("Command Register 2 : {:#06x}", cr2_.toU16()));
    registers.emplace_back(fmt::format("Command Register 3 : {:#06x}", cr3_.toU16()));
    registers.emplace_back(fmt::format("Command Register 4 : {:#06x}", cr4_.toU16()));

    return registers;
}

//--------------------------------------------------------------------------------------------------------------
// PRIVATE section
//--------------------------------------------------------------------------------------------------------------

//
// void CCdRom::RunCdBlock(int32_t cycles)
//{
//	// we must not issue periodic response before initialisation string is read in CR registers
//	if (firstReading) return;
//
//
//	// Periodic response
//	counter-=cycles;
//	if (counter<=0)
//	{
//		#ifdef _LOGS
//		//EmuState::pLog->CdBlockWrite("#periodic response");
//        EmuState::pLog->CdBlockWrite(str(format("%s HIRQREQ=0x%x") % "#periodic response" % HIRQREQ));
//        //tmp += str(format("%1%") % screen);
//		#endif
//		executedCommands = 0;
//		HIRQREQ|=CMOK;
//
//		counter+=responsePeriod;
//
//
//		// Sector reading
//		if((cdDriveStatus & STAT_PLAY) == STAT_PLAY){
//			if(currentPlayMode == PLAY_MODE_FAD){
//				// Sector reading
//				#ifdef _LOGS
//				EmuState::pLog->CdBlockWrite("#read one sector");
//				#endif
//
//				CdBlockReadSectors(1);
//
//			}else if(currentPlayMode == PLAY_MODE_TRACK){
//				//EmuState::pLog->CdBlockWrite("PLAY MODE TRACK (NOT IMPLEMENTED)");
//			}
//		}else if((cdDriveStatus & STAT_PAUSE) == STAT_PAUSE){
//			if(FindFreeSector()==NO_FREE_SECTOR){
//				// Buffer is full
//				HIRQREQ|=BFUL;
//				//HIRQREQ|=BFUL|DRDY;
//				//HIRQREQ&=~EFLS;
//				HIRQREQ|=EHST;
//				cdDriveStatus|=STAT_TRNS;
//			}else{
//				// has play to be resumed ?
//				if(remainingFADs){
//					cdDriveStatus = STAT_PLAY|STAT_TRNS;
//					HIRQREQ|=DRDY;
//					HIRQREQ&=~EFLS;
//					HIRQREQ&=~EHST;
//				}
//				HIRQREQ&=~BFUL;
//			}
//		}
//
//
//		// we must not issue periodic response while a command is written
//		if (writingCRRegs) return;
//
//		// periodic response
//		SendStatus();
//		CR1|=STAT_PERI<<8;
//
//		// periodic response timing is the same as SCDQ update timing
//		HIRQREQ|=SCDQ;
//		if (FindFreeSector()==NO_FREE_SECTOR){
//
//				// buffer is full
//				//HIRQREQ|=BFUL|DRDY;
//				HIRQREQ|=BFUL;
//				//HIRQREQ&=~EFLS;
//				HIRQREQ|=EHST;
//				cdDriveStatus|=STAT_TRNS;
//				CR1|=STAT_TRNS<<8;
//		}
//	}
//}
//
// void CCdRom::UpdatePeriod()
//{
//	cyclesPerMs = EmuState::pSmpc->GetPllClock() / 1000;
//	switch (driveSpeed)
//	{
//		case STD_PLAY_SPEED:
//			responsePeriod=static_cast<int32_t>(STD_PLAY_SPEED_PERIOD*cyclesPerMs);
//			executedCommandsMax = 34;
//			break;
//		case DBL_PLAY_SPEED:
//			responsePeriod=static_cast<int32_t>(DBL_PLAY_SPEED_PERIOD*cyclesPerMs);
//			executedCommandsMax = 30;
//			break;
//		case STANDBY:
//			responsePeriod=static_cast<int32_t>(STANDBY_PERIOD*cyclesPerMs);
//			executedCommandsMax = 60;
//			break;
//	}
//}
//
// int32_t CCdRom::FreeNumOfSectorsInBuffer()
//{
//	int32_t num=0;
//	for (int32_t i=0;i<MAX_SECTORS;i++)
//	{
//		if (sectorsBuffer[i].size) num++;
//	}
//	num=0xC8-num;
//	if (num<0) return 0;
//	else return num;
//}
//
// void CCdRom::BuildFileInfos(uint32_t fileId)
//{
//	filesInfos[0]=static_cast<uint8_t>(filesOnCD[fileId].LSN>>24);
//	filesInfos[1]=static_cast<uint8_t>(filesOnCD[fileId].LSN>>16);
//	filesInfos[2]=static_cast<uint8_t>(filesOnCD[fileId].LSN>>8);
//	filesInfos[3]=static_cast<uint8_t>(filesOnCD[fileId].LSN);
//	filesInfos[4]=static_cast<uint8_t>(filesOnCD[fileId].dataLength>>24);
//	filesInfos[5]=static_cast<uint8_t>(filesOnCD[fileId].dataLength>>16);
//	filesInfos[6]=static_cast<uint8_t>(filesOnCD[fileId].dataLength>>8);
//	filesInfos[7]=static_cast<uint8_t>(filesOnCD[fileId].dataLength);
//	filesInfos[8]=0;
//	filesInfos[9]=0;
//	filesInfos[10]=static_cast<uint8_t>(fileId);
//	filesInfos[11]=0;
//}
//
// void CCdRom::RefreshPeriod(){
//	UpdatePeriod();
//}

void Cdrom::reset() {
    // CD-BLOCK registers initialization
    hirq_status_reg_.reset();
    hirq_status_reg_.set(HirqStatusRegister::scdq, Scdq::subcode_q_decoded);
    hirq_status_reg_.set(HirqStatusRegister::efls, Efls::file_system_finished);
    hirq_status_reg_.set(HirqStatusRegister::ecpy, Ecpy::sector_copy_or_move_finished);
    hirq_status_reg_.set(HirqStatusRegister::ehst, Ehst::host_io_finished);
    hirq_status_reg_.set(HirqStatusRegister::esel, Esel::soft_reset_or_selector_set_finished);
    hirq_status_reg_.set(HirqStatusRegister::dchg, Dchg::disk_has_not_changed);
    hirq_status_reg_.set(HirqStatusRegister::pend, Pend::cd_play_has_ended);
    hirq_status_reg_.set(HirqStatusRegister::bful, Bful::buffer_not_full);
    hirq_status_reg_.set(HirqStatusRegister::csct, Csct::sector_not_stored);
    hirq_status_reg_.set(HirqStatusRegister::drdy, Drdy::setup_complete);
    hirq_status_reg_.set(HirqStatusRegister::cmok, Cmok::ready);

    hirq_mask_reg_.set();

    constexpr auto cr1_default = u8{0x43}; // 'C'
    cr1_.set(CommandRegister::lower_8_bits, cr1_default);
    constexpr auto cr2_default = u16{0x4442}; // 'DB'
    cr2_.set(CommandRegister::all_bits, cr2_default);
    constexpr auto cr3_default = u16{0x4C4F}; // 'LO'
    cr3_.set(CommandRegister::all_bits, cr3_default);
    constexpr auto cr4_default = u16{0x434B}; // 'CK'
    cr4_.set(CommandRegister::all_bits, cr4_default);

    cd_drive_status_    = CdDriveStatus::no_disc_inserted;
    cd_drive_play_mode_ = CdDrivePlayMode::standby;

    periodic_response_duration_ = calculatePeriodicResponseDuration();
    elapsed_cycles_             = periodic_response_duration_;
}

auto Cdrom::calculatePeriodicResponseDuration() -> u32 {
    enum class NumberOfCommands { standard_play_speed = 34, double_play_speed = 30, standby = 60 };

    switch (cd_drive_play_mode_) {
        case CdDrivePlayMode::standard_play_speed:
            max_number_of_commands_ = utilities::toUnderlying(NumberOfCommands::standard_play_speed);
            return modules_.smpc()->calculateCyclesNumber(periodic_response_period_standard);

        case CdDrivePlayMode::double_play_speed:
            max_number_of_commands_ = utilities::toUnderlying(NumberOfCommands::double_play_speed);
            return modules_.smpc()->calculateCyclesNumber(periodic_response_period_double);

        case CdDrivePlayMode::standby:
            max_number_of_commands_ = utilities::toUnderlying(NumberOfCommands::standby);
            return modules_.smpc()->calculateCyclesNumber(periodic_response_period_standby);
        default: Log::warning(Logger::cdrom, "Unknown play mode"); return 0;
    }
}

//--------------------------------------------------------------------------------------------------------------
// CDBLOCK COMMANDS methods
//--------------------------------------------------------------------------------------------------------------
void Cdrom::sendStatus() {
    // CR1 = cd_drive_status_ << 8; // CR1-H
    cr1_.set(CommandRegister::status, cd_drive_status_);
    switch (cd_drive_status_) {
        case CdDriveStatus::drive_is_open:
        case CdDriveStatus::no_disc_inserted:
        case CdDriveStatus::error:
            // CR1 |= 0xFF;
            cr1_.set(CommandRegister::lower_8_bits, u8{0xff});
            cr2_.set(CommandRegister::all_bits, u16{0xffff});
            cr3_.set(CommandRegister::all_bits, u16{0xffff});
            cr4_.set(CommandRegister::all_bits, u16{0xffff});
            break;
        default:
            // CR1 |= ((flag & 0xF) << 4) | (repCnt & 0xF);
            // CR2 = (ctrlAdr << 8) | tno;
            // CR3 = (uint16_t)((ino << 8) | (FAD >> 16));
            // CR4 = (uint16_t)FAD;
            break;
    }
}

void Cdrom::getStatus() {
    // Status(8) | Flags(4) | Rep Cnt(4)
    // Ctrl Addr(8) | Track No(8)
    // Index No(8) | Upper Byte of current FAD (8)
    // Lower word of current FAD

    sendStatus();

    hirq_status_reg_.set(HirqStatusRegister::cmok, Cmok::ready);

    Log::debug(Logger::cdrom, "Get Status executed");
}

void Cdrom::getHardwareInfo() {
    // Status(8) | 0x00
    // Hardware Flag(8) | Version(8)
    // 0x00 | Mpeg Version (8)
    // Driver Version(8) | Revision(8)

    cr1_.set(CommandRegister::status, cd_drive_status_);
    cr1_.set(CommandRegister::lower_8_bits, u8{0});
    cr2_.set(CommandRegister::all_bits, u16{0x0001});
    cr3_.set(CommandRegister::all_bits, u16{0x0000});
    cr4_.set(CommandRegister::all_bits, u16{0x0102});

    hirq_status_reg_.set(HirqStatusRegister::cmok, Cmok::ready);

    Log::debug(Logger::cdrom, "Get Hardware Info executed");
}

void Cdrom::endDataTransfer() {
    // Status(8) | Upper byte of word written(8)
    // Lower word of words written(16)
    // 0x0000
    // 0x0000

    //
    //			cdDriveStatus &=~STAT_TRNS;
    //
    //			CR1=cdDriveStatus<<8;
    //			CR1|=((bytesTransfered/2)>>16)&0xFF;
    //			CR2=static_cast<uint16_t>(bytesTransfered/2);
    //			CR3=0;
    //			CR4=0;
    //
    //			#ifdef _LOGS
    //			EmuState::pLog->CdBlockWrite("Bytes transfered : 0x",bytesTransfered);
    //			#endif
    //
    //			// data fetching is finished
    //			// check if sectors must be erased
    //			if (DeleteSectorDataExecuted)
    //			{
    //				#ifdef _LOGS
    //				EmuState::pLog->CdBlockWrite("*Deleting sectors in buffer partition n°0x",fetchedBuffer);
    //				EmuState::pLog->CdBlockWrite("Old buffer size: 0x",bufferPartitions[fetchedBuffer].size);
    //				EmuState::pLog->CdBlockWrite("Position of sector to delete in buffer: 0x",posOfSectorsToDelete);
    //				EmuState::pLog->CdBlockWrite("Number of sectors to delete: 0x",numberOfSectorsToDelete);
    //				#endif
    //				int32_t oldSize=bufferPartitions[fetchedBuffer].size;
    //
    //				for (int32_t i=posOfSectorsToDelete;i<(posOfSectorsToDelete+numberOfSectorsToDelete);i++)
    //				{
    //					if (i>MAX_SECTORS)
    //					{
    //						#ifdef _LOGS
    //						EmuState::pLog->CdBlockWrite("End of buffer partition reached");
    //						#endif
    //						break;
    //					}
    //					if (bufferPartitions[fetchedBuffer].sectors[i])
    //					{
    //						#ifdef _LOGS
    //						EmuState::pLog->CdBlockWrite("Deleting in buffer partition sector n° ",i);
    //						#endif
    //						bufferPartitions[fetchedBuffer].sectors[i]->size=0;
    //						bufferPartitions[fetchedBuffer].sectors[i]=NULL;
    //						bufferPartitions[fetchedBuffer].size--;
    //					}
    //					else break;
    //				}
    //
    //				// we shift the sectors in buffer partition if necessary
    //				if (numberOfSectorsToDelete<oldSize)
    //				{
    //					// we did not erase all the sectors
    //					// we check if there a hole in buffer partition
    //					#ifdef _LOGS
    //					EmuState::pLog->CdBlockWrite("All sectors haven't been deleted, checking for hole");
    //					#endif
    //					int32_t holeStart=0;
    //					for (;holeStart<MAX_SECTORS;holeStart++)
    //					{
    //						if (!bufferPartitions[fetchedBuffer].sectors[holeStart]) break;
    //					}
    //					// holeStart contains the position of the hole
    //					// let's check if the hole is positioned before the end of used sectors
    //					if (holeStart<bufferPartitions[fetchedBuffer].size)
    //					{
    //						EmuState::pLog->CdBlockWrite("There is a hole at sector n°",holeStart);
    //						// let's get the position of the end of the hole
    //						int32_t holeEnd=holeStart;
    //						for (;holeEnd<MAX_SECTORS;holeEnd++)
    //						{
    //							if (bufferPartitions[fetchedBuffer].sectors[holeEnd]) break;
    //						}
    //						// we shift the sectors
    //						int32_t i=0;
    //						//for (i=0;(i+holeEnd)<MAX_SECTORS;i++)
    //						for (i=0;(i+holeEnd)<(holeEnd+bufferPartitions[fetchedBuffer].size);i++)
    //						{
    //							if ((i+holeEnd)>MAX_SECTORS) break;
    //							bufferPartitions[fetchedBuffer].sectors[holeStart+i]=bufferPartitions[fetchedBuffer].sectors[holeEnd+i];
    //							#ifdef _LOGS
    //							EmuState::pLog->CdBlockWrite("Sector n°",holeEnd+i);
    //							EmuState::pLog->CdBlockWrite("shifted to sector n°",holeStart+i);
    //							#endif
    //						}
    //						for (;i<MAX_SECTORS;i++)
    //						{
    //							bufferPartitions[fetchedBuffer].sectors[i]=NULL;
    //						}
    //					}
    //				}
    //
    //				DeleteSectorDataExecuted=false;
    //				// update buffer partition size
    //				/*bufferPartitions[fetchedBuffer].size=0;
    //				for (int32_t i=0;i<MAX_SECTORS;i++)
    //				{
    //					if (bufferPartitions[fetchedBuffer].sectors[i]) bufferPartitions[fetchedBuffer].size++;
    //					else break;
    //				}*/
    //
    //				#ifdef _LOGS
    //				EmuState::pLog->CdBlockWrite("New buffer size in sectors: 0x",bufferPartitions[fetchedBuffer].size);
    //				#endif
    //				// buffer should not be full
    //				HIRQREQ&=~BFUL;
    //			}
    //
    //			bytesTransfered=0;
    //
    //			// flags setup
    //			HIRQREQ|=CMOK | EHST;
    //			//HIRQREQ|=DRDY;

    cr1_.set(CommandRegister::status, cd_drive_status_);
    cr1_.set(CommandRegister::lower_8_bits, u8{0});
    cr2_.set(CommandRegister::all_bits, u16{0x0});
    cr3_.set(CommandRegister::all_bits, u16{0x0});
    cr4_.set(CommandRegister::all_bits, u16{0x0});

    hirq_status_reg_.set(HirqStatusRegister::cmok, Cmok::ready);
    hirq_status_reg_.set(HirqStatusRegister::drdy, Drdy::setup_complete);

    Log::debug(Logger::cdrom, "End Data Transfer executed");
}

void Cdrom::abortFile() {
    // Status(8) | Flags(4) | Rep Cnt(4)
    // Ctrl Addr(8) | Track No(8)
    // Index No(8) | Upper Byte of curre,t FAD (8)
    // Lower word of current FAD

    cd_drive_status_ = isCdInserted() ? CdDriveStatus::paused : CdDriveStatus::no_disc_inserted;

    hirq_status_reg_.set(HirqStatusRegister::cmok, Cmok::ready);
    hirq_status_reg_.set(HirqStatusRegister::efls, Efls::file_system_finished);

    sendStatus();

    Log::debug(Logger::cdrom, "Abort File executed");
}

void Cdrom::getCopyError() {
    // Status(8) | Copy error value (8)
    // 0x0000
    // 0x0000
    // 0x0000

    cr1_.set(CommandRegister::status, cd_drive_status_);
    cr1_.set(CommandRegister::lower_8_bits, u8{0});
    cr2_.set(CommandRegister::all_bits, u16{0x0});
    cr3_.set(CommandRegister::all_bits, u16{0x0});
    cr4_.set(CommandRegister::all_bits, u16{0x0});

    hirq_status_reg_.set(HirqStatusRegister::cmok, Cmok::ready);
    hirq_status_reg_.set(HirqStatusRegister::ecpy, Ecpy::sector_copy_or_move_finished);

    Log::debug(Logger::cdrom, "Get Copy Error executed");
}

void Cdrom::getDeviceAuthenticationStatus() {
    cr1_.set(CommandRegister::status, cd_drive_status_);
    cr1_.set(CommandRegister::lower_8_bits, u8{0});
    cr2_.set(CommandRegister::all_bits, u16{0x0004});
    cr3_.set(CommandRegister::all_bits, u16{0x0000});
    cr4_.set(CommandRegister::all_bits, u16{0x0000});

    hirq_status_reg_.set(HirqStatusRegister::cmok, Cmok::ready);

    Log::debug(Logger::cdrom, "Get Device Authentication Status executed");
}
} // namespace saturnin::cdrom

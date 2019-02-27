// 
// scu.cpp
// Saturnin
//
// Copyright (c) 2018-2019 Renaud Toumazet
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

#include "scu.h"
#include "scu_registers.h"
#include "emulator_context.h"
#include "memory.h"
#include "interrupt_sources.h"

namespace is = saturnin::core::interrupt_source;

namespace saturnin {
namespace core {

// SCU DMA accesses
// Write to A-Bus prohibited
// Read from VDP2 area prohibited (B-Bus)
// Write to VP1 registers (B-Bus) must use 2 bytes
// Access to Workram L not possible
// During DMA operation A -> B or B -> A, no CPU access to A-Bus


Scu::Scu(Emulator_context* ec) : emulator_context_(ec) {
    initializeRegisters();
};

auto Scu::scuMemory() const {
    return emulator_context_->memory()->scu_;
}

u32 Scu::read32(const u32 addr) const {
    switch (addr) {
        case dsp_program_control_port:
            Log::debug("scu", "DSP - Program Control Port read");
            //data = current_DSP_program_control_port_;
            break;
        case dsp_program_ram_dataport:
            Log::debug("scu", "DSP - Program Dataport read");
            //data = current_DSP_program_dataport_;
            break;
        case dsp_data_ram_address_port:
            Log::debug("scu", "DSP - Data RAM Address Port read");
            //data = current_DSP_data_ram_address_port_;
            break;
        case dsp_data_ram_data_port:
            Log::debug("scu", "DSP - Data RAM Dataport read");
            //data = current_DSP_data_ram_dataport_;
            break;
        default:
            return rawRead<u32>(scuMemory(), addr & scu_memory_mask);
    }

    return rawRead<u32>(scuMemory(), addr & scu_memory_mask);

};

void Scu::write32(const u32 addr, const u32 data) {
    //    uint8_t index = 0;
    //    uint32_t temp = 0;
    //
    switch (addr) {
        //        case dsp_program_control_port:
        //            current_DSP_program_control_port_ = data;
        //            if (current_DSP_program_control_port_&DSP_T0) {
        //                // D0 Bus DMA execution
        //
        //            }
        //            if (current_DSP_program_control_port_&DSP_EX) {
        //#ifdef _DEBUGEMU
        //                std::ostringstream oss;
        //                oss << "DSP program execution started at address : " << showbase << hex << static_cast<uint32_t>((current_DSP_program_control_port_ & DSP_P));
        //                EmuState::pLog->ScuDsp(oss.str());
        //#endif
        //                PC_ = static_cast<uint8_t>((current_DSP_program_control_port_ & DSP_P));
        //                DSPProgramDisasm(PC_);
        //                current_DSP_program_control_port_ &= ~DSP_EX; // DSP interception
        //                SetISTFlag(0x45); // DSP interrupt flag
        //            }
        //            break;
        //        case dsp_program_ram_dataport:
        //            index = static_cast<uint8_t>((current_DSP_program_control_port_ & DSP_P));
        //
        //            RawWrite32(pProgramRam_, index, data);
        //            index++;
        //            current_DSP_program_control_port_ &= 0xFFFFFF00;
        //            current_DSP_program_control_port_ |= static_cast<uint8_t>(index);
        //
        //            current_DSP_program_dataport_ = data;
        //            break;
        //        case dsp_data_ram_address_port:
        //            current_DSP_data_ram_address_port_ = static_cast<uint8_t>(data);
        //            break;
        //        case dsp_data_ram_data_port:
        //            current_DSP_data_ram_dataport_ = data;
        //            break;
        case level_0_dma_enable_register: {
            if (DmaEnableRegister(data).get(DmaEnableRegister::dmaEnable) == DmaEnable::enabled) {
                auto dma_0_config = configureDmaTransfer(DmaLevel::level_0);
                executeDma(dma_0_config);
            }
            break;
        }
        case level_1_dma_enable_register: {
            if (DmaEnableRegister(data).get(DmaEnableRegister::dmaEnable) == DmaEnable::enabled) {
                auto dma_1_config = configureDmaTransfer(DmaLevel::level_1);
                executeDma(dma_1_config);
            }
            break;
        }
        case level_2_dma_enable_register: {
            if (DmaEnableRegister(data).get(DmaEnableRegister::dmaEnable) == DmaEnable::enabled) {
                auto dma_2_config = configureDmaTransfer(DmaLevel::level_2);
                executeDma(dma_2_config);
            }
            break;
        }
                                          //        case dma_status_register + 0x0C:
                                          //            // DMA registers write
                                          //            dmaUpdate_ = true;
                                          //            break;
        default:

            break;
    }

    //if(address==0x25FE00A0) DebugBreak();
    rawWrite<u32>(scuMemory(), addr & scu_memory_mask, data);
};

void Scu::executeDma(const DmaConfiguration& dc) {
    switch (dc.dma_mode) {
        case DmaMode::direct:
            //DmaStarting::started
            break;
        case DmaMode::indirect:

            break;
        default:
            Log::warning("scu", "Unknown DMA mode !");
    }

    
//    switch (GetBitValue(static_cast<uint32_t>(d0md), 24)) { // DMA Mode 
//        case 0x0:
//            // Direct mode
//            d0r = EmuState::pMem->ReadLong(0x25FE0000); // Address only used in Direct Mode
//
//            // Start factor
//            switch (d0md & 0x7) {
//                case VBLANKIN:
//                    if (currentStartFactor == VBLANKIN) startDma0 = true;
//                    break;
//                case VBLANKOUT:
//                    if (currentStartFactor == VBLANKOUT) startDma0 = true;
//                    break;
//                case HBLANKIN:
//                    if (currentStartFactor == HBLANKIN) startDma0 = true;
//                    break;
//                case TIMER0:
//                    if (currentStartFactor == TIMER0) startDma0 = true;
//                    break;
//                case TIMER1:
//                    if (currentStartFactor == TIMER1) startDma0 = true;
//                    break;
//                case SOUNDREQ:
//                    if (currentStartFactor == SOUNDREQ) startDma0 = true;
//                    break;
//                case SPRITEDRAWEND:
//                    if (currentStartFactor == SPRITEDRAWEND) startDma0 = true;
//                    break;
//                case DMA:
//                    // checking start factor bit
//                    if (d0en & 0x1) startDma0 = true;
//                    break;
//            }
//            // transfer start
//            if (startDma0) {
//                // FLAG UPDATE NOT NEEDED AS THERE ARE NO EXTERNAL ACCESS DURING TRANSFER
//                //dsta |= 0x4; // DMA 0 in operation
//
//                int16_t dest_add;
//                int16_t source_add;
//                uint32_t byteCounter, wordCounter, longCounter;
//                int32_t readByteOffset, writeByteOffset;
//                uint32_t data;
//                byteCounter = wordCounter = longCounter = 0;
//                readByteOffset = writeByteOffset = 0;
//
//                if (d0c == 0) d0c = 0x100000;
//#ifdef _LOGS
//                EmuState::pLog->ScuWrite("DIRECT MODE DMA - Level 0");
//                EmuState::pLog->Scu("Source : ", d0r);
//                EmuState::pLog->Scu("Destination : ", d0w);
//                EmuState::pLog->Scu("Transfer size : ", d0c);
//                EmuState::pLog->Scu("Address add : ", d0ad);
//#endif
//
//                // Source Address Add Value
//                if (d0ad & 0x100) source_add = 4;
//                else source_add = 0;
//
//                // Destination Address Add Value
//                switch (d0ad & 0x7)
//                {
//                    case 0: dest_add = 0;   break;
//                    case 1: dest_add = 2;   break;
//                    case 2: dest_add = 4;   break;
//                    case 3: dest_add = 8;  break;
//                    case 4: dest_add = 16;  break;
//                    case 5: dest_add = 32;  break;
//                    case 6: dest_add = 64; break;
//                    case 7: dest_add = 128; break;
//                }
//
//                if (((d0w & 0x0FFFFFFF) >= 0x05A00000) &&
//                    ((d0w & 0x0FFFFFFF) < 0x6000000))
//                {
//                    // B-Bus write
//                    // 32 bits splitted into 2*16 bits
//#ifdef _LOGS
//                    EmuState::pLog->Scu("B-Bus transfer", 0);
//#endif
//
//                    if (((d0r & 0x0FFFFFFF) < 0x05800000) ||
//                        ((d0r & 0x0FFFFFFF) >= 0x05900000))
//                    {
//                        source_add = 4;
//                    }
//                    while (byteCounter < static_cast<uint32_t>(d0c))
//                    {
//                        data = EmuState::pMem->ReadByte((d0r & 0x7FFFFFFF) + readByteOffset + longCounter * source_add);
//                        EmuState::pMem->WriteByte(d0w + writeByteOffset + wordCounter * dest_add, static_cast<uint8_t>(data));
//
//                        readByteOffset++;
//                        writeByteOffset++;
//
//                        if (readByteOffset == 4)
//                        {
//                            readByteOffset = 0;
//                            longCounter++;
//                        }
//                        if (writeByteOffset == 2)
//                        {
//                            writeByteOffset = 0;
//                            wordCounter++;
//                        }
//                        byteCounter++;
//                    }
//
//                }
//                else if (((d0w & 0x0FFFFFFF) >= 0x02000000) &&
//                    ((d0w & 0x0FFFFFFF) < 0x5900000))
//                {
//                    // A-Bus transfer
//#ifdef _LOGS
//                    EmuState::pLog->Scu("A-Bus transfer", 0);
//#endif
//                    if (((d0r & 0x0FFFFFFF) < 0x05800000) ||
//                        ((d0r & 0x0FFFFFFF) >= 0x05900000))
//                    {
//                        source_add = 4;
//                    }
//                    if ((d0w & 0xFFFFFFF) < 0x5800000)
//                    {
//                        dest_add = 4;
//                    }
//                    else {
//                        if (dest_add) dest_add = 4;
//                    }
//
//                    while (byteCounter < static_cast<uint32_t>(d0c))
//                    {
//                        data = EmuState::pMem->ReadByte((d0r & 0x7FFFFFFF) + readByteOffset + longCounter * source_add);
//                        EmuState::pMem->WriteByte(d0w + readByteOffset + longCounter * dest_add, static_cast<uint8_t>(data));
//
//                        readByteOffset++;
//
//                        if (readByteOffset == 4)
//                        {
//                            readByteOffset = 0;
//                            longCounter++;
//                        }
//                        byteCounter++;
//                    }
//
//                }
//                else
//                {
//                    // other write
//                    if (((d0r & 0x0FFFFFFF) < 0x05800000) ||
//                        ((d0r & 0x0FFFFFFF) >= 0x05900000))
//                    {
//                        source_add = 4;
//                    }
//                    dest_add = 4;
//
//                    while (byteCounter < static_cast<uint32_t>(d0c))
//                    {
//                        data = EmuState::pMem->ReadByte((d0r & 0x7FFFFFFF) + readByteOffset + longCounter * source_add);
//                        EmuState::pMem->WriteByte(d0w + readByteOffset + longCounter * dest_add, static_cast<uint8_t>(data));
//
//                        readByteOffset++;
//
//                        if (readByteOffset == 4)
//                        {
//                            readByteOffset = 0;
//                            longCounter++;
//                        }
//                        byteCounter++;
//                    }
//                }
//
//                if (!GetBitValue(static_cast<uint32_t>(d0md), 16)) d0r += longCounter * source_add;
//                if (!GetBitValue(static_cast<uint32_t>(d0md), 8)) d0w += wordCounter * dest_add;
//
//
//                // Interrupt
//                EmuState::gInt->AddInterruptMaster(0x4B, 0x5);
//                startDma0 = false;
//
//                if (d0en & 0x1) {
//                    if ((d0md & 0x7) == DMA) {
//                        d0en &= 0xFFFFFFFE;
//                        EmuState::pMem->WriteLong(0x25FE0010, d0en);
//                        dmaUpdate_ = false;
//                    }
//                }
//#ifdef _LOGS
//                EmuState::pLog->ScuWrite("Level 0 DMA done");
//#endif	
//            }
//            break;
//        case 0x1:
//            // Indirect mode
//            // Start factor
//            switch (GetBitValue(static_cast<uint32_t>(d0md), 0, 2)) {
//                case VBLANKIN:
//                    if (currentStartFactor == VBLANKIN) startDma0 = true;
//                    break;
//                case VBLANKOUT:
//                    if (currentStartFactor == VBLANKOUT) startDma0 = true;
//                    break;
//                case HBLANKIN:
//                    if (currentStartFactor == HBLANKIN) startDma0 = true;
//                    break;
//                case TIMER0:
//                    if (currentStartFactor == TIMER0) startDma0 = true;
//                    break;
//                case TIMER1:
//                    if (currentStartFactor == TIMER1) startDma0 = true;
//                    break;
//                case SOUNDREQ:
//                    if (currentStartFactor == SOUNDREQ) startDma0 = true;
//                    break;
//                case SPRITEDRAWEND:
//                    if (currentStartFactor == SPRITEDRAWEND) startDma0 = true;
//                    break;
//                case DMA:
//                    // checking start factor bit
//                    if (GetBitValue(static_cast<uint32_t>(d0en), 0)) startDma0 = true;
//                    break;
//            }
//            // transfer start
//            if (startDma0) {
//                int32_t src, dest, count;
//                bool end;
//                int16_t source_add;
//                int16_t dest_add;
//                uint32_t byteCounter, wordCounter, longCounter;
//                int32_t readByteOffset, writeByteOffset;
//                uint32_t data;
//
//                // Source Address Add Value
//                if (d0ad & 0x100) source_add = 4;
//                else source_add = 0;
//
//                // Destination Address Add Value
//                switch (d0ad & 0x7)
//                {
//                    case 0: dest_add = 0;   break;
//                    case 1: dest_add = 2;   break;
//                    case 2: dest_add = 4;   break;
//                    case 3: dest_add = 8;  break;
//                    case 4: dest_add = 16;  break;
//                    case 5: dest_add = 32;  break;
//                    case 6: dest_add = 64; break;
//                    case 7: dest_add = 128; break;
//                }
//
//                end = false;
//                while (!end)
//                {
//                    byteCounter = wordCounter = longCounter = 0;
//                    readByteOffset = writeByteOffset = 0;
//
//                    src = EmuState::pMem->ReadLong(d0w + 8);
//                    dest = EmuState::pMem->ReadLong(d0w + 4);
//                    count = EmuState::pMem->ReadLong(d0w);
//
//                    if (count == 0) count = 0x100000;
//#ifdef _LOGS
//                    EmuState::pLog->ScuWrite("INDIRECT MODE DMA - Level 0");
//                    EmuState::pLog->Scu("Source : ", src & 0x7FFFFFFF);
//                    EmuState::pLog->Scu("Destination : ", dest);
//                    EmuState::pLog->Scu("Transfer size : ", count);
//#endif
//
//                    if (((dest & 0x0FFFFFFF) >= 0x05A00000) &&
//                        ((dest & 0x0FFFFFFF) < 0x6000000))
//                    {
//                        // B-Bus write
//                        // 32 bits splitted into 2*16 bits
//#ifdef _LOGS
//                        EmuState::pLog->Scu("B-Bus transfer", 0);
//#endif
//
//                        if (((src & 0x0FFFFFFF) < 0x05800000) ||
//                            ((src & 0x0FFFFFFF) >= 0x05900000))
//                        {
//                            source_add = 4;
//                        }
//                        while (byteCounter < static_cast<uint32_t>(count))
//                        {
//                            data = EmuState::pMem->ReadByte((src & 0x7FFFFFFF) + readByteOffset + longCounter * source_add);
//                            EmuState::pMem->WriteByte(dest + writeByteOffset + wordCounter * dest_add, static_cast<uint8_t>(data));
//
//                            readByteOffset++;
//                            writeByteOffset++;
//
//                            if (readByteOffset == 4)
//                            {
//                                readByteOffset = 0;
//                                longCounter++;
//                            }
//                            if (writeByteOffset == 2)
//                            {
//                                writeByteOffset = 0;
//                                wordCounter++;
//                            }
//                            byteCounter++;
//                        }
//
//                    }
//                    else if (((dest & 0x0FFFFFFF) >= 0x02000000) &&
//                        ((dest & 0x0FFFFFFF) < 0x5900000))
//                    {
//                        // A-Bus transfer
//#ifdef _LOGS
//                        EmuState::pLog->Scu("A-Bus transfer", 0);
//#endif
//                        if (((src & 0x0FFFFFFF) < 0x05800000) ||
//                            ((src & 0x0FFFFFFF) >= 0x05900000))
//                        {
//                            source_add = 4;
//                        }
//                        if ((dest & 0xFFFFFFF) < 0x5800000)
//                        {
//                            dest_add = 4;
//                        }
//                        else {
//                            if (dest_add) dest_add = 4;
//                        }
//
//                        while (byteCounter < static_cast<uint32_t>(count))
//                        {
//                            data = EmuState::pMem->ReadByte((src & 0x7FFFFFFF) + readByteOffset + longCounter * source_add);
//                            EmuState::pMem->WriteByte(dest + readByteOffset + longCounter * dest_add, static_cast<uint8_t>(data));
//
//                            readByteOffset++;
//
//                            if (readByteOffset == 4)
//                            {
//                                readByteOffset = 0;
//                                longCounter++;
//                            }
//                            byteCounter++;
//                        }
//
//                    }
//                    else
//                    {
//                        // other write
//                        if (((src & 0x0FFFFFFF) < 0x05800000) ||
//                            ((src & 0x0FFFFFFF) >= 0x05900000))
//                        {
//                            source_add = 4;
//                        }
//                        dest_add = 4;
//
//                        while (byteCounter < static_cast<uint32_t>(count))
//                        {
//                            data = EmuState::pMem->ReadByte((src & 0x7FFFFFFF) + readByteOffset + longCounter * source_add);
//                            EmuState::pMem->WriteByte(dest + readByteOffset + longCounter * dest_add, static_cast<uint8_t>(data));
//
//                            readByteOffset++;
//
//                            if (readByteOffset == 4)
//                            {
//                                readByteOffset = 0;
//                                longCounter++;
//                            }
//                            byteCounter++;
//                        }
//                    }
//
//                    if (src & 0x80000000) end = true; // end code check
//                    d0w += 0xC;
//                }
//
//                if (GetBitValue(static_cast<uint32_t>(d0md), 16)) EmuState::pMem->WriteLong(0x25FE0004, d0w); // Update ?
//
//                // Interrupt
//                EmuState::gInt->AddInterruptMaster(0x4B, 0x5);
//                startDma0 = false;
//                if (GetBitValue(static_cast<uint32_t>(d0en), 0)) {
//                    if (GetBitValue(static_cast<uint32_t>(d0md), 0, 2) == DMA) {
//                        d0en &= 0xFFFFFFFE;
//                        EmuState::pMem->WriteLong(0x25FE0010, d0en);
//                        dmaUpdate_ = false;
//                    }
//                }
//
//    dc.dma_mode == DmaLevel::
//#ifdef _LOGS
//                EmuState::pLog->ScuWrite("Level 0 Indirect DMA done");
//#endif
//            }
//            break;
//    }
//}

}

void Scu::setInterruptStatusRegister(const Interrupt& i) {
    auto isr = InterruptStatusRegister(rawRead<u32>(scuMemory(), interrupt_status_register & scu_memory_mask));
    isr.set(i.status);
    rawWrite<u32>(scuMemory(), interrupt_status_register & scu_memory_mask, isr.toUlong());
};

void Scu::resetInterruptStatusRegister(const Interrupt& i) {
    auto isr = InterruptStatusRegister(rawRead<u32>(scuMemory(), interrupt_status_register & scu_memory_mask));
    isr.reset(i.status);
    rawWrite<u32>(scuMemory(), interrupt_status_register & scu_memory_mask, isr.toUlong());
};

bool Scu::isInterruptMasked(const Interrupt& i, Sh2Type t) const {
    auto imr = InterruptMaskRegister(rawRead<u32>(scuMemory(), interrupt_mask_register & scu_memory_mask));
    switch(t) {
        case Sh2Type::master:
            switch (i) {
                case is::vector_nmi: 
                    // NMI interrupt is always accepted
                    return true; 
                    break;
                case is::vector_system_manager:
                    if (emulator_context_->hardwareMode_ == HardwareMode::stv) return true;
                    else return (imr.get(i.mask) == InterruptMask::masked);
                    break;
                case is::vector_v_blank_in:
                case is::vector_v_blank_out:
                case is::vector_h_blank_in:
                case is::vector_timer_0:
                case is::vector_timer_1:
                case is::vector_dsp_end:
                case is::vector_sound_request:
                case is::vector_pad_interrupt:
                case is::vector_level_2_dma_end:
                case is::vector_level_1_dma_end:
                case is::vector_level_0_dma_end:
                case is::vector_dma_illegal:
                case is::vector_sprite_draw_end:
                case is::vector_external_00:
                case is::vector_external_01:
                case is::vector_external_02:
                case is::vector_external_03:
                case is::vector_external_04:
                case is::vector_external_05:
                case is::vector_external_06:
                case is::vector_external_07:
                case is::vector_external_08:
                case is::vector_external_09:
                case is::vector_external_10:
                case is::vector_external_11:
                case is::vector_external_12:
                case is::vector_external_13:
                case is::vector_external_14:
                case is::vector_external_15:
                case is::vector_frt_input_capture:
                    return (imr.get(i.mask) == InterruptMask::masked);
                    break;
                default: 
                    return false;
                    break;
            }
            break;
        case Sh2Type::slave:
            switch (i) {
                case is::vector_nmi:
                case is::vector_v_blank_in:
                case is::vector_h_blank_in:
                case is::vector_frt_input_capture:
                    return true;
                    break;
                default:
                    return false;
            }
            break;
    } 
    return false;
}

void Scu::initializeRegisters() {
    // DMA
    rawWrite<u32>(scuMemory(), level_0_dma_add_value_register & scu_memory_mask, 0x00000101);
    rawWrite<u32>(scuMemory(), level_0_dma_enable_register    & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), level_0_dma_mode_register      & scu_memory_mask, 0x00000007);
    rawWrite<u32>(scuMemory(), level_1_dma_add_value_register & scu_memory_mask, 0x00000101);
    rawWrite<u32>(scuMemory(), level_1_dma_enable_register    & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), level_1_dma_mode_register      & scu_memory_mask, 0x00000007);
    rawWrite<u32>(scuMemory(), level_2_dma_add_value_register & scu_memory_mask, 0x00000101);
    rawWrite<u32>(scuMemory(), level_2_dma_enable_register    & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), level_2_dma_mode_register      & scu_memory_mask, 0x00000007);
    rawWrite<u32>(scuMemory(), dma_forced_stop                & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), (dma_status_register + 0x0C)   & scu_memory_mask, 0x00000000);

    // DSP
    rawWrite<u32>(scuMemory(), dsp_program_control_port  & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), dsp_data_ram_address_port & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), dsp_data_ram_data_port    & scu_memory_mask, 0x00000000);

    // Timer
    rawWrite<u32>(scuMemory(), timer_1_mode_register & scu_memory_mask, 0x00000000);

    // Interrupt control
    rawWrite<u32>(scuMemory(), interrupt_mask_register   & scu_memory_mask, 0x0000BFFF);
    rawWrite<u32>(scuMemory(), interrupt_status_register & scu_memory_mask, 0x00000000);

    // A-BUS control
    rawWrite<u32>(scuMemory(), a_bus_interrupt_acknowledge & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), a_bus_set_register          & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), (a_bus_set_register + 4)    & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), a_bus_refresh_register      & scu_memory_mask, 0x00000000);

    // SCU control
    rawWrite<u32>(scuMemory(), scu_sdram_select_register & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), scu_version_register      & scu_memory_mask, 0x00000000);

    //d0en = 0;
    //d1en = 0;
    //d2en = 0;
    //dsta = 0;
}

DmaConfiguration Scu::configureDmaTransfer(DmaLevel level) const {
    DmaConfiguration dc{};
    switch (level) {
        case DmaLevel::level_0:
            initializeDmaReadAddress(dc, level_0_dma_read_address);
            initializeDmaWriteAddress(dc, level_0_dma_write_address);
            initializeDmaTransferByteNumber(dc, level_0_dma_transfer_byte_number);
            initializeDmaAddressAdd(dc, level_0_dma_add_value_register);
            initializeDmaEnable(dc, level_0_dma_enable_register);
            initializeDmaMode(dc, level_0_dma_mode_register);
            break;
        case DmaLevel::level_1:
            initializeDmaReadAddress(dc, level_1_dma_read_address);
            initializeDmaWriteAddress(dc, level_1_dma_write_address);
            initializeDmaTransferByteNumber(dc, level_1_dma_transfer_byte_number);
            initializeDmaAddressAdd(dc, level_1_dma_add_value_register);
            initializeDmaEnable(dc, level_1_dma_enable_register);
            initializeDmaMode(dc, level_1_dma_mode_register);
            break;
        case DmaLevel::level_2:
            initializeDmaReadAddress(dc, level_2_dma_read_address);
            initializeDmaWriteAddress(dc, level_2_dma_write_address);
            initializeDmaTransferByteNumber(dc, level_2_dma_transfer_byte_number);
            initializeDmaAddressAdd(dc, level_2_dma_add_value_register);
            initializeDmaEnable(dc, level_2_dma_enable_register);
            initializeDmaMode(dc, level_2_dma_mode_register);
            break;
        default:
            break;
    }
    dc.dma_level = level;
    return dc;
}

void Scu::initializeDmaMode(DmaConfiguration& dc, const u32 register_address) const {
    auto dmr                  = DmaModeRegister(rawRead<u32>(scuMemory(), register_address & scu_memory_mask));
    dc.dma_mode               = dmr.get(DmaModeRegister::dmaMode);
    dc.read_address_update    = dmr.get(DmaModeRegister::readAddressUpdate);
    dc.write_address_update   = dmr.get(DmaModeRegister::writeAddressUpdate);
    dc.starting_factor_select = dmr.get(DmaModeRegister::startingFactorSelect);
}

void Scu::initializeDmaEnable(DmaConfiguration& dc, const u32 register_address) const {
    auto der        = DmaEnableRegister(rawRead<u32>(scuMemory(), register_address & scu_memory_mask));
    dc.dma_enable   = der.get(DmaEnableRegister::dmaEnable);
    dc.dma_starting = der.get(DmaEnableRegister::dmaStarting);
}

void Scu::initializeDmaAddressAdd(DmaConfiguration& dc, const u32 register_address) const {
    auto avr           = DmaAddressAddValueRegister(rawRead<u32>(scuMemory(), register_address & scu_memory_mask));
    dc.read_add_value  = avr.get(DmaAddressAddValueRegister::readAddValue);
    dc.write_add_value = avr.get(DmaAddressAddValueRegister::writeAddValue);
}

void Scu::initializeDmaTransferByteNumber(DmaConfiguration& dc, const u32 register_address) const {
    switch (register_address & scu_memory_mask) {
        case (level_0_dma_transfer_byte_number & 0xFF): {
            auto tbnr               = DmaLevel2TransferByteNumberRegister(rawRead<u32>(scuMemory(), level_0_dma_transfer_byte_number & scu_memory_mask));
            dc.transfer_byte_number = tbnr.get(DmaLevel2TransferByteNumberRegister::transferByteNumber);
            break;
        }
        case (level_1_dma_transfer_byte_number & 0xFF): {
            auto tbnr               = DmaLevel2TransferByteNumberRegister(rawRead<u32>(scuMemory(), level_1_dma_transfer_byte_number & scu_memory_mask));
            dc.transfer_byte_number = tbnr.get(DmaLevel2TransferByteNumberRegister::transferByteNumber);
            break;
        }
        case (level_2_dma_transfer_byte_number & 0xFF): {
            auto tbnr               = DmaLevel2TransferByteNumberRegister(rawRead<u32>(scuMemory(), level_2_dma_transfer_byte_number & scu_memory_mask));
            dc.transfer_byte_number = tbnr.get(DmaLevel2TransferByteNumberRegister::transferByteNumber);
            break;
        }
        default:
            Log::error("scu", "Unknown DMA Transfer Byte Number register address ! {}", register_address);
            break;
    }
}
void Scu::initializeDmaWriteAddress(DmaConfiguration& dc, const u32 register_address) const {
    auto war         = DmaWriteAddressRegister(rawRead<u32>(scuMemory(), register_address & scu_memory_mask));
    dc.write_address = war.get(DmaWriteAddressRegister::writeAddress);
}

void Scu::initializeDmaReadAddress(DmaConfiguration& dc, const u32 register_address) const {
    auto rar        = DmaReadAddressRegister(rawRead<u32>(scuMemory(), register_address & scu_memory_mask));
    dc.read_address = rar.get(DmaReadAddressRegister::readAddress);
}

}
}
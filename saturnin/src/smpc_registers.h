// 
// smpc_registers.h
// Saturnin
//
// Copyright (c) 2019 Renaud Toumazet
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	smpc_registers.h
///
/// \brief	Declares SMPC onchip registers and related enums. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "emulator_defs.h"

namespace saturnin {
namespace core {

/// \name SH2 interface registers addresses
//@{
constexpr u32 command_register          = 0x2010001F;
constexpr u32 status_register           = 0x20100061;
constexpr u32 status_flag               = 0x20100063;
constexpr u32 input_register_0          = 0x20100001;
constexpr u32 input_register_1          = 0x20100003;
constexpr u32 input_register_2          = 0x20100005;
constexpr u32 input_register_3          = 0x20100007;
constexpr u32 input_register_4          = 0x20100009;
constexpr u32 input_register_5          = 0x2010000B;
constexpr u32 input_register_6          = 0x2010000D;
constexpr u32 output_register_0         = 0x20100021;
constexpr u32 output_register_1         = 0x20100023;
constexpr u32 output_register_2         = 0x20100025;
constexpr u32 output_register_3         = 0x20100027;
constexpr u32 output_register_4         = 0x20100029;
constexpr u32 output_register_5         = 0x2010002B;
constexpr u32 output_register_6         = 0x2010002D;
constexpr u32 output_register_7         = 0x2010002F;
constexpr u32 output_register_8         = 0x20100031;
constexpr u32 output_register_9         = 0x20100033;
constexpr u32 output_register_10        = 0x20100035;
constexpr u32 output_register_11        = 0x20100037;
constexpr u32 output_register_12        = 0x20100039;
constexpr u32 output_register_13        = 0x2010003B;
constexpr u32 output_register_14        = 0x2010003D;
constexpr u32 output_register_15        = 0x2010003F;
constexpr u32 output_register_16        = 0x20100041;
constexpr u32 output_register_17        = 0x20100043;
constexpr u32 output_register_18        = 0x20100045;
constexpr u32 output_register_19        = 0x20100047;
constexpr u32 output_register_20        = 0x20100049;
constexpr u32 output_register_21        = 0x2010004B;
constexpr u32 output_register_22        = 0x2010004D;
constexpr u32 output_register_23        = 0x2010004F;
constexpr u32 output_register_24        = 0x20100051;
constexpr u32 output_register_25        = 0x20100053;
constexpr u32 output_register_26        = 0x20100055;
constexpr u32 output_register_27        = 0x20100057;
constexpr u32 output_register_28        = 0x20100059;
constexpr u32 output_register_29        = 0x2010005B;
constexpr u32 output_register_30        = 0x2010005D;
constexpr u32 output_register_31        = 0x2010005F;
constexpr u32 data_direction_register_1 = 0x20100079;
constexpr u32 data_direction_register_2 = 0x2010007B;
constexpr u32 port_data_register_1      = 0x20100075;
constexpr u32 port_data_register_2      = 0x20100077;
constexpr u32 io_select_register        = 0x2010007D;
constexpr u32 external_latch_register   = 0x2010007D;
//@}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CommandRegister
///
/// \brief  Command Register (COMREG).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class CommandRegister : public Register {
public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{ 0, 7 };   ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  StatusRegister
///
/// \brief  Status Register (SR). 
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class StatusRegister : public Register {
public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{ 0, 7 };   ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  StatusFlag
///
/// \brief  Status Flag (SF).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class StatusFlag : public Register {
public:
    using Register::Register;
    inline static const BitRange<u8> sf{ 0 };  ///< SF value.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  InputRegister
///
/// \brief  Input Register (IREGx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class InputRegister : public Register {
public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{ 0, 7 };   ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  OutputRegister
///
/// \brief  Output Register (OREGx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class OutputRegister : public Register {
public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{ 0, 7 };   ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DataDirectionRegister
///
/// \brief  Data Direction Register (DDRx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DataDirectionRegister : public Register {
public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{ 0, 6 };         ///< Defines the range of all the bits of the register.
    inline static const u32 accessMask() { return 0b1111111; } ///< Returns access mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PortDataRegister
///
/// \brief  Port Data Register (PDRx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class PortDataRegister : public Register {
public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{ 0, 6 };         ///< Defines the range of all the bits of the register.
    inline static const u32 accessMask() { return 0b1111111; } ///< Returns access mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralPortMode
///
/// \brief  IOSEL values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralPortMode : u8 {
    smpc_control_mode = 0, ///< SMPC control mode. (initial)
    sh2_direct_mode   = 1, ///< SH2 direct mode 
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  IOSelect
///
/// \brief  I/O Select (IOSEL).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class IOSelect : public Register {
public:
    using Register::Register;
    inline static const BitRange<PeripheralPortMode> peripheral_port_2_mode { 1 }; ///< IOSEL2 bit.
    inline static const BitRange<PeripheralPortMode> peripheral_port_1_mode { 0 }; ///< IOSEL1 bit.
    inline static const BitRange<u8> all_bits{ 0, 1 };                             ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ExternalLatch
///
/// \brief  EXLE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ExternalLatch : u8 {
    disabled = 0, ///< Disabled. (initial)
    enabled = 1, ///< Enabled 
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ExternalLatchEnable
///
/// \brief  External Latch Enable (EXLE).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class ExternalLatchEnable : public Register {
public:
    using Register::Register;
    inline static const BitRange<ExternalLatch> external_latch_2_enable { 1 }; ///< EXLE2 bit.
    inline static const BitRange<ExternalLatch> external_latch_1_enable { 0 }; ///< EXLE1 bit.
    inline static const BitRange<u8> all_bits{ 0, 1 };                         ///< Defines the range of all the bits of the register.
};

}
}

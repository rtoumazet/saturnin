//
// memory_constants.h
// Saturnin
//
// Copyright (c) 2024 Renaud Toumazet
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

#pragma once

#include <saturnin/src/emulator_defs.h>

namespace saturnin::core {

constexpr auto workram_low_size             = u32{0x100000};
constexpr auto workram_high_size            = u32{0x100000};
constexpr auto rom_size                     = u32{0x80000};
constexpr auto smpc_size                    = u8{0x80};
constexpr auto backup_ram_size              = u32{0x10000};
constexpr auto scu_size                     = u8{0xD0};
constexpr auto vdp2_vram_size               = u32{0x80000};
constexpr auto vdp2_cram_size               = u16{0x1000};
constexpr auto vdp2_registers_size          = u16{0x200};
constexpr auto vdp1_vram_size               = u32{0x80000};
constexpr auto vdp1_framebuffer_size        = u32{0x40000};
constexpr auto vdp1_registers_size          = u8{0x18};
constexpr auto sound_ram_size               = u32{0x100000};
constexpr auto stv_io_size                  = u16{0x100};
constexpr auto cart_size                    = u32{0x3000000};
constexpr auto rom_memory_mask              = u32{0x7FFFF};
constexpr auto smpc_memory_mask             = u32{0x7F};
constexpr auto backup_ram_memory_mask       = u32{0xFFFF};
constexpr auto workram_low_memory_mask      = u32{0xFFFFF};
constexpr auto stv_io_memory_mask           = u32{0xFF};
constexpr auto cart_memory_mask             = u32{0x01FFFFFF};
constexpr auto vdp1_ram_memory_mask         = u32{0x7FFFF};
constexpr auto vdp1_framebuffer_memory_mask = u32{0x3FFFF};
constexpr auto vdp1_registers_memory_mask   = u32{0x1F};
constexpr auto vdp2_vram_memory_mask        = u32{0x7FFFF};
constexpr auto vdp2_cram_memory_mask        = u32{0xFFF};
constexpr auto vdp2_registers_memory_mask   = u32{0x1FF};
constexpr auto scu_memory_mask              = u32{0xFF};
constexpr auto workram_high_memory_mask     = u32{0xFFFFF};
constexpr auto stv_io_port_a                = u32{0x400001};
constexpr auto stv_io_port_b                = u32{0x400003};
constexpr auto stv_io_port_c                = u32{0x400005};
constexpr auto stv_io_port_d                = u32{0x400007};

constexpr auto stv_protection_register_address = u32{0x04FFFFFC};
constexpr auto stv_protection_enabled          = u32{0x04FFFFF1};

constexpr auto memory_handler_size = u32{0x10000};

constexpr auto cart_absolute_address = u32{0x02000000};

constexpr auto full_memory_map_size = u32{0x10000000};

constexpr auto vdp2_page_disp           = u8{11};
constexpr auto vdp2_minimum_page_size   = u16{0x800};
constexpr auto vdp2_bitmap_disp         = u8{17};
constexpr auto vdp2_minimum_bitmap_size = u32{0x20000};

} // namespace saturnin::core

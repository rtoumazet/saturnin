//
// pch.h
// Saturnin
//
// Copyright (c) 2020 Renaud Toumazet
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
/// \file	pch.h
///
/// \brief	Precompiled headers.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// STL
#include <any>
#include <array>
#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstdint>
#include <date/date.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future> // test if still needed
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>

// Windows API
#define NOMINMAX
#include <windows.h>

// Windows SDK
#include <ntddscsi.h>
#include <ntddcdrm.h>

// External libs
#pragma warning(disable : 4275) // libconfig specific warning disable
#include <libconfig.h++>

#include <lodepng.h>

//#include <fmt/format.h>

#include <libzippp/libzippp.h>

#define SPDLOG_FMT_EXTERNAL
#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/ostream_sink.h>

#pragma warning(push)
#pragma warning(disable : 4267)
#include <spirit_po/spirit_po.hpp>
#pragma warning(pop)

// Project headers
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/emulator_enums.h>
#include <saturnin/src/locale.h>
#include <saturnin/src/log.h>
#include <saturnin/src/stv_definitions.h>
#include <saturnin/src/scu_registers.h>
#include <saturnin/src/interrupt_sources.h> // Needs scu_registers
#include <saturnin/src/sh2_registers.h>
#include <saturnin/src/smpc_registers.h>

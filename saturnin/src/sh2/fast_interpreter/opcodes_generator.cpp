//
// opcodes_generator.h
// Saturnin
//
// Copyright (c) 2023 Renaud Toumazet
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
#include <saturnin/src/sh2/fast_interpreter/opcodes_generator.h>
#include <saturnin/src/utilities.h> // format

namespace uti = saturnin::utilities;

namespace saturnin::sh2 {

constexpr auto call_max    = 0x10000;
constexpr auto counter_max = 0x10;
auto           generateOpcodes() -> bool {
    // Lookup table generation.
    std::string       calls{};
    std::array<u8, 4> counters{};
    for (int call_counter = 0; call_counter < call_max; ++call_counter) {
        calls += uti::format("\ncall_{0:x}_{1:x}_{2:x}_{3:x}", counters[0], counters[1], counters[2], counters[3]);
        if (call_counter < call_max - 1) { calls += ","; } // Prevents the last comma.
        ++counters[3];
        if (counters[3] == counter_max) {
            counters[3] = 0;
            ++counters[2];
        }
        if (counters[2] == counter_max) {
            counters[2] = 0;
            ++counters[1];
        }
        if (counters[1] == counter_max) {
            counters[1] = 0;
            ++counters[0];
        }
        if (counters[0] == counter_max) {
            // end of loops
        }
    }

    std::string content{uti::format("std::array<OpcodeFunc, 2> opcodes_func{{ {0} }};", calls)};

    // Write to the file.
    const auto path = std::string{"./sh2_opcodes.inc"};
    auto       file = std::ofstream(path, std::ios::binary);
    file << content;
    file.close();

    return true;
}
}; // namespace saturnin::sh2

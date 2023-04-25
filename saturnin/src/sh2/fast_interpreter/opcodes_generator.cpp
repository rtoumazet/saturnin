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

auto generateOpcodes() -> bool {
    // Functions generation.
    std::string functions{};
    functions += generateFunctions<FunctionType::anmd>(std::string_view("add"), {3, 0xc});

    // Lookup table generation.
    constexpr auto    call_max    = 0x10000;
    constexpr auto    counter_max = 0x10;
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

    std::string content{uti::format("{0}\n std::array<OpcodeFunc, 2> opcodes_func{{ {1} }};", functions, calls)};

    // Write to the file.
    const auto path = std::string{"./sh2_opcodes.inc"};
    auto       file = std::ofstream(path, std::ios::binary);
    file << content;
    file.close();

    return true;
}

template<FunctionType Type>
auto generateFunctions(std::string_view func_name, const std::vector<int>& args) -> std::string {
    std::string generated{};
    switch (Type) {
        using enum FunctionType;
        case anmd: {
            // 3xxC
            if (args.size() != 2) {
                generated = uti::format("Wrong number of arguments for {}", func_name);
                break;
            }

            constexpr auto    func_nb_max = 0x100;
            constexpr auto    counter_max = 0x10;
            std::array<u8, 2> counters{};

            const auto a = args[0];
            const auto d = args[1];

            auto func_template = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    {4}(s,{1:x},{2:x});
}})";

            for (int func_counter = 0; func_counter < func_nb_max; ++func_counter) {
                generated += uti::format(func_template, a, counters[0], counters[1], d, func_name);
                ++counters[1];
                if (counters[1] == counter_max) {
                    counters[1] = 0;
                    ++counters[0];
                }
            }

            break;
        }
            // default:
    }
    return generated;
}

}; // namespace saturnin::sh2

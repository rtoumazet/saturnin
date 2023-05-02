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
    using enum FunctionType;
    std::string functions{};
    functions += generateFunctions<frrf>(std::string_view("add"), {3, 12});
    functions += generateFunctions<frxx>(std::string_view("addi"), {7});
    functions += generateFunctions<frrf>(std::string_view("addc"), {3, 14});
    functions += generateFunctions<frrf>(std::string_view("addv"), {3, 15});
    functions += generateFunctions<frrf>(std::string_view("and_op"), {2, 9});
    functions += generateFunctions<ffxx>(std::string_view("andi"), {12, 9});
    functions += generateFunctions<ffxx>(std::string_view("andm"), {12, 13});
    functions += generateFunctions<ffxx>(std::string_view("bf"), {8, 11});
    functions += generateFunctions<ffxx>(std::string_view("bfs"), {8, 15});
    functions += generateFunctions<fxxx>(std::string_view("bra"), {10});
    functions += generateFunctions<frff>(std::string_view("braf"), {0, 2, 3});
    functions += generateFunctions<fxxx>(std::string_view("bsr"), {11});
    functions += generateFunctions<frff>(std::string_view("bsrf"), {0, 0, 3});
    functions += generateFunctions<ffxx>(std::string_view("bt"), {8, 9});
    functions += generateFunctions<ffxx>(std::string_view("bts"), {8, 13});
    functions += generateFunctions<ffff>(std::string_view("clrmac"), {0, 0, 2, 8});
    functions += generateFunctions<ffff>(std::string_view("clrt"), {0, 0, 0, 8});
    functions += generateFunctions<frrf>(std::string_view("cmpeq"), {3, 0});
    functions += generateFunctions<frrf>(std::string_view("cmpge"), {3, 3});
    functions += generateFunctions<frrf>(std::string_view("cmpgt"), {3, 7});
    functions += generateFunctions<frrf>(std::string_view("cmphi"), {3, 6});
    functions += generateFunctions<frrf>(std::string_view("cmphs"), {3, 2});
    functions += generateFunctions<frff>(std::string_view("cmppl"), {4, 1, 5});
    functions += generateFunctions<frff>(std::string_view("cmppz"), {4, 1, 1});
    functions += generateFunctions<frrf>(std::string_view("cmpstr"), {2, 12});
    functions += generateFunctions<ffxx>(std::string_view("cmpim"), {8, 8});
    functions += generateFunctions<frrf>(std::string_view("div0s"), {2, 7});
    functions += generateFunctions<ffff>(std::string_view("div0u"), {0, 0, 1, 9});
    functions += generateFunctions<frrf>(std::string_view("div1"), {3, 4});
    functions += generateFunctions<frrf>(std::string_view("dmuls"), {3, 13});
    functions += generateFunctions<frrf>(std::string_view("dmulu"), {3, 5});
    functions += generateFunctions<frff>(std::string_view("dt"), {4, 1, 0});
    functions += generateFunctions<frrf>(std::string_view("extsb"), {6, 14});
    functions += generateFunctions<frrf>(std::string_view("extsw"), {6, 15});
    functions += generateFunctions<frrf>(std::string_view("extub"), {6, 12});
    functions += generateFunctions<frrf>(std::string_view("extuw"), {6, 13});
    functions += generateFunctions<frff>(std::string_view("jmp"), {4, 2, 11});
    functions += generateFunctions<frff>(std::string_view("jsr"), {4, 0, 11});
    functions += generateFunctions<frff>(std::string_view("ldcsr"), {4, 0, 14});
    functions += generateFunctions<frff>(std::string_view("ldcgbr"), {4, 1, 14});
    functions += generateFunctions<frff>(std::string_view("ldcvbr"), {4, 2, 14});
    functions += generateFunctions<frff>(std::string_view("ldcmsr"), {4, 0, 7});
    functions += generateFunctions<frff>(std::string_view("ldcmgbr"), {4, 1, 7});
    functions += generateFunctions<frff>(std::string_view("ldcmvbr"), {4, 2, 7});
    functions += generateFunctions<frff>(std::string_view("ldsmach"), {4, 0, 10});
    functions += generateFunctions<frff>(std::string_view("ldsmacl"), {4, 1, 10});
    functions += generateFunctions<frff>(std::string_view("ldspr"), {4, 2, 10});
    functions += generateFunctions<frff>(std::string_view("ldsmmach"), {4, 0, 6});
    functions += generateFunctions<frff>(std::string_view("ldsmmacl"), {4, 1, 6});
    functions += generateFunctions<frff>(std::string_view("ldsmpr"), {4, 2, 6});
    functions += generateFunctions<frrf>(std::string_view("mac"), {0, 15});
    functions += generateFunctions<frrf>(std::string_view("macw"), {4, 15});
    functions += generateFunctions<frrf>(std::string_view("mov"), {6, 3});
    functions += generateFunctions<frrf>(std::string_view("movbs"), {2, 0});
    functions += generateFunctions<frrf>(std::string_view("movws"), {2, 1});
    functions += generateFunctions<frrf>(std::string_view("movls"), {2, 2});
    functions += generateFunctions<frrf>(std::string_view("movbl"), {6, 0});
    functions += generateFunctions<frrf>(std::string_view("movwl"), {6, 1});
    functions += generateFunctions<frrf>(std::string_view("movll"), {6, 2});
    functions += generateFunctions<frrf>(std::string_view("movbm"), {2, 4});
    functions += generateFunctions<frrf>(std::string_view("movwm"), {2, 5});
    functions += generateFunctions<frrf>(std::string_view("movlm"), {2, 6});
    functions += generateFunctions<frrf>(std::string_view("movbp"), {6, 4});
    functions += generateFunctions<frrf>(std::string_view("movwp"), {6, 5});
    functions += generateFunctions<frrf>(std::string_view("movlp"), {6, 6});
    functions += generateFunctions<frrf>(std::string_view("movbs0"), {0, 4});
    functions += generateFunctions<frrf>(std::string_view("movws0"), {0, 5});
    functions += generateFunctions<frrf>(std::string_view("movls0"), {0, 6});
    functions += generateFunctions<frrf>(std::string_view("movbl0"), {0, 12});
    functions += generateFunctions<frrf>(std::string_view("movwl0"), {0, 13});
    functions += generateFunctions<frrf>(std::string_view("movll0"), {0, 14});
    functions += generateFunctions<frxx>(std::string_view("movi"), {14});
    functions += generateFunctions<frxx>(std::string_view("movwi"), {9});
    functions += generateFunctions<frxx>(std::string_view("movli"), {13});
    functions += generateFunctions<ffxx>(std::string_view("movblg"), {12, 4});
    functions += generateFunctions<ffxx>(std::string_view("movwlg"), {12, 5});
    functions += generateFunctions<ffxx>(std::string_view("movllg"), {12, 6});
    functions += generateFunctions<ffxx>(std::string_view("movbsg"), {12, 0});
    functions += generateFunctions<ffxx>(std::string_view("movwsg"), {12, 1});
    functions += generateFunctions<ffxx>(std::string_view("movlsg"), {12, 2});
    functions += generateFunctions<ffrx>(std::string_view("movbs4"), {8, 0});
    functions += generateFunctions<ffrx>(std::string_view("movws4"), {8, 1});
    functions += generateFunctions<frrx>(std::string_view("movls4"), {1});
    functions += generateFunctions<ffrx>(std::string_view("movbl4"), {8, 4});
    functions += generateFunctions<ffrx>(std::string_view("movwl4"), {8, 5});
    functions += generateFunctions<frrx>(std::string_view("movll4"), {5});
    functions += generateFunctions<ffxx>(std::string_view("mova"), {12, 7});
    functions += generateFunctions<frff>(std::string_view("movt"), {0, 2, 9});
    functions += generateFunctions<frrf>(std::string_view("mull"), {0, 7});
    functions += generateFunctions<frrf>(std::string_view("muls"), {2, 15});
    functions += generateFunctions<frrf>(std::string_view("mulu"), {2, 14});
    functions += generateFunctions<frrf>(std::string_view("neg"), {6, 11});
    functions += generateFunctions<frrf>(std::string_view("negc"), {6, 10});
    functions += generateFunctions<ffff>(std::string_view("nop"), {0, 0, 0, 9});
    functions += generateFunctions<frrf>(std::string_view("not_op"), {6, 7});
    functions += generateFunctions<frrf>(std::string_view("or_op"), {2, 11});
    functions += generateFunctions<ffxx>(std::string_view("ori"), {12, 11});
    functions += generateFunctions<ffxx>(std::string_view("orm"), {12, 15});
    functions += generateFunctions<frff>(std::string_view("rotcl"), {4, 2, 4});
    functions += generateFunctions<frff>(std::string_view("rotcr"), {4, 2, 5});
    functions += generateFunctions<frff>(std::string_view("rotl"), {4, 0, 4});
    functions += generateFunctions<frff>(std::string_view("rotr"), {4, 0, 5});
    functions += generateFunctions<ffff>(std::string_view("rte"), {0, 0, 2, 11});
    functions += generateFunctions<ffff>(std::string_view("rts"), {0, 0, 0, 11});
    functions += generateFunctions<ffff>(std::string_view("sett"), {0, 0, 1, 8});
    functions += generateFunctions<frff>(std::string_view("shal"), {4, 2, 0});
    functions += generateFunctions<frff>(std::string_view("shar"), {4, 2, 1});
    functions += generateFunctions<frff>(std::string_view("shll"), {4, 0, 0});
    functions += generateFunctions<frff>(std::string_view("shll2"), {4, 0, 8});
    functions += generateFunctions<frff>(std::string_view("shll8"), {4, 1, 8});
    functions += generateFunctions<frff>(std::string_view("shll16"), {4, 2, 8});
    functions += generateFunctions<frff>(std::string_view("shlr"), {4, 0, 1});
    functions += generateFunctions<frff>(std::string_view("shlr2"), {4, 0, 9});
    functions += generateFunctions<frff>(std::string_view("shlr8"), {4, 1, 9});
    functions += generateFunctions<frff>(std::string_view("shlr16"), {4, 2, 9});
    functions += generateFunctions<ffff>(std::string_view("sleep"), {0, 0, 1, 11});
    functions += generateFunctions<frff>(std::string_view("stcsr"), {0, 0, 2});
    functions += generateFunctions<frff>(std::string_view("stcgbr"), {0, 1, 2});
    functions += generateFunctions<frff>(std::string_view("stcvbr"), {0, 2, 2});

    // stcmsr(Sh2 & s, const u32 n);                           // frff
    // stcmgbr(Sh2 & s, const u32 n);                          // frff
    // stcmvbr(Sh2 & s, const u32 n);                          // frff
    // stsmach(Sh2 & s, const u32 n);                          // frff
    // stsmacl(Sh2 & s, const u32 n);                          // frff
    // stspr(Sh2 & s, const u32 n);                            // frff
    // stsmmach(Sh2 & s, const u32 n);                         // frff
    // stsmmacl(Sh2 & s, const u32 n);                         // frff
    // stsmpr(Sh2 & s, const u32 n);                           // frff
    // sub(Sh2 & s, const u32 n, const u32 m);                 // frrf
    // subc(Sh2 & s, const u32 n, const u32 m);                // frrf
    // subv(Sh2 & s, const u32 n, const u32 m);                // frrf
    // swapb(Sh2 & s, const u32 n, const u32 m);               // frrf
    // swapw(Sh2 & s, const u32 n, const u32 m);               // frrf
    // tas(Sh2 & s, const u32 n);                              // frff
    // trapa(Sh2 & s, const u32 i);                            // ffxx
    // tst(Sh2 & s, const u32 n, const u32 m);                 // frrf
    // tsti(Sh2 & s, const u32 i);                             // ffxx
    // tstm(Sh2 & s, const u32 i);                             // ffxx
    // xor_op(Sh2 & s, const u32 n, const u32 m);              // frrf
    // xori(Sh2 & s, const u32 i);                             // ffxx
    // xorm(Sh2 & s, const u32 i);                             // ffxx
    // xtrct(Sh2 & s, const u32 n, const u32 m);               // frrf

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
        case ffff: {
            if (args.size() != 4) {
                generated = uti::format("Wrong number of arguments for {}", func_name);
                break;
            }
            auto func_template = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    {4}(s);
}})";

            break;
        }
        case frrf: {
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

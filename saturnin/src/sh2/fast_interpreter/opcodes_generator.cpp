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
    functions += generateFunctions<FunctionType::frrf>(std::string_view("add"), {3, 12});
    functions += generateFunctions<FunctionType::frxx>(std::string_view("addi"), {7});
    functions += generateFunctions<FunctionType::frrf>(std::string_view("addc"), {3, 14});
    functions += generateFunctions<FunctionType::frrf>(std::string_view("addv"), {3, 15});
    functions += generateFunctions<FunctionType::frrf>(std::string_view("and_op"), {2, 9});
    functions += generateFunctions<FunctionType::ffxx>(std::string_view("andi"), {12, 9});
    functions += generateFunctions<FunctionType::ffxx>(std::string_view("andm"), {12, 13});
    functions += generateFunctions<FunctionType::ffxx>(std::string_view("bf"), {8, 11});
    functions += generateFunctions<FunctionType::ffxx>(std::string_view("bfs"), {8, 15});
    functions += generateFunctions<FunctionType::fxxx>(std::string_view("bra"), {10});
    functions += generateFunctions<FunctionType::frff>(std::string_view("braf"), {0, 2, 3});
    functions += generateFunctions<FunctionType::fxxx>(std::string_view("bsr"), {11});
    functions += generateFunctions<FunctionType::frff>(std::string_view("bsrf"), {0, 0, 3});
    functions += generateFunctions<FunctionType::ffxx>(std::string_view("bt"), {8, 9});
    functions += generateFunctions<FunctionType::ffxx>(std::string_view("bts"), {8, 13});
    functions += generateFunctions<FunctionType::ffff>(std::string_view("clrmac"), {0, 0, 2, 8});
    functions += generateFunctions<FunctionType::ffff>(std::string_view("clrt"), {0, 0, 0, 8});

    // cmpeq(Sh2 & s, const u32 n, const u32 m);               // frrf
    // cmpge(Sh2 & s, const u32 n, const u32 m);               // frrf
    // cmpgt(Sh2 & s, const u32 n, const u32 m);               // frrf
    // cmphi(Sh2 & s, const u32 n, const u32 m);               // frrf
    // cmphs(Sh2 & s, const u32 n, const u32 m);               // frrf
    // cmppl(Sh2 & s, const u32 n);                            // frff
    // cmppz(Sh2 & s, const u32 n);                            // frff
    // cmpstr(Sh2 & s, const u32 n, const u32 m);              // frrf
    // cmpim(Sh2 & s, const u32 i);                            // ffxx
    // div0s(Sh2 & s, const u32 n, const u32 m);               // frrf
    // div0u(Sh2 & s);                                         // ffff
    // div1(Sh2 & s, const u32 n, const u32 m);                // frrf
    // dmuls(Sh2 & s, const u32 n, const u32 m);               // frrf
    // dmulu(Sh2 & s, const u32 n, const u32 m);               // frrf
    // dt(Sh2 & s, const u32 n);                               // frff
    // extsb(Sh2 & s, const u32 n, const u32 m);               // frrf
    // extsw(Sh2 & s, const u32 n, const u32 m);               // frrf
    // extub(Sh2 & s, const u32 n, const u32 m);               // frrf
    // extuw(Sh2 & s, const u32 n, const u32 m);               // frrf
    // jmp(Sh2 & s, const u32 m);                              // frff
    // jsr(Sh2 & s, const u32 m);                              // frff
    // ldcsr(Sh2 & s, const u32 m);                            // frff
    // ldcgbr(Sh2 & s, const u32 m);                           // frff
    // ldcvbr(Sh2 & s, const u32 m);                           // frff
    // ldcmsr(Sh2 & s, const u32 m);                           // frff
    // ldcmgbr(Sh2 & s, const u32 m);                          // frff
    // ldcmvbr(Sh2 & s, const u32 m);                          // frff
    // ldsmach(Sh2 & s, const u32 m);                          // frff
    // ldsmacl(Sh2 & s, const u32 m);                          // frff
    // ldspr(Sh2 & s, const u32 m);                            // frff
    // ldsmmach(Sh2 & s, const u32 m);                         // frff
    // ldsmmacl(Sh2 & s, const u32 m);                         // frff
    // ldsmpr(Sh2 & s, const u32 m);                           // frff
    // mac(Sh2 & s, const u32 n, const u32 m);                 // frrf
    // macw(Sh2 & s, const u32 n, const u32 m);                // frrf
    // mov(Sh2 & s, const u32 n, const u32 m);                 // frrf
    // movbs(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movws(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movls(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movbl(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movwl(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movll(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movbm(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movwm(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movlm(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movbp(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movwp(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movlp(Sh2 & s, const u32 n, const u32 m);               // frrf
    // movbs0(Sh2 & s, const u32 n, const u32 m);              // frrf
    // movws0(Sh2 & s, const u32 n, const u32 m);              // frrf
    // movls0(Sh2 & s, const u32 n, const u32 m);              // frrf
    // movbl0(Sh2 & s, const u32 n, const u32 m);              // frrf
    // movwl0(Sh2 & s, const u32 n, const u32 m);              // frrf
    // movll0(Sh2 & s, const u32 n, const u32 m);              // frrf
    // movi(Sh2 & s, const u32 n, const u32 i);                // frxx
    // movwi(Sh2 & s, const u32 n, const u32 d);               // frxx
    // movli(Sh2 & s, const u32 n, const u32 d);               // frxx
    // movblg(Sh2 & s, const u32 d);                           // ffxx
    // movwlg(Sh2 & s, const u32 d);                           // ffxx
    // movllg(Sh2 & s, const u32 d);                           // ffxx
    // movbsg(Sh2 & s, const u32 d);                           // ffxx
    // movwsg(Sh2 & s, const u32 d);                           // ffxx
    // movlsg(Sh2 & s, const u32 d);                           // ffxx
    // movbs4(Sh2 & s, const u32 n, const u32 d);              // ffrx
    // movws4(Sh2 & s, const u32 n, const u32 d);              // ffrx
    // movls4(Sh2 & s, const u32 n, const u32 m, const u32 d); // frrx
    // movbl4(Sh2 & s, const u32 m, const u32 d);              // ffrx
    // movwl4(Sh2 & s, const u32 m, const u32 d);              // ffrx
    // movll4(Sh2 & s, const u32 n, const u32 m, const u32 d); // frrx
    // mova(Sh2 & s, const u32 d);                             // ffxx
    // movt(Sh2 & s, const u32 n);                             // frff
    // mull(Sh2 & s, const u32 n, const u32 m);                // frrf
    // muls(Sh2 & s, const u32 n, const u32 m);                // frrf
    // mulu(Sh2 & s, const u32 n, const u32 m);                // frrf
    // neg(Sh2 & s, const u32 n, const u32 m);                 // frrf
    // negc(Sh2 & s, const u32 n, const u32 m);                // frrf
    // nop(Sh2 & s);                                           // ffff
    // not_op(Sh2 & s, const u32 n, const u32 m);              // frrf
    // or_op(Sh2 & s, const u32 n, const u32 m);               // frrf
    // ori(Sh2 & s, const u32 i);                              // ffxx
    // orm(Sh2 & s, const u32 i);                              // ffxx
    // rotcl(Sh2 & s, const u32 n);                            // frff
    // rotcr(Sh2 & s, const u32 n);                            // frff
    // rotl(Sh2 & s, const u32 n);                             // frff
    // rotr(Sh2 & s, const u32 n);                             // frff
    // rte(Sh2 & s);                                           // ffff
    // rts(Sh2 & s);                                           // ffff
    // sett(Sh2 & s);                                          // ffff
    // shal(Sh2 & s, const u32 n);                             // frff
    // shar(Sh2 & s, const u32 n);                             // frff
    // shll(Sh2 & s, const u32 n);                             // frff
    // shll2(Sh2 & s, const u32 n);                            // frff
    // shll8(Sh2 & s, const u32 n);                            // frff
    // shll16(Sh2 & s, const u32 n);                           // frff
    // shlr(Sh2 & s, const u32 n);                             // frff
    // shlr2(Sh2 & s, const u32 n);                            // frff
    // shlr8(Sh2 & s, const u32 n);                            // frff
    // shlr16(Sh2 & s, const u32 n);                           // frff
    // sleep(Sh2 & s);                                         // ffff
    // stcsr(Sh2 & s, const u32 n);                            // frff
    // stcgbr(Sh2 & s, const u32 n);                           // frff
    // stcvbr(Sh2 & s, const u32 n);                           // frff
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

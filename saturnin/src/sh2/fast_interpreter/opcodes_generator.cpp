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

auto isArgsNumberCorrect(std::string_view func_name, const size_t args_nb, const size_t expected_args_nb)
    -> std::optional<std::string> {
    if (args_nb != expected_args_nb) {
        return std::optional<std::string>(uti::format("Wrong number of arguments for {}", func_name));
    }
    return std::nullopt;
}

auto generateFunctionsWithParams2To3(std::string_view        func_template,
                                     std::string_view        func_name,
                                     GeneratedArray&         arr,
                                     const std::vector<int>& args) {
    // Part 1 & 2 are fixed, 3 & 4 variables.

    if (auto error = isArgsNumberCorrect(func_name, args.size(), 2); error.has_value()) { return *error; }

    constexpr auto    func_nb_max = 0x100;
    constexpr auto    counter_max = 0x10;
    std::array<u8, 2> counters{};

    auto generated = std::string{};
    for (int func_counter = 0; func_counter < func_nb_max; ++func_counter) {
        arr[args[0]][args[1]][counters[0]][counters[1]] = true;
        generated += uti::format(func_template, args[0], args[1], counters[0], counters[1], func_name);
        ++counters[1];
        if (counters[1] == counter_max) {
            counters[1] = 0;
            ++counters[0];
        }
    }
    return generated;
}

auto generateFunctionsWithParams1To3(std::string_view        func_template,
                                     std::string_view        func_name,
                                     GeneratedArray&         arr,
                                     const std::vector<int>& args) {
    // Part 1 is fixed, 2, 3 & 4 are variables.

    if (auto error = isArgsNumberCorrect(func_name, args.size(), 1); error.has_value()) { return *error; }

    constexpr auto    func_nb_max = 0x1000;
    constexpr auto    counter_max = 0x10;
    std::array<u8, 3> counters{};

    auto generated = std::string{};
    for (int func_counter = 0; func_counter < func_nb_max; ++func_counter) {
        arr[args[0]][counters[0]][counters[1]][counters[2]] = true;
        generated += uti::format(func_template, args[0], counters[0], counters[1], counters[2], func_name);
        ++counters[2];
        if (counters[2] == counter_max) {
            counters[2] = 0;
            ++counters[1];
        }
        if (counters[1] == counter_max) {
            counters[1] = 0;
            ++counters[0];
        }
    }
    return generated;
}

template<>
auto generateFunctions<FunctionType::ffff>(std::string_view func_name, GeneratedArray& arr, const std::vector<int>& args)
    -> std::string {
    // Opcode is directly specified.

    if (auto error = isArgsNumberCorrect(func_name, args.size(), 4); error.has_value()) { return *error; }

    auto func_template                      = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    FastInterpreter::{4}(s);
}})";
    arr[args[0]][args[1]][args[2]][args[3]] = true;
    return uti::format(func_template, args[0], args[1], args[2], args[3], func_name);
}

template<>
auto generateFunctions<FunctionType::ffxx>(std::string_view func_name, GeneratedArray& arr, const std::vector<int>& args)
    -> std::string {
    // Part 1 & 2 are fixed, 3 & 4 variables.
    const auto func_template = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    FastInterpreter::{4}(s, 0x{2:x}{3:x});
}})";

    return generateFunctionsWithParams2To3(func_template, func_name, arr, args);
}

template<>
auto generateFunctions<FunctionType::ffrx>(std::string_view func_name, GeneratedArray& arr, const std::vector<int>& args)
    -> std::string {
    // Part 1 & 2 are fixed, 3 & 4 variables.
    auto func_template = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    FastInterpreter::{4}(s, 0x{2:x}, 0x{3:x});
}})";

    return generateFunctionsWithParams2To3(func_template, func_name, arr, args);
}

template<>
auto generateFunctions<FunctionType::fxxx>(std::string_view func_name, GeneratedArray& arr, const std::vector<int>& args)
    -> std::string {
    // Part 1 is fixed, 2, 3 & 4 are variables.

    auto func_template = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    FastInterpreter::{4}(s, 0x{1:x}{2:x}{3:x});
}})";

    return generateFunctionsWithParams1To3(func_template, func_name, arr, args);
}

template<>
auto generateFunctions<FunctionType::frxx>(std::string_view func_name, GeneratedArray& arr, const std::vector<int>& args)
    -> std::string {
    // Part 1 is fixed, 2, 3 & 4 are variables.

    auto func_template = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    FastInterpreter::{4}(s, 0x{1:x}, 0x{2:x}{3:x});
}})";

    return generateFunctionsWithParams1To3(func_template, func_name, arr, args);
}

template<>
auto generateFunctions<FunctionType::frff>(std::string_view func_name, GeneratedArray& arr, const std::vector<int>& args)
    -> std::string {
    // Part 1,3 & 4 are fixed, 2 is variable.

    if (auto error = isArgsNumberCorrect(func_name, args.size(), 3); error.has_value()) { return *error; }

    constexpr auto func_nb_max = 0x10;
    auto           counter     = u8{};

    auto func_template = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    FastInterpreter::{4}(s, 0x{1:x});
}})";

    auto generated = std::string{};
    for (int func_counter = 0; func_counter < func_nb_max; ++func_counter) {
        arr[args[0]][counter][args[1]][args[2]] = true;
        generated += uti::format(func_template, args[0], counter, args[1], args[2], func_name);
        ++counter;
    }

    return generated;
}

template<>
auto generateFunctions<FunctionType::frrx>(std::string_view func_name, GeneratedArray& arr, const std::vector<int>& args)
    -> std::string {
    // Part 1 is fixed, 2, 3 & 4 are variables.

    auto func_template = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    FastInterpreter::{4}(s, 0x{1:x}, 0x{2:x}, 0x{3:x});
}})";

    return generateFunctionsWithParams1To3(func_template, func_name, arr, args);
}

template<>
auto generateFunctions<FunctionType::frrf>(std::string_view func_name, GeneratedArray& arr, const std::vector<int>& args)
    -> std::string {
    // Part 1 & 4 are fixed, 2 & 3 variables.

    if (args.size() != 2) { return uti::format("Wrong number of arguments for {}", func_name); }

    constexpr auto    func_nb_max = 0x100;
    constexpr auto    counter_max = 0x10;
    std::array<u8, 2> counters{};

    auto func_template = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    FastInterpreter::{4}(s, 0x{1:x}, 0x{2:x});
}})";

    auto generated = std::string{};
    for (int func_counter = 0; func_counter < func_nb_max; ++func_counter) {
        arr[args[0]][counters[0]][counters[1]][args[1]] = true;
        generated += uti::format(func_template, args[0], counters[0], counters[1], args[1], func_name);
        ++counters[1];
        if (counters[1] == counter_max) {
            counters[1] = 0;
            ++counters[0];
        }
    }

    return generated;
}

template<FunctionType Type>
auto generateFunctions([[maybe_unused]] std::string_view        func_name,
                       [[maybe_unused]] GeneratedArray&         arr,
                       [[maybe_unused]] const std::vector<int>& args) -> std::string {
    return std::string{};
}

auto generateOpcodes() -> bool {
    // Functions generation.
    GeneratedArray generated{}; // Entries will be true for already generated functions

    using enum FunctionType;
    std::string functions{};
    functions += generateFunctions<frrf>(std::string_view("add"), generated, {3, 12});
    functions += generateFunctions<frxx>(std::string_view("addi"), generated, {7});
    functions += generateFunctions<frrf>(std::string_view("addc"), generated, {3, 14});
    functions += generateFunctions<frrf>(std::string_view("addv"), generated, {3, 15});
    functions += generateFunctions<frrf>(std::string_view("and_op"), generated, {2, 9});
    functions += generateFunctions<ffxx>(std::string_view("andi"), generated, {12, 9});
    functions += generateFunctions<ffxx>(std::string_view("andm"), generated, {12, 13});
    functions += generateFunctions<ffxx>(std::string_view("bf"), generated, {8, 11});
    functions += generateFunctions<ffxx>(std::string_view("bfs"), generated, {8, 15});
    functions += generateFunctions<fxxx>(std::string_view("bra"), generated, {10});
    functions += generateFunctions<frff>(std::string_view("braf"), generated, {0, 2, 3});
    functions += generateFunctions<fxxx>(std::string_view("bsr"), generated, {11});
    functions += generateFunctions<frff>(std::string_view("bsrf"), generated, {0, 0, 3});
    functions += generateFunctions<ffxx>(std::string_view("bt"), generated, {8, 9});
    functions += generateFunctions<ffxx>(std::string_view("bts"), generated, {8, 13});
    functions += generateFunctions<ffff>(std::string_view("clrmac"), generated, {0, 0, 2, 8});
    functions += generateFunctions<ffff>(std::string_view("clrt"), generated, {0, 0, 0, 8});
    functions += generateFunctions<frrf>(std::string_view("cmpeq"), generated, {3, 0});
    functions += generateFunctions<frrf>(std::string_view("cmpge"), generated, {3, 3});
    functions += generateFunctions<frrf>(std::string_view("cmpgt"), generated, {3, 7});
    functions += generateFunctions<frrf>(std::string_view("cmphi"), generated, {3, 6});
    functions += generateFunctions<frrf>(std::string_view("cmphs"), generated, {3, 2});
    functions += generateFunctions<frff>(std::string_view("cmppl"), generated, {4, 1, 5});
    functions += generateFunctions<frff>(std::string_view("cmppz"), generated, {4, 1, 1});
    functions += generateFunctions<frrf>(std::string_view("cmpstr"), generated, {2, 12});
    functions += generateFunctions<ffxx>(std::string_view("cmpim"), generated, {8, 8});
    functions += generateFunctions<frrf>(std::string_view("div0s"), generated, {2, 7});
    functions += generateFunctions<ffff>(std::string_view("div0u"), generated, {0, 0, 1, 9});
    functions += generateFunctions<frrf>(std::string_view("div1"), generated, {3, 4});
    functions += generateFunctions<frrf>(std::string_view("dmuls"), generated, {3, 13});
    functions += generateFunctions<frrf>(std::string_view("dmulu"), generated, {3, 5});
    functions += generateFunctions<frff>(std::string_view("dt"), generated, {4, 1, 0});
    functions += generateFunctions<frrf>(std::string_view("extsb"), generated, {6, 14});
    functions += generateFunctions<frrf>(std::string_view("extsw"), generated, {6, 15});
    functions += generateFunctions<frrf>(std::string_view("extub"), generated, {6, 12});
    functions += generateFunctions<frrf>(std::string_view("extuw"), generated, {6, 13});
    functions += generateFunctions<frff>(std::string_view("jmp"), generated, {4, 2, 11});
    functions += generateFunctions<frff>(std::string_view("jsr"), generated, {4, 0, 11});
    functions += generateFunctions<frff>(std::string_view("ldcsr"), generated, {4, 0, 14});
    functions += generateFunctions<frff>(std::string_view("ldcgbr"), generated, {4, 1, 14});
    functions += generateFunctions<frff>(std::string_view("ldcvbr"), generated, {4, 2, 14});
    functions += generateFunctions<frff>(std::string_view("ldcmsr"), generated, {4, 0, 7});
    functions += generateFunctions<frff>(std::string_view("ldcmgbr"), generated, {4, 1, 7});
    functions += generateFunctions<frff>(std::string_view("ldcmvbr"), generated, {4, 2, 7});
    functions += generateFunctions<frff>(std::string_view("ldsmach"), generated, {4, 0, 10});
    functions += generateFunctions<frff>(std::string_view("ldsmacl"), generated, {4, 1, 10});
    functions += generateFunctions<frff>(std::string_view("ldspr"), generated, {4, 2, 10});
    functions += generateFunctions<frff>(std::string_view("ldsmmach"), generated, {4, 0, 6});
    functions += generateFunctions<frff>(std::string_view("ldsmmacl"), generated, {4, 1, 6});
    functions += generateFunctions<frff>(std::string_view("ldsmpr"), generated, {4, 2, 6});
    functions += generateFunctions<frrf>(std::string_view("mac"), generated, {0, 15});
    functions += generateFunctions<frrf>(std::string_view("macw"), generated, {4, 15});
    functions += generateFunctions<frrf>(std::string_view("mov"), generated, {6, 3});
    functions += generateFunctions<frrf>(std::string_view("movbs"), generated, {2, 0});
    functions += generateFunctions<frrf>(std::string_view("movws"), generated, {2, 1});
    functions += generateFunctions<frrf>(std::string_view("movls"), generated, {2, 2});
    functions += generateFunctions<frrf>(std::string_view("movbl"), generated, {6, 0});
    functions += generateFunctions<frrf>(std::string_view("movwl"), generated, {6, 1});
    functions += generateFunctions<frrf>(std::string_view("movll"), generated, {6, 2});
    functions += generateFunctions<frrf>(std::string_view("movbm"), generated, {2, 4});
    functions += generateFunctions<frrf>(std::string_view("movwm"), generated, {2, 5});
    functions += generateFunctions<frrf>(std::string_view("movlm"), generated, {2, 6});
    functions += generateFunctions<frrf>(std::string_view("movbp"), generated, {6, 4});
    functions += generateFunctions<frrf>(std::string_view("movwp"), generated, {6, 5});
    functions += generateFunctions<frrf>(std::string_view("movlp"), generated, {6, 6});
    functions += generateFunctions<frrf>(std::string_view("movbs0"), generated, {0, 4});
    functions += generateFunctions<frrf>(std::string_view("movws0"), generated, {0, 5});
    functions += generateFunctions<frrf>(std::string_view("movls0"), generated, {0, 6});
    functions += generateFunctions<frrf>(std::string_view("movbl0"), generated, {0, 12});
    functions += generateFunctions<frrf>(std::string_view("movwl0"), generated, {0, 13});
    functions += generateFunctions<frrf>(std::string_view("movll0"), generated, {0, 14});
    functions += generateFunctions<frxx>(std::string_view("movi"), generated, {14});
    functions += generateFunctions<frxx>(std::string_view("movwi"), generated, {9});
    functions += generateFunctions<frxx>(std::string_view("movli"), generated, {13});
    functions += generateFunctions<ffxx>(std::string_view("movblg"), generated, {12, 4});
    functions += generateFunctions<ffxx>(std::string_view("movwlg"), generated, {12, 5});
    functions += generateFunctions<ffxx>(std::string_view("movllg"), generated, {12, 6});
    functions += generateFunctions<ffxx>(std::string_view("movbsg"), generated, {12, 0});
    functions += generateFunctions<ffxx>(std::string_view("movwsg"), generated, {12, 1});
    functions += generateFunctions<ffxx>(std::string_view("movlsg"), generated, {12, 2});
    functions += generateFunctions<ffrx>(std::string_view("movbs4"), generated, {8, 0});
    functions += generateFunctions<ffrx>(std::string_view("movws4"), generated, {8, 1});
    functions += generateFunctions<frrx>(std::string_view("movls4"), generated, {1});
    functions += generateFunctions<ffrx>(std::string_view("movbl4"), generated, {8, 4});
    functions += generateFunctions<ffrx>(std::string_view("movwl4"), generated, {8, 5});
    functions += generateFunctions<frrx>(std::string_view("movll4"), generated, {5});
    functions += generateFunctions<ffxx>(std::string_view("mova"), generated, {12, 7});
    functions += generateFunctions<frff>(std::string_view("movt"), generated, {0, 2, 9});
    functions += generateFunctions<frrf>(std::string_view("mull"), generated, {0, 7});
    functions += generateFunctions<frrf>(std::string_view("muls"), generated, {2, 15});
    functions += generateFunctions<frrf>(std::string_view("mulu"), generated, {2, 14});
    functions += generateFunctions<frrf>(std::string_view("neg"), generated, {6, 11});
    functions += generateFunctions<frrf>(std::string_view("negc"), generated, {6, 10});
    functions += generateFunctions<ffff>(std::string_view("nop"), generated, {0, 0, 0, 9});
    functions += generateFunctions<frrf>(std::string_view("not_op"), generated, {6, 7});
    functions += generateFunctions<frrf>(std::string_view("or_op"), generated, {2, 11});
    functions += generateFunctions<ffxx>(std::string_view("ori"), generated, {12, 11});
    functions += generateFunctions<ffxx>(std::string_view("orm"), generated, {12, 15});
    functions += generateFunctions<frff>(std::string_view("rotcl"), generated, {4, 2, 4});
    functions += generateFunctions<frff>(std::string_view("rotcr"), generated, {4, 2, 5});
    functions += generateFunctions<frff>(std::string_view("rotl"), generated, {4, 0, 4});
    functions += generateFunctions<frff>(std::string_view("rotr"), generated, {4, 0, 5});
    functions += generateFunctions<ffff>(std::string_view("rte"), generated, {0, 0, 2, 11});
    functions += generateFunctions<ffff>(std::string_view("rts"), generated, {0, 0, 0, 11});
    functions += generateFunctions<ffff>(std::string_view("sett"), generated, {0, 0, 1, 8});
    functions += generateFunctions<frff>(std::string_view("shal"), generated, {4, 2, 0});
    functions += generateFunctions<frff>(std::string_view("shar"), generated, {4, 2, 1});
    functions += generateFunctions<frff>(std::string_view("shll"), generated, {4, 0, 0});
    functions += generateFunctions<frff>(std::string_view("shll2"), generated, {4, 0, 8});
    functions += generateFunctions<frff>(std::string_view("shll8"), generated, {4, 1, 8});
    functions += generateFunctions<frff>(std::string_view("shll16"), generated, {4, 2, 8});
    functions += generateFunctions<frff>(std::string_view("shlr"), generated, {4, 0, 1});
    functions += generateFunctions<frff>(std::string_view("shlr2"), generated, {4, 0, 9});
    functions += generateFunctions<frff>(std::string_view("shlr8"), generated, {4, 1, 9});
    functions += generateFunctions<frff>(std::string_view("shlr16"), generated, {4, 2, 9});
    functions += generateFunctions<ffff>(std::string_view("sleep"), generated, {0, 0, 1, 11});
    functions += generateFunctions<frff>(std::string_view("stcsr"), generated, {0, 0, 2});
    functions += generateFunctions<frff>(std::string_view("stcgbr"), generated, {0, 1, 2});
    functions += generateFunctions<frff>(std::string_view("stcvbr"), generated, {0, 2, 2});
    functions += generateFunctions<frff>(std::string_view("stcmsr"), generated, {4, 0, 3});
    functions += generateFunctions<frff>(std::string_view("stcmgbr"), generated, {4, 1, 3});
    functions += generateFunctions<frff>(std::string_view("stcmvbr"), generated, {4, 2, 3});
    functions += generateFunctions<frff>(std::string_view("stsmach"), generated, {0, 0, 10});
    functions += generateFunctions<frff>(std::string_view("stsmacl"), generated, {0, 1, 10});
    functions += generateFunctions<frff>(std::string_view("stspr"), generated, {0, 2, 10});
    functions += generateFunctions<frff>(std::string_view("stsmmach"), generated, {4, 0, 2});
    functions += generateFunctions<frff>(std::string_view("stsmmacl"), generated, {4, 1, 2});
    functions += generateFunctions<frff>(std::string_view("stsmpr"), generated, {4, 2, 2});
    functions += generateFunctions<frrf>(std::string_view("sub"), generated, {3, 8});
    functions += generateFunctions<frrf>(std::string_view("subc"), generated, {3, 10});
    functions += generateFunctions<frrf>(std::string_view("subv"), generated, {3, 11});
    functions += generateFunctions<frrf>(std::string_view("swapb"), generated, {6, 8});
    functions += generateFunctions<frrf>(std::string_view("swapw"), generated, {6, 9});
    functions += generateFunctions<frff>(std::string_view("tas"), generated, {4, 1, 11});
    functions += generateFunctions<ffxx>(std::string_view("trapa"), generated, {12, 3});
    functions += generateFunctions<frrf>(std::string_view("tst"), generated, {2, 8});
    functions += generateFunctions<ffxx>(std::string_view("tsti"), generated, {12, 8});
    functions += generateFunctions<ffxx>(std::string_view("tstm"), generated, {12, 12});
    functions += generateFunctions<frrf>(std::string_view("xor_op"), generated, {2, 10});
    functions += generateFunctions<ffxx>(std::string_view("xori"), generated, {12, 10});
    functions += generateFunctions<ffxx>(std::string_view("xorm"), generated, {12, 14});
    functions += generateFunctions<frrf>(std::string_view("xtrct"), generated, {2, 13});

    functions += generateBadOpcodes(generated);

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

    std::string content{uti::format(R"(
namespace saturnin::sh2::fast_interpreter{{
{0}
std::array<OpcodeFunc, 0x10000> opcodes_func{{ {1} }};
}})",
                                    functions,
                                    calls)};

    // Write to the file.
    const auto path = std::string{"./sh2_opcodes.inc"};
    auto       file = std::ofstream(path, std::ios::binary);
    file << content;
    file.close();

    return true;
}

auto generateBadOpcodes(GeneratedArray& arr) -> std::string {
    auto generated = std::string{};

    constexpr auto    call_max    = 0x10000;
    constexpr auto    counter_max = 0x10;
    std::string       calls{};
    std::array<u8, 4> counters{};
    auto              func_template = R"(
void call_{0:x}_{1:x}_{2:x}_{3:x}(Sh2& s){{
    FastInterpreter::badOpcode(s);
}})";
    for (int call_counter = 0; call_counter < call_max; ++call_counter) {
        if (!arr[counters[0]][counters[1]][counters[2]][counters[3]]) {
            generated += uti::format(func_template, counters[0], counters[1], counters[2], counters[3]);
        }
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

    return generated;
}

}; // namespace saturnin::sh2

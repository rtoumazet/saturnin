//
// tests.cpp
// Saturnin
//
// Copyright (c) 2018 Renaud Toumazet
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
#include <saturnin/src/tests.h>

#include <chrono>
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/bit_register.h>
#include <saturnin/src/utilities.h>
#include <saturnin/src/video/vdp2.h>

namespace saturnin::tests {

void Test::startTest() { start_time_ = std::chrono::steady_clock::now(); }

auto Test::endTest() -> std::string {
    elapsed_time_ = std::chrono::steady_clock::now() - start_time_;
    auto res      = (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time_)).count();

    using namespace std::literals;
    return utilities::format("{} µs", res);
}

void runTests() {
    if constexpr (constexpr auto run_bitfields_benchmarks = false) {
        using namespace saturnin::video;
        using namespace std::string_literals;

        auto bitfield_original = Test();

        bitfield_original.startTest();

        auto           test = std::vector<u32>{};
        constexpr auto iterations{1000000};
        constexpr auto tvmd = TvScreenMode{0b1000000000000011};
        auto           val  = u32{};

        for (int i = 0; i < iterations; ++i) {
            if (toEnum<HorizontalResolution>(tvmd.horizontal_resolution) == HorizontalResolution::hi_res_704) { ++val; }
            test.push_back(val);
        }

        auto result = "Original bitfield "s;
        result += bitfield_original.endTest();
        core::Log::info(Logger::test, result);
    }

    if constexpr (constexpr auto run_registers_benchmarks = true) {
        using namespace saturnin::video;
        using namespace std::string_literals;

        auto reg = Test();

        reg.startTest();

        auto               test = std::vector<u32>{};
        constexpr auto     iterations{1000000};
        Dots16BitsRegister reg32{};
        reg32.set(Dots16BitsRegister::raw, 0x80000000);
        auto val = u32{};

        for (int i = 0; i < iterations; ++i) {
            val += reg32.get(Dots16BitsRegister::dot_0);
            val += reg32.get(Dots16BitsRegister::dot_1);
            test.push_back(val);
        }

        auto result = "Register "s;
        result += reg.endTest();
        core::Log::info(Logger::test, result);

        // 2
        reg.startTest();

        std::vector<u32>().swap(test);
        auto row = Dots16Bits{0x80000000};
        val      = 0;
        for (int i = 0; i < iterations; ++i) {
            val += row.dot_0;
            val += row.dot_1;
            test.push_back(val);
        }

        result = "BitField "s;
        result += reg.endTest();
        core::Log::info(Logger::test, result);

        // 3
        reg.startTest();

        std::vector<u32>().swap(test);
        auto row_slice = std::bitset<32>{0x80004000};
        val            = 0;
        for (int i = 0; i < iterations; ++i) {
            val += slice<32, 31, 16>(row_slice).to_ulong();
            val += slice<32, 15, 0>(row_slice).to_ulong();
            test.push_back(val);
        }

        result = "Slice "s;
        result += reg.endTest();
        core::Log::info(Logger::test, result);

        // 4
        reg.startTest();

        std::vector<u32>().swap(test);
        auto row_slice2 = std::bitset<32>{0x80004000};
        val             = 0;
        for (int i = 0; i < iterations; ++i) {
            val += make_slice<16, 16>(row_slice2).operator std::bitset<16Ui64>().to_ulong();
            val += make_slice<0, 16>(row_slice2). operator std::bitset<16Ui64>().to_ulong();
            test.push_back(val);
        }

        result = "Slice2 "s;
        result += reg.endTest();
        core::Log::info(Logger::test, result);

        // 5
        reg.startTest();

        std::vector<u32>().swap(test);
        Dots16BitsRegbit regbit;
        regbit.data = 0x12345678;

        val = 0;
        for (int i = 0; i < iterations; ++i) {
            val += regbit.data >> Dots16BitsRegbit::Data::upper16_shft;
            val += regbit.data >> Dots16BitsRegbit::Data::lower16_shft;
            test.push_back(val);
        }

        result = "Regbit "s;
        result += reg.endTest();
        core::Log::info(Logger::test, result);
    }
}

} // namespace saturnin::tests

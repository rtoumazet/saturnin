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
    constexpr auto run_bitfields_benchmarks = true;
    if constexpr (run_bitfields_benchmarks) {
        using namespace saturnin::tests;
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
}

} // namespace saturnin::tests

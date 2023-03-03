//
// tests.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	tests.h
///
/// \brief	Declares functions needed to test various aspects of the emulator.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace saturnin::tests {

class Test {
  public:
    void startTest();

    auto endTest() -> std::string;

  private:
    std::chrono::time_point<std::chrono::steady_clock> start_time_{};
    std::chrono::duration<double>                      elapsed_time_{};
};

void runTests();

} // namespace saturnin::tests

//
// timer.h
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
#include <saturnin/src/timer.h>

namespace saturnin::core {

void Timer::start() { start_time_ = std::chrono::steady_clock::now(); }

void Timer::stop() { elapsed_time_ = std::chrono::steady_clock::now() - start_time_; }

[[nodiscard]] auto Timer::micros() const -> std::chrono::microseconds::rep {
    return (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time_)).count();
}

[[nodiscard]] auto Timer::ms() const -> std::chrono::milliseconds::rep {
    return (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time_)).count();
}
} // namespace saturnin::core

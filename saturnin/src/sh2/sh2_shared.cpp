//
// sh2_shared.cpp
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
#include <saturnin/src/sh2/sh2_shared.h>
#include <saturnin/src/sh2/sh2.h>

namespace saturnin::sh2 {
// void badOpcode(Sh2& s) {
//     const auto type = std::string{(s.sh2_type_ == Sh2Type::master) ? "Master" : "Slave"};
//     Log::error(Logger::sh2, "Unexpected opcode({} SH2). Opcode = {:#06x}. PC = {:#010x}", type, s.current_opcode_, s.pc_);
//
//     s.modules_.context()->debugStatus(core::DebugStatus::paused);
// }
} // namespace saturnin::sh2

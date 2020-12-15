//
// utilities.cpp
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

#include <saturnin/src/utilities.h>
#include <windows.h>
#include <sstream> // istringstream

namespace saturnin::utilities {

auto stringToVector(const std::string& source, const u32 reserved_size) -> std::vector<char> {
    auto v = std::vector<char>(source.c_str(), source.c_str() + source.size() + 1u);
    v.reserve(reserved_size);
    return v;
}

auto getLastErrorMessage() -> std::string {
    const auto error = GetLastError();
    if (error != 0) {
        auto       buffer = LPVOID{};
        const auto buf_len
            = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                            nullptr,
                            error,
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            (LPTSTR)&buffer,
                            0,
                            nullptr);
        if (buf_len != 0) {
            const auto str    = static_cast<LPCSTR>(buffer);
            const auto result = std::string(str, str + buf_len);

            LocalFree(buffer);

            return result;
        }
    }
    return std::string();
}

auto explode(std::string const& s, char delim) -> std::vector<std::string> {
    auto result = std::vector<std::string>{};
    auto iss    = std::istringstream(s);

    for (std::string token; std::getline(iss, token, delim);) {
        result.push_back(std::move(token));
    }

    return result;
}

auto dec2bcd(u16 dec) -> u32 {
    constexpr auto decimal_base = u8{10};
    auto           result       = u32{};
    auto           shift        = s32{};

    while (dec != 0) {
        result += (dec % decimal_base) << shift;
        dec = dec / decimal_base;
        shift += 4;
    }
    return result;
}

} // namespace saturnin::utilities

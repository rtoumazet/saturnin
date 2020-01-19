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

#include <windows.h>
#include <sstream> // istringstream
#include "utilities.h"


namespace saturnin {
namespace utilities {

std::vector<char> stringToVector(const std::string& source, const uint32_t reserved_size) {
    std::vector<char> v(source.c_str(), source.c_str() + source.size() + 1u);
    v.reserve(reserved_size);
    return v;
}

std::string getLastErrorMessage() {
    DWORD error = GetLastError();
    if (error) {
        LPVOID buffer;
        DWORD buf_len = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&buffer,
            0, NULL);
        if (buf_len) {
            LPCSTR str = (LPCSTR)buffer;
            std::string result(str, str + buf_len);

            LocalFree(buffer);

            return result;
        }
    }
    return std::string();
}

std::vector<std::string> explode(std::string const & s, char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); )
    {
        result.push_back(std::move(token));
    }

    return result;
}

uint32_t dec2bcd(uint16_t dec) {
    uint32_t result = 0;
    int shift = 0;

    while (dec) {
        result += (dec % 10) << shift;
        dec = dec / 10;
        shift += 4;
    }
    return result;
}

}
}
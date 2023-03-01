//
// exceptions.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	exceptions.h
///
/// \brief	Declares the various exceptions used in Saturnin.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdexcept>
#include <string>

namespace saturnin::exception {

class CdromError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class ConfigError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class GenericError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class GuiError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class MainError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class MemoryError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class Sh2Error : public std::runtime_error {
    using runtime_error::runtime_error;
};

class ScuError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class Vdp1Error : public std::runtime_error {
    using runtime_error::runtime_error;
};

class Vdp2Error : public std::runtime_error {
    using runtime_error::runtime_error;
};

class OpenglError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class SmpcError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class ScspError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class TextureError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class UnimplementedError : public std::runtime_error {
    using runtime_error::runtime_error;
};

class LogError : public std::runtime_error {
    using runtime_error::runtime_error;
};

} // namespace saturnin::exception

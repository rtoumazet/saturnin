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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	opcodes_generator.h
///
/// \brief	Functions to generate a file containing generated SH2 opcodes.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace saturnin::sh2 {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	auto generateOpcodes() -> bool;
///
/// \brief	Generates the SH2 opcodes to a file in order to be added to the compilation at a later
/// 		time.
///
/// \author	Runik
/// \date	24/04/2023
///
/// \returns	True if the generation was successful.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto generateOpcodes() -> bool;
}; // namespace saturnin::sh2

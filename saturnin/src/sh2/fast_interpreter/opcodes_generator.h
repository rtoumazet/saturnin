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

#include <string_view>
#include <initializer_list>

namespace saturnin::sh2 {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	FunctionType
///
/// \brief	Values that represent function types.
/// 		F: fixed data.
/// 		R: M or N register.
/// 		X: displacement or immediate data.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class FunctionType { ffff, ffxx, ffrx, fxxx, frxx, frff, frrx, frrf };

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	template<FunctionType Type> auto generateFunctions(std::string_view func_name, const std::initializer_list<int>& args) ->
/// std::string;
///
/// \brief	Generates all the possible calls for the function of the specified type using template specialization.
///
/// \tparam	Type	    Type of the function to generate.
/// \param 	func_name	Name of the function to generate.
/// \param 	args	 	Function arguments.
///
/// \returns	A string containing calls to every possible parameters of type FunctionType.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<FunctionType Type>
auto generateFunctions(std::string_view func_name, const std::vector<int>& args) -> std::string;

}; // namespace saturnin::sh2

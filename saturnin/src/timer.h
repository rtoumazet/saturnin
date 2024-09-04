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

///////////////////////////////////////////////////////////////////////
/// \file	timer.h
///
/// \brief	Declares the timer class used to measure execution time.
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include <chrono>

namespace saturnin::core {

class Timer {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Timer::start();
    ///
    /// \brief	Starts the internal counter.
    ///
    /// \author	Runik
    /// \date	24/08/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void start();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Timer::stop();
    ///
    /// \brief	Stops the internal counter and save the elapsed time.
    ///
    /// \author	Runik
    /// \date	24/08/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void stop();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Timer::micros() const -> std::chrono::microseconds::rep;
    ///
    /// \brief	Returns elapsed time in micros.
    ///
    /// \author	Runik
    /// \date	24/08/2023
    ///
    /// \returns	A std::chrono::microseconds::rep.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto micros() const -> std::chrono::microseconds::rep;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Timer::ms() const -> std::chrono::milliseconds::rep;
    ///
    /// \brief	Returns elapsed time in ms.
    ///
    /// \author	Runik
    /// \date	24/08/2023
    ///
    /// \returns	A std::chrono::milliseconds::rep.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto ms() const -> std::chrono::milliseconds::rep;

  private:
    std::chrono::time_point<std::chrono::steady_clock> start_time_{};
    std::chrono::duration<double>                      elapsed_time_{};
};
} // namespace saturnin::core

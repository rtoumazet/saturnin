//
// thread_pool.h
// Saturnin
//
// Copyright (c) 2022 Renaud Toumazet
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
/// \file	thrad_pool.h
///
/// \brief	Declares the static ThreadPool class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <BS_thread_pool.hpp> // thread_pool

namespace saturnin::core {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ThreadPool
///
/// \brief  Handles a pool of general purpose threads in the emulator. BS_thread_pool wrapper.
///
/// \author Runik
/// \date   29/06/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

class ThreadPool {
  public:
    template<typename T>
    using multi_future = BS::multi_future<T>;

    //@{
    // Constructors / Destructors
    ThreadPool()                                       = delete;
    ThreadPool(const ThreadPool&)                      = delete;
    ThreadPool(ThreadPool&&)                           = delete;
    auto operator=(const ThreadPool&) & -> ThreadPool& = delete;
    auto operator=(ThreadPool&&) & -> ThreadPool&      = delete;
    ~ThreadPool()                                      = delete;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto ThreadPool::initialize() -> bool;
    ///
    /// \brief  Initializes a pool of thread for future usage in Saturnin.
    ///
    /// \author Runik
    /// \date   29/06/2022
    ///
    /// \return True if initialization is successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto initialize() -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void ThreadPool::shutdown();
    ///
    /// \brief  Shuts down this and frees any resources it is using
    ///
    /// \author Runik
    /// \date   29/06/2022
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void shutdown();

    static BS::thread_pool pool_; ///< Pool of threads
};
}; // namespace saturnin::core

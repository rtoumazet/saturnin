//
// thread_pool.cpp
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
//

#include <saturnin/src/pch.h>
#include <saturnin/src/thread_pool.h>

namespace saturnin::core {

BS::thread_pool ThreadPool::pool_;

// static //
auto ThreadPool::initialize() -> bool {
    const auto available_threads = std::thread::hardware_concurrency() - 2;
    pool_.reset(available_threads);
    return true;
}

// static //
void ThreadPool::shutdown() {}

} // namespace saturnin::core

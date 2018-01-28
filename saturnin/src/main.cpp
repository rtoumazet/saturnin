// 
// main.cpp
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
//

#include <iostream> // cout
//#include <spdlog/sinks/file_sinks.h>
#include "locale.h"
#include "config.h"
#include "video/opengl.h"
#include "video/opengl_legacy.h"
#include "video/opengl_modern.h"


using namespace std;
using namespace saturnin::video;
using namespace saturnin::core;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int argc, char *argv[])
{
    if ( !Locale::initialize() ) return 1;
    cout << saturnin::core::tr("Hello World") << endl;
    
    Config cfg;
    if ( !cfg.initialize(OpenGl::isModernOpenGlCapable()) ) return 1;

    bool isLegacyOpenGl = cfg.lookup("rendering.legacy_opengl");

    if (isLegacyOpenGl) return runLegacyOpenGl(); else return runModernOpenGl();
}
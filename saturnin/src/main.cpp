// 
// main.cpp
// Saturnin
//
// Copyright (c) 2003-2017 Renaud Toumazet
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

#include <stdio.h>
#include "video/opengl_legacy.h"
#include "video/opengl_modern.h"

#include <boost/filesystem.hpp>
#include <boost/system/config.hpp>
#include <boost/locale.hpp>

using namespace std;
//using namespace boost::filesystem;
using namespace boost::locale;
using namespace saturnin::video;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int argc, char *argv[])
{
    generator gen;
    // Specify location of dictionaries
    gen.add_messages_path(".");
    gen.add_messages_domain("saturnin");
    // Generate locales and imbue them to iostream
    locale::global(gen(""));
    cout.imbue(locale());
    // Display a message using current system locale
    cout << translate("Hello World") << endl;




    bool isLegacy = false;
    if (isLegacy) {
        return runLegacyOpenGl();
    }
    else {
        return runModernOpenGl();
    }
}
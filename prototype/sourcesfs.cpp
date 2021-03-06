// MIT License
//
// Copyright (c) 2017 Joerg Hallmann
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// https://github.com/joerghall/sourcesfs
//
#include <cstring>

#include "sourcesfs.h"
#include "prototype.hpp"

#include <tclap/CmdLine.h>
#include <iostream>
#include <fstream>
#include <json/json.hpp>
#include <map>
#include <FuseHandler.hpp>
#include <FuseUtil.hpp>
#include <sys/mount.h>

using json = nlohmann::json;
namespace fs = boost::filesystem;
using namespace std;
#include <string>
#include <regex>

namespace
{

#if 0
    template<class A>
    struct has_correct_replace_function
    {
        template<typename T, T> struct TypeCheck;

        template<typename T> struct replace
        {
            typedef void(T::*fptr)(string::const_iterator i1, string::const_iterator i2, const std::string &);
        };

        template<typename T> static true_type test(TypeCheck<typename replace<T>::fptr, &T::replace>*);
        template<typename T> static false_type test(...);
    };
#endif

    template<typename T>
    void replace(T& result, typename T::const_iterator first, typename T::const_iterator second, const enable_if_t<false, T>& var)
    {
        result.replace(first, second, var);
    }

    template<typename T>
    void replace(T& result, typename T::const_iterator first, typename T::const_iterator second, const enable_if_t<true, T>& var)
    {
        const size_t firstOffset = first - result.begin();
        const size_t secondOffset = second - result.begin();
        result.replace(result.begin() + firstOffset, result.begin() + secondOffset, var);
    }

    // Update the input string.
    string autoExpandEnvironmentVariables(const string& text)
    {
        static regex env( "\\$\\{([^}]+)\\}" );
        smatch match;
        string result(text);
        while (regex_search(result, match, env))
        {
            const char* s = getenv(match[1].str().c_str());
            const string var(s == nullptr ? "" : s);
            replace(result, match[0].first, match[0].second, var);
        }

        return result;
    }
}

json readDefaultConfig(const fs::path& configFileName)
{
    fstream configFile(configFileName.c_str(), ios_base::in);
    json j;
    configFile >> j;
    std::cout << std::setw(4) << j << std::endl;
    return j;
}

int main(int argc, char** argv)
{

    TCLAP::CmdLine cmd("SourcesFS - help to debug since 2017", ' ', SOURCE_VERSION);

    TCLAP::ValueArg<std::string> argMountPoint("m", "mountpoint", "Mount point - default \"${HOME}/sourcesfs\"", false, "${HOME}/sourcesfs", "path", cmd);
    TCLAP::ValueArg<std::string> argWorkDir("w", "workdir", "Workdir or empty - default \"\"", false, "", "path", cmd);
    TCLAP::ValueArg<std::string> argConfigFile("c", "config", "Json config file - default \"${HOME}/.sourcesfs\"", false, "${HOME}/.sourcesfs", "path", cmd);
    cmd.parse(argc, argv);

    const fs::path configFileName = autoExpandEnvironmentVariables(argConfigFile.getValue());
    if (!fs::exists(configFileName))
    {
        throw runtime_error(string("No SourcesFS configuration file found at ") + configFileName.string());
    }

    // TODO: Derive this from configuration file!
    const fs::path defaultFallbackPath = autoExpandEnvironmentVariables("${HOME}/test");
    if (!fs::exists(defaultFallbackPath))
    {
        throw runtime_error(string("No default fallback path found at ") + defaultFallbackPath.string());
    }

    json config = readDefaultConfig(configFileName);
    Prototype prototype(defaultFallbackPath, config);

    const string mountPoint = autoExpandEnvironmentVariables(argMountPoint.getValue().c_str());

#ifdef CONFIG_LINUX
    umount2(mountPoint.c_str(), MNT_FORCE);
#elif CONFIG_OSX
    unmount(mountPoint.c_str(), MNT_FORCE);
#endif

    std::vector<const char*> vecArgs;
    vecArgs.push_back("fuse");
    vecArgs.push_back("-f"); // Foreground
    vecArgs.push_back("-d"); // Debug
    vecArgs.push_back("-s"); // Sync
    vecArgs.push_back(mountPoint.c_str());
    return runFuse(vecArgs.size(), const_cast<char**>(vecArgs.data()), prototype);

}

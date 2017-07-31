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
#include "FuseHandler.hpp"
#include "FuseUtil.hpp"
#include "GitProvider.hpp"
#include "P4Provider.hpp"
#include "CacheProvider.hpp"
#include "ProviderConfig.hpp"
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <json/json.hpp>
#include <map>

using json = nlohmann::json;
namespace fs = boost::filesystem;
using namespace std;
#include <string>
#include <regex>

namespace
{
    vector<path_element> splitAndNativizePath(const fs::path& path)
    {
        vector<path_element> pathElements;
        for (const auto& p : path)
        {
            pathElements.push_back(p.native());
        }

        return pathElements;
    }

    fs::path joinPaths(const vector<path_element>& pathElements)
    {
        fs::path path;
        for (const auto& pathElement : pathElements)
        {
            path /= pathElement;
        }

        return path;
    }
}

// Update the input string.
std::string autoExpandEnvironmentVariables(const std::string& text)
{
    static std::regex env( "\\$\\{([^}]+)\\}" );
    std::smatch match;
    std::string result(text);
    while (std::regex_search(result, match, env))
    {
        const char * s = getenv( match[1].str().c_str() );
        const std::string var( s == NULL ? "" : s );
        result.replace( match[0].first, match[0].second, var );
    }

    return result;
}

class Prototype final : public FuseHandler
{
public:
    Prototype(const Prototype&) = delete;
    Prototype& operator=(const Prototype&) = delete;

public:
    Prototype(const fs::path& defaultFallbackPath, const json &config);
    ~Prototype();

    int getattr(const char* path, struct stat* stbuf) override;
    int open(const char* path, struct fuse_file_info* fi) override;
    int read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) override;
    int release(const char* path, struct fuse_file_info* fi) override;
    int readdir(const char* path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) override;
    int releasedir(const char* path, struct fuse_file_info* fi) override;

private:
    static std::map<std::string, ProviderConfig> makeProviderConfigs(const json& configs);
public:
    fs::path FusePathToRealPath(const char* path);

private:
    const fs::path _defaultFallbackPath;
    const std::map<std::string, ProviderConfig> _providerConfigs;
    std::map<std::string, shared_ptr<Provider>> _providers;
};

Prototype::Prototype(const fs::path& defaultFallbackPath, const json& configs)
    : _defaultFallbackPath(defaultFallbackPath)
    , _providerConfigs(makeProviderConfigs(configs))
{
}

Prototype::~Prototype()
{
    cout <<  "Prototype::~Prototype" << endl;
}

map<string, ProviderConfig> Prototype::makeProviderConfigs(const json& configs)
{
    map<string, ProviderConfig> providerConfigs;
    for (const auto& config : configs.get<json::object_t>())
    {
        const auto& name = config.first;
        const auto& type = config.second.at("type");
        const auto& urlTemplate = config.second.at("url");
        const auto& args = config.second.at("args");
        providerConfigs.insert(pair<string, ProviderConfig>(name, { name, type, urlTemplate, args }));
    }

    return providerConfigs;
}

namespace
{
    shared_ptr<Provider> makeProvider(const ProviderConfig& providerConfig, const RepoPathInfo& pathInfo)
    {
        if (providerConfig.type() == "git")
        {
            return make_shared<GitProvider>(pathInfo.url, pathInfo.revision, fs::temp_directory_path());
        }
        else if (providerConfig.type() == "p4")
        {
            return make_shared<P4Provider>(pathInfo.url, pathInfo.revision, fs::temp_directory_path());
        }
        else if (providerConfig.type() == "cache")
        {
            return make_shared<CacheProvider>(pathInfo.url);
        }
        else
        {
            return nullptr;
        }
    }
}

fs::path Prototype::FusePathToRealPath(const char* path)
{
    const auto pathElements = splitAndNativizePath(path);
    if (pathElements.size())
    {
        // Eliminate metadata files translations
        // https://apple.stackexchange.com/questions/14980/why-are-dot-underscore-files-created-and-how-can-i-avoid-them
        if (pathElements.back().find("._") == 0)
        {
            return "";
        }
    }

    if (pathElements.size() < 2)
    {
        return _defaultFallbackPath;
    }

    const auto providerConfigIter = _providerConfigs.find(pathElements[1]);
    if (providerConfigIter == _providerConfigs.cend())
    {
       return "";
    }

    shared_ptr<Provider> provider;
    const auto& providerConfig = providerConfigIter->second;
    const auto pathInfo = providerConfig.resolvePath(pathElements);
    const auto providerIter = _providers.find(pathInfo.key);
    if (providerIter == _providers.end())
    {
        provider = makeProvider(providerConfig, pathInfo);
        _providers.insert(make_pair(pathInfo.key, provider));
    }
    else
    {
        provider = providerIter->second;
    }

    if (provider)
    {
        const auto relPath = joinPaths(pathInfo.pathElements);
        const auto absPath = provider->retrieve(relPath);
        return absPath.c_str();
    }
    else
    {
        return _defaultFallbackPath;
    }
}

int Prototype::getattr(const char* path, struct stat* stbuf)
{
    const auto fusedPath = FusePathToRealPath(path);
    if(fusedPath=="")
    {
        return -ENOENT;
    }

    cout <<  "Prototype::getattr path=" << fusedPath << endl;
    return ::stat(fusedPath.c_str(), stbuf);
}

int Prototype::open(const char* path, struct fuse_file_info* fi)
{
    const auto fusedPath = FusePathToRealPath(path);
    if(fusedPath=="")
    {
        return -ENOENT;
    }

    if ((fi->flags & O_ACCMODE) != O_RDONLY)
    {
        // Only reading allowed
        return -EACCES;
    }

    cout <<  "Prototype::open path=" << fusedPath << endl;
    fi->fh = ::open(fusedPath.c_str(), fi->flags);
    return 0;
}

int Prototype::read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
    const auto fusedPath = FusePathToRealPath(path);
    if(fusedPath=="")
    {
        return -ENOENT;
    }

    cout <<  "Prototype::read path=" << fusedPath << endl;
    return ::pread(fi->fh, buf, size, offset);
}

int Prototype::release(const char* path, struct fuse_file_info* fi)
{
    const auto fusedPath = FusePathToRealPath(path);
    if(fusedPath=="")
    {
        return -ENOENT;
    }

    cout <<  "Prototype::release path=" << fusedPath << endl;
    return ::close(fi->fh);;
}

int Prototype::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
{
    const auto fusedPath = FusePathToRealPath(path);
    if(fusedPath=="")
    {
        return -ENOENT;
    }

    cout <<  "Prototype::readdir path=" << fusedPath << endl;

    unique_ptr<DIR, decltype(&::closedir)> dir(::opendir(fusedPath.c_str()), ::closedir);
    if (!dir)
    {
        // TODO: Review error handling
        return errno;
    }

    auto* dp = dir.get();

    auto* de = ::readdir(dp);
    if (!de)
    {
        // TODO: Review error handling
        return errno;
    }

    do
    {
        if (filler(buf, de->d_name, nullptr, 0) != 0)
        {
            // TODO: Review error handling
            return -1;
        }
    }
    while ((de = ::readdir(dp)));

    return 0;
}

int Prototype::releasedir(const char* path, struct fuse_file_info* fi)
{
    const auto fusedPath = FusePathToRealPath(path);
    if(fusedPath=="")
    {
        return -ENOENT;
    }

    cout <<  "Prototype::releasedir path=" << fusedPath << endl;
    return 0;
}

json readDefaultConfig(const fs::path& configFileName)
{
    fstream configFile(configFileName.c_str(), ios_base::in);
    json j;
    configFile >> j;
    std::cout << std::setw(4) << j << std::endl;
    return j;
}

int main(int argc, char* argv[])
{
    const fs::path configFileName = autoExpandEnvironmentVariables("${HOME}/.sourcesfs");
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
    return runFuse(argc, argv, prototype);
}

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
using path_fragment = string;

struct RepoConfig
{
    vector<string> args;
    string urlTemplate;
};

struct RepoPathInfo
{
    const RepoConfig& repoConfig;
    string url;
    vector<path_fragment> pathFragments;
};

string expandRepoUrlTemplate(
    const RepoConfig& repoConfig,
    vector<path_fragment>::const_iterator& iter,
    vector<path_fragment>::const_iterator& endIter)
{
    auto url(repoConfig.urlTemplate);
    for (const auto& arg : repoConfig.args)
    {
        const auto token = "${" + arg + "}";
        const auto& argValue = *iter++;
        const auto findIter = url.find(token);
        if (findIter != string::npos)
        {
            url.replace(findIter, token.size(), argValue);
        }
    }

    return url;
}

RepoPathInfo resolveRepoPathInfo(
    const map<string, RepoConfig>& repoConfigs,
    const vector<path_fragment>& pathFragments)
{
    if (pathFragments.size() < 1)
    {
        throw runtime_error("Invalid path");
    }

    auto pathFragmentIter = pathFragments.cbegin();
    auto pathFragmentEndIter = pathFragments.cend();

    const auto providerName = *pathFragmentIter++;
    const auto repoConfigIter = repoConfigs.find(providerName);
    if (repoConfigIter == repoConfigs.cend())
    {
        throw runtime_error("Could not find provider \"" + providerName + "\"");
    }

    const auto& repoConfig = repoConfigIter->second;
    const auto url = expandRepoUrlTemplate(repoConfig, pathFragmentIter, pathFragmentEndIter);

    return RepoPathInfo
    {
        repoConfig,
        url,
        { pathFragmentIter, pathFragmentEndIter }
    };
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
        const auto& url = config.second.at("url");
        providerConfigs.insert(pair<string, ProviderConfig>(name, ProviderConfig(name, type, url)));
    }

    return providerConfigs;
}

fs::path Prototype::FusePathToRealPath(const char* path)
{
    // Split path
    fs::path p = path;
    vector<string> elements;
    for(auto& part : p)
    {
        elements.push_back(part.native());
    }

    if(elements.size())
    {
        // Eliminate metadata files translations
        // https://apple.stackexchange.com/questions/14980/why-are-dot-underscore-files-created-and-how-can-i-avoid-them
        if(0==elements.back().find("._"))
        {
            return "";
        }
    }

    if(elements.size()<2)
    {
        return _defaultFallbackPath;
    }

    auto it = _providerConfigs.find(elements[1]);
    if(it == _providerConfigs.end())
    {
       return "";
    }

    const ProviderConfig &conf = it->second;
    shared_ptr<Provider> provider;
    fs::path relPath;
    fs::path idPath;
    int pathOffset=0;

    if (conf.type() == "git")
    {
        pathOffset=5;
        if (elements.size() >= pathOffset)
        {
            // Path exists as provider?
            idPath = fs::path(elements[1]) / fs::path(elements[2]) / fs::path(elements[3]) / fs::path(elements[4]);
            auto itProvider = _providers.find(idPath.string());
            if (itProvider == _providers.end())
            {
                const ProviderConfig &conf = it->second;

                // Prepare final url
                string url = conf.urlTemplate();
                auto f1 = url.find("${group}");
                url.replace(f1, 8, elements[2]);
                auto f2 = url.find("${name}");
                url.replace(f2, 7, elements[3]);

                provider = make_shared<GitProvider>(url, elements[4], fs::temp_directory_path());
                _providers.insert(make_pair(idPath.string(), provider));
            }
            else
            {
                provider = itProvider->second;
            }
        }
    }
    else if (conf.type() == "p4")
    {
        pathOffset=3;
        if (elements.size() >= pathOffset)
        {
            idPath = fs::path(elements[1]) / fs::path(elements[2]);

            auto itProvider = _providers.find(idPath.string());
            if (itProvider == _providers.end())
            {
                const ProviderConfig &conf = it->second;
                provider = make_shared<P4Provider>(conf.urlTemplate(), elements[2], fs::temp_directory_path());
                _providers.insert(make_pair(idPath.string(), provider));
            }
            else
            {
                provider = itProvider->second;
            }
        }
    }
    else if (conf.type() == "cache")
    {
        pathOffset=2;
        if (elements.size() >= pathOffset)
        {
            idPath = fs::path(elements[1]);
        }

        auto itProvider = _providers.find(idPath.string());
        if (itProvider == _providers.end())
        {
            const ProviderConfig &conf = it->second;
            provider = make_shared<CacheProvider>(conf.urlTemplate());
            _providers.insert(make_pair(idPath.string(), provider));
        }
        else
        {
            provider = itProvider->second;
        }
    }

    if (provider)
    {
        for (auto element = elements.begin() + pathOffset; element != elements.end(); ++element)
        {
            relPath /= *element;
        }
        fs::path absPath = provider->retrieve(relPath);
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
    map<string, RepoConfig> repoConfigs;
    repoConfigs.emplace(make_pair<string, RepoConfig>("gitlab", { { "group", "name" }, "git@gitlab.com:${group}/${name}.git" }));

    const auto repoPathInfo = resolveRepoPathInfo(repoConfigs, { "gitlab", "xxx", "yyy", "commit", "a", "b", "c" });
    cout << repoPathInfo.url << endl;
    for (const auto& pathFragment : repoPathInfo.pathFragments)
    {
        cout << "  " << pathFragment << endl;
    }

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

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
#pragma once

#include <boost/filesystem.hpp>
#include <FuseHandler.hpp>
#include <ProviderConfig.hpp>
#include <Provider.hpp>
#include <map>
#include <json/json.hpp>

namespace fs = boost::filesystem;
using namespace std;
using json = nlohmann::json;

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

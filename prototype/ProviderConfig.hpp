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
#pragma once

#include <memory>
#include <string>
#include <vector>

using path_element = std::string;

class Provider;
enum class ProviderType;

struct RepoPathInfo
{
    std::string key;
    std::string url;
    std::string revision;
    std::vector<path_element> pathElements;
};

class ProviderConfig final
{
//public:
//    ProviderConfig(ProviderConfig&) = default;
//    ProviderConfig(ProviderConfig&&) = default;
//    ProviderConfig& operator=(ProviderConfig&) = default;
//    ProviderConfig& operator=(ProviderConfig&&) = default;

public:
    ProviderConfig(
        const std::string& name,
        ProviderType providerType,
        const std::string& urlTemplate,
        const std::vector<std::string>& args);

    RepoPathInfo resolvePath(const std::vector<path_element>& pathElements) const;
    std::shared_ptr<Provider> makeProvider(const std::string& url, const std::string& revision) const;

private:
    const std::string _name;
    const ProviderType _providerType;
    const std::string _urlTemplate;
    const std::vector<std::string> _args;
    const bool _hasRevision;
};

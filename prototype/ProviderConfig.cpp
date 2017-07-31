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
#include "ProviderConfig.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

ProviderConfig::ProviderConfig(
    const string& name,
    const string& type,
    const string& urlTemplate,
    const vector<string>& args)
    : _name(name)
    , _type(type)
    , _urlTemplate(urlTemplate)
    , _args(args)
    , _hasRevision(type != "cache")
{
}

const string& ProviderConfig::type() const
{
    return _type;
}

RepoPathInfo ProviderConfig::resolvePath(const vector<path_element>& pathElements) const
{
    auto pathElementIter = pathElements.cbegin();
    auto pathElementEndIter = pathElements.cend();

    // Check that path begins with "/"
    if (*pathElementIter++ != "/")
    {
        throw runtime_error("Invalid path");
    }

    // Check that provider specified in path matches this provider
    string key = *pathElementIter++;
    if (key != _name)
    {
        throw runtime_error("Invalid VCS provider");
    }

    auto url(_urlTemplate);
    for (const auto& arg : _args)
    {
        const auto token = "${" + arg + "}";
        const auto& argValue = *pathElementIter++;
        key += "/" + argValue;
        const auto findIter = url.find(token);
        if (findIter != string::npos)
        {
            url.replace(findIter, token.size(), argValue);
        }
    }

    // Add revision number to key: key is all of the user-supplied path up to
    // and including the revision
    string revision;
    if (_hasRevision)
    {
        revision = *pathElementIter++;
        key += "/" + revision;
    }

    return RepoPathInfo
    {
        key,
        url,
        revision,
        { pathElementIter, pathElementEndIter }
    };
}

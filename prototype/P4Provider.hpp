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
#include <string>

#include "Provider.hpp"

class P4Provider : public Provider
{
public:
    P4Provider() = delete;
    P4Provider(const P4Provider&) = delete;
    P4Provider& operator=(const P4Provider&) = delete;

public:
    P4Provider(const std::string& repositoryUrl, const std::string& commitNumber, const boost::filesystem::path& workingDirectoryRoot);
    virtual ~P4Provider() override;

    boost::filesystem::path retrieve(const boost::filesystem::path& path) override;

private:
    const std::string _repositoryUrl;
    const std::string _commitNumber;
    const boost::filesystem::path _workingDirectory;
};

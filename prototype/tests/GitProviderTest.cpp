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

#include "GitProvider.hpp"
#include <catch/catch.hpp>
#include <memory>
#include <iostream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
// #include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;

bool sync_gitlab_project()
{
    fs::path workDir = fs::temp_directory_path();
    std::unique_ptr<GitProvider> git = std::make_unique<GitProvider>("https://github.com/joerghall/sourcesfs.git", "19fc775a34edbd2f560c9f7002299f728796ad5b", workDir.c_str());
    std::cout << "work directory:" << workDir << std::endl;
    return true;
}

TEST_CASE( "sync_gitlab_project", "[sync_gitlab_project]")
{
    REQUIRE( sync_gitlab_project() == true );
}

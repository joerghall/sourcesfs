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
#include "CacheProvider.hpp"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;
#include <iostream>
#include <sstream>

using namespace std;

CacheProvider::CacheProvider(const std::string& repositoryUrl)
    : _repositoryUrl(repositoryUrl)
    , _workingDirectory(repositoryUrl)
{

}

CacheProvider::~CacheProvider()
{
    std::cout << "LEAVE CacheProvider::~CacheProvider()" << std::endl;
}

boost::filesystem::path CacheProvider::retrieve(const boost::filesystem::path& path)
{
    return _workingDirectory / path;
}

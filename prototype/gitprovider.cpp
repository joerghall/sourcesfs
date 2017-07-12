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
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
// #include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;
#include <iostream>
#include <sstream>


GitProvider::GitProvider(std::string repositoryUrl, std::string commitHash, std::string workingDirectory)
    : _repositoryUrl(repositoryUrl)
    , _commitHash(commitHash)
{
    for(auto c : repositoryUrl)
    {
        if(std::isalnum(c))
        {
            _aliasName.push_back(c);
        }
    }

    // Make cache directory
    fs::path cachePath = fs::path(workingDirectory) / fs::path(_aliasName + "_" + commitHash);
    _workingDirectory = cachePath.c_str();

    if(fs::exists(_workingDirectory))
    {
        std::cout << "remove " << _workingDirectory << std::endl;
        fs::remove_all(_workingDirectory);
    }

    std::stringstream ssClone;
    ssClone << "git clone " << repositoryUrl << " " << _workingDirectory;
    std::cout << ssClone.str() << std::endl;
    int rc = std::system(ssClone.str().c_str());
    if(rc!=0)
    {
        throw std::runtime_error("Clone failed");
    }

    std::stringstream ssCheckout;
    ssCheckout << "git -C "  << _workingDirectory << " checkout " << _commitHash;
    std::cout << ssCheckout.str() << std::endl;
    rc = std::system(ssCheckout.str().c_str());
    if(rc!=0)
    {
        throw std::runtime_error("Checkout failed");
    }
}

GitProvider::~GitProvider()
{
    try
    {
        if(fs::exists(_workingDirectory))
        {
            fs::remove_all(_workingDirectory);
        }
    }
    catch(const std::exception &e)
    {
        std::cout << "GitProvider::~GitProvider() exception=" << e.what() << std::endl;
    }

    std::cout << "LEAVE GitProvider::~GitProvider()" << std::endl;
}

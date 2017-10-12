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
#include "P4Provider.hpp"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

namespace
{
    string makeAliasName(const string& repositoryUrl)
    {
        string aliasName;
        for (auto c : repositoryUrl)
        {
            if (isalnum(c))
            {
                aliasName.push_back(c);
            }
        }

        return aliasName;
    }

    fs::path makeWorkingDirectory(const string& repositoryUrl, const string& commitNumber, const fs::path& workingDirectoryRoot)
    {
        auto aliasName = makeAliasName(repositoryUrl);
        return fs::path(workingDirectoryRoot) / fs::path(aliasName + "_" + commitNumber);
    }
}

P4Provider::P4Provider(const std::string& repositoryUrl, const std::string& commitNumber, const boost::filesystem::path& workingDirectoryRoot)
    : _repositoryUrl(repositoryUrl)
    , _commitNumber(commitNumber)
    , _workingDirectory(workingDirectoryRoot)
{
    if(fs::exists(_workingDirectory))
    {
        std::cout << "remove " << _workingDirectory << std::endl;
        fs::remove_all(_workingDirectory);
    }

    fs::create_directories(_workingDirectory);
}

P4Provider::~P4Provider()
{
    std::cout << "LEAVE P4Provider::~P4Provider()" << std::endl;
}

boost::filesystem::path P4Provider::retrieve(const boost::filesystem::path& path)
{
    fs::path target =  _workingDirectory / path;
    if(fs::exists(target))
    {
        return target;
    }

    std::stringstream fileDir;
    fileDir << "p4 dirs //" << path.string() << "@" << _commitNumber << " >" << (_workingDirectory / "stdout.txt" ).string() << " 2>" << (_workingDirectory / "stderr.txt" ).string();
    std::string fileDirCmd = fileDir.str();

    std::stringstream fileSync;
    fileSync << "p4 print //" << path.string() << "@" << _commitNumber << " >" << target.string() << " 2>" << (_workingDirectory / "stderr.txt" ).string();
    std::string fileSyncCmd = fileSync.str();

    fs::path pathCache = target.parent_path();
    if(!fs::exists(pathCache))
    {
        std::system(fileSyncCmd.c_str());
        fs::create_directories(pathCache);
    }

    std::system(fileSyncCmd.c_str());
    std::ifstream ifs((_workingDirectory / "stderr.txt").string());
    std::string stringError((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();

    if(stringError.length())
    {
        // Check if dir
        fs::remove(target);

        std::system(fileDirCmd.c_str());

        std::ifstream ifs1((_workingDirectory / "stderr.txt").string());
        std::string stringError1((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        ifs1.close();
        if(stringError1.length()==0)
        {
            fs::create_directories(target);
            cout << "P4: dir created" << target << endl;
        }
    } else {
        cout << "P4: file created" << target << endl;
    }
    return target;
}

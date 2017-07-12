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
#include <cstring>
#include <errno.h>

namespace fs = boost::filesystem;
using namespace std;

static const char  *file_path      = "/hello.txt";
static const char   file_content[] = "Hello World!\n";
static const size_t file_size      = sizeof(file_content)/sizeof(char) - 1;

class Prototype final : public FuseHandler
{
public:
    Prototype(const Prototype&) = delete;
    Prototype& operator=(const Prototype&) = delete;

public:
    Prototype();
    ~Prototype();

    int getattr(const char* path, struct stat* stbuf) override;
    int open(const char* path, struct fuse_file_info* fi) override;
    int readdir(const char* path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) override;
    int read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) override;

private:
    // Example of state shared between handlers defined in this class
    std::unique_ptr<GitProvider> _git;
};

Prototype::Prototype()
{
    fs::path workingDirectoryRoot = fs::temp_directory_path();
    /*
    _git = make_unique<GitProvider>(
        "https://github.com/joerghall/sourcesfs.git",
        "19fc775a34edbd2f560c9f7002299f728796ad5b",
        workingDirectoryRoot.c_str());
    */
}

Prototype::~Prototype() = default;

int Prototype::getattr(const char* path, struct stat* stbuf)
{
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) { /* The root directory of our file system. */
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 3;
    } else if (strcmp(path, file_path) == 0) { /* The only file we have. */
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = file_size;
    } else /* We reject everything else. */
        return -ENOENT;

    return 0;
}

int Prototype::open(const char* path, struct fuse_file_info* fi)
{
    if (strcmp(path, file_path) != 0) /* We only recognize one file. */
        return -ENOENT;

    if ((fi->flags & O_ACCMODE) != O_RDONLY) /* Only reading allowed. */
        return -EACCES;

    return 0;
}

int Prototype::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
{
    if (strcmp(path, "/") != 0) /* We only recognize the root directory. */
        return -ENOENT;

    filler(buf, ".", NULL, 0);           /* Current directory (.)  */
    filler(buf, "..", NULL, 0);          /* Parent directory (..)  */
    filler(buf, file_path + 1, NULL, 0); /* The only file we have. */

    return 0;
}

int Prototype::read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
    if (strcmp(path, file_path) != 0)
        return -ENOENT;

    if (offset >= file_size) /* Trying to read past the end of file. */
        return 0;

    if (offset + size > file_size) /* Trim the read to the file size. */
        size = file_size - offset;

    memcpy(buf, file_content + offset, size); /* Provide the content. */

    return size;
}

int main(int argc, char* argv[])
{
    Prototype prototype;
    return runFuse(argc, argv, prototype);
}

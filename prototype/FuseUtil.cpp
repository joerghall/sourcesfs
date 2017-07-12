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

namespace
{
    int fuse_getattr(const char* path, struct stat* stbuf)
    {
        auto* ctx = fuse_get_context();
        auto* self = static_cast<FuseHandler*>(ctx->private_data);
        return self->getattr(path, stbuf);
    }

    int fuse_open(const char* path, struct fuse_file_info* fi)
    {
        auto* ctx = fuse_get_context();
        auto* self = static_cast<FuseHandler*>(ctx->private_data);
        return self->open(path, fi);
    }

    int fuse_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
    {
        auto* ctx = fuse_get_context();
        auto* self = static_cast<FuseHandler*>(ctx->private_data);
        return self->read(path, buf, size, offset, fi);
    }

    int fuse_release(const char* path, struct fuse_file_info* fi)
    {
        auto* ctx = fuse_get_context();
        auto* self = static_cast<FuseHandler*>(ctx->private_data);
        return self->release(path, fi);
    }

    int fuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
    {
        auto* ctx = fuse_get_context();
        auto* self = static_cast<FuseHandler*>(ctx->private_data);
        return self->readdir(path, buf, filler, offset, fi);
    }

    int fuse_releasedir(const char* path, struct fuse_file_info* fi)
    {
        auto* ctx = fuse_get_context();
        auto* self = static_cast<FuseHandler*>(ctx->private_data);
        return self->releasedir(path, fi);
    }
}

int runFuse(int argc, char* argv[], FuseHandler& handler)
{
    fuse_operations ops{ nullptr };
    ops.getattr = fuse_getattr;
    ops.open = fuse_open;
    ops.read = fuse_read;
    ops.release = fuse_release;
    ops.readdir = fuse_readdir;
    ops.releasedir = fuse_releasedir;
    return fuse_main(argc, argv, &ops, &handler);
}

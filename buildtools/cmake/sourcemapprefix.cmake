# MIT License
#
# Copyright (c) 2017 Joerg Hallmann
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# https://github.com/joerghall/sourcesfs
#
cmake_minimum_required(VERSION 3.0)

# asource_map_prefix
# Adds -fdebug-prefix-map option to compiler flags when supported: instead of
# embedding absolute source paths in binaries, which will vary from build
# to build
function(source_map_prefix source_dir)
    include(CheckCXXCompilerFlag)

    # Do not emit "failure" message to output
    set(saved_CMAKE_REQUIRED_QUIET ${CMAKE_REQUIRED_QUIET})
    set(CMAKE_REQUIRED_QUIET 1)
    check_cxx_compiler_flag(-fdebug-prefix-map=a=b SUPPORTS_DEBUG_PREFIX_MAP)
    set(CMAKE_REQUIRED_QUIET ${saved_CMAKE_REQUIRED_QUIET})

    if(SUPPORTS_DEBUG_PREFIX_MAP)
        message("-fdebug-prefix-map is supported")
        message("Remapping source paths from ${CMAKE_SOURCE_DIR} to ${source_dir}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdebug-prefix-map=${CMAKE_SOURCE_DIR}=${source_dir}" PARENT_SCOPE)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1" PARENT_SCOPE)
    else()
        message("-fdebug-prefix-map is not supported")
    endif()
endfunction()

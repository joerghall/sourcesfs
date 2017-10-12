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

include(filelist.cmake)
include(fuse)

# Prototype
include_directories(
    SYSTEM /usr/local/include/osxfuse/fuse
    SYSTEM ${CMAKE_SOURCE_DIR}/_thirdparty/include
)

link_directories(
    /usr/local/lib
)

add_definitions(
    -DFUSE_USE_VERSION=26
    -D_FILE_OFFSET_BITS=64
    -D_DARWIN_USE_64_BIT_INODE
)

add_executable(sourcesfs
    ${prototype_sources}
    ${prototype_headers}
    sourcesfs.cpp sourcesfs.h)

set(common_LIBRARIES ${Boost_LIBRARIES} ${fuse_LIBRARIES})

target_link_libraries(sourcesfs
    ${common_LIBRARIES}
)

#set_target_properties(sourcesfs PROPERTIES
#    MACOSX_BUNDLE_BUNDLE_NAME SourcesFS
#    MACOSX_FRAMEWORK_INFO_PLIST res/Info.plist
#    MACOSX_BUNDLE_ICON_FILE res/app.ico
#)

# Prototype unittest
#
include_directories(
    .
)

add_executable(test_sourcesfs
    ${prototype_sources_test}
    ${prototype_headers_test}
)


target_link_libraries(test_sourcesfs
    ${common_LIBRARIES}
)

add_test(NAME test_sourcesfs
    COMMAND test_sourcesfs
)

install(TARGETS sourcesfs RUNTIME DESTINATION bin)

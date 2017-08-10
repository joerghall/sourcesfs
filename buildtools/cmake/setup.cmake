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

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}/components")
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}")

get_filename_component(temp_file_path ${CMAKE_CURRENT_LIST_DIR}/.. REALPATH)
set(BUILD_TOOLS ${temp_file_path} CACHE PATH "Location of the buildtools" FORCE)

include(os)

if(BUILD_BITNESS)
else()
    set(BUILD_BITNESS x86_64 CACHE STRING "Bitness of the build" FORCE)
endif()

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
    set(BUILD_PLATFORM "osx${CMAKE_OSX_DEPLOYMENT_TARGET}" CACHE STRING "Platform of the build" FORCE)
elseif (UNIX)
    set(BUILD_PLATFORM ${BUILD_OS}${BUILD_OS_VERSION} CACHE STRING "Platform of the build" FORCE)
endif ()

if (CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Multipe build configurations - IDE build")
else ()
    if (CMAKE_BUILD_TYPE)
        set(BUILD_TYPE debug CACHE STRING "Build type" FORCE)
    else ()
        set(BUILD_TYPE release CACHE STRING "Build type" FORCE)
    endif ()
endif ()

include(version)
include(sourcemapprefix)

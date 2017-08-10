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

if (WIN32)

    message(FATAL_ERROR "Unsupported OS version - Windows")
    set(BUILD_OS windows CACHE STRING "Build operating system" FORCE)
    set(BUILD_WINDOWS 1 CACHE STRING "Builds windows" FORCE)

elseif (APPLE)

    set(BUILD_OS osx CACHE STRING "Build operating system" FORCE)
    set(BUILD_OSX 1 CACHE STRING "Builds osx" FORCE)
    set(BUILD_POSIX 1 CACHE STRING "Builds on posix platfrom" FORCE)

elseif (UNIX)

    set(BUILD_LINUX 1 CACHE STRING "Builds Linux" FORCE)
    set(BUILD_POSIX 1 CACHE STRING "Builds on posix platfrom" FORCE)

    if (EXISTS /etc/redhat-release)
        file(READ /etc/redhat-release release_file)
    elseif (EXISTS /etc/lsb-release)
        file(READ /etc/lsb-release release_file)
    else ()
        message(FATAL_ERROR "Unknown release schema for Unix os")
    endif ()

    if (release_file MATCHES Ubuntu)

        string(REGEX REPLACE ".*Ubuntu ([0-9]+)[.]([0-9]+).*" "\\1" OS_VERSION "${release_file}")

        set(BUILD_OS ubuntu CACHE STRING "Build operating system" FORCE)
        set(BUILD_OS_VERSION ${OS_VERSION} CACHE STRING "Build operating system version" FORCE)

    elseif (release_file MATCHES "Fedora|CentOS|RedHat")

        if (CMAKE_MATCH_0 STREQUAL "Fedora")

            set(BUILD_OS fc CACHE STRING "Build operating system" FORCE)

        elseif (CMAKE_MATCH_0 STREQUAL "CentOS")

            set(BUILD_OS redhat CACHE STRING "Build operating system" FORCE)

        elseif (CMAKE_MATCH_0 STREQUAL "RedHat")

            set(BUILD_OS redhat CACHE STRING "Build operating system" FORCE)

        endif ()

        string(REGEX REPLACE ".*release ([0-9]+)[. ].*" "\\1" OS_VERSION "${release_file}")
        set(BUILD_OS_VERSION ${OS_VERSION} CACHE STRING "Build operating system version" FORCE)

    else ()

        message(FATAL_ERROR "Unknown linux distribution")

    endif ()

else ()

    message(FATAL_ERROR "Unsupported OS version")

endif ()

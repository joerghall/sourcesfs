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
cmake_minimum_required(VERSION 3.5)

include(buildtools/cmake/setup.cmake)

discover_source_version()

set(CMAKE_CXX_STANDARD 14)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
endif()

project(sourcesfs)

enable_testing()

source_map_prefix("sourcesfs/github/joerghall/sourcesfs/${SOURCE_COMMIT_ID}")

add_definitions(-DSOURCE_VERSION="${SOURCE_VERSION}")

if(UNIX AND NOT APPLE)
    add_definitions(-DCONFIG_LINUX)
elseif(APPLE)
    add_definitions(-DCONFIG_OSX)
else()
    add_definitions(-DCONFIG_WINDOWS)
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(Boost
    COMPONENTS filesystem
)
set(Boost_USE_STATIC_RUNTIME on)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_subdirectory(prototype)

include(InstallRequiredSystemLibraries)
#set(CPACK_PACKAGING_INSTALL_PREFIX “/opt/sourcesfs/{version}”)
#CPACK_PROJECT_CONFIG_FILE
set(CPACK_GENERATOR "TGZ")

set(CPACK_PACKAGE_VERSION ${SOURCE_VERSION})
set(CPACK_SYSTEM_NAME ${BUILD_PLATFORM})
SET(CPACK_PACKAGE_FILE_NAME "sourcesfs-${BUILD_PLATFORM}-${BUILD_BITNESS}-${BUILD_TYPE}-${SOURCE_BRANCH}.${SOURCE_VERSION}")
#set(CPACK_PACKAGE_VERSION_MAJOR "0")
#set(CPACK_PACKAGE_VERSION_MINOR "0")
#set(CPACK_PACKAGE_VERSION_PATCH "0")

# RPM
#set(CPACK_RPM_PACKAGE_NAME    "${CPACK_PACKAGE_NAME}")                done by default!
#set(CPACK_RPM_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")             done by default!
#set(CPACK_RPM_PACKAGE_RELEASE "1")                                    done by default, do alter if packaging errors have to be fixed!
#set(CPACK_RPM_PACKAGE_RELEASE "2")                        activate, if the previous packaging was buggy and/or for fancy Linux distro specific numbering!
#set(CPACK_RPM_PACKAGE_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}") done by default!
#set(CPACK_RPM_PACKAGE_VENDOR  "${CPACK_PACKAGE_VENDOR}")              done by default!

#if(BUILD_X64)
#    set(CPACK_RPM_PACKAGE_ARCH "x86_64")
#else(BUILD_X32)
#    set(CPACK_RPM_PACKAGE_ARCH "i386")
#endif(BUILD_X64)
#set(CPACK_RPM_FILE_NAME "${CPACK_RPM_PACKAGE_NAME}-${CPACK_RPM_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.${CPACK_RPM_PACKAGE_ARCH}.rpm")
#set(CPACK_RPM_PACKAGE_LICENSE "${CPACK_RESOURCE_FILE_LICENSE}")

# DEB
include(CPack)

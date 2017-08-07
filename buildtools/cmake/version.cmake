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

# Discovers the version based on rules of the souce control system
#
# the logic for disconvery is implemented in get-version-info.py
#
# Summary git branch name discovery
#
# master  => 0.0.<revision>.r<shaa[0:8]>
# release_<major>_<minor> => <major>.<minor>.<revision>.r<shaa[0:8]>
# => 0.0.<revision>.r<shaa[0:8]>
#
function(discover_source_version)

    execute_process(COMMAND ${BUILD_TOOLS}/buildpy/get-version-info.py --source_path ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${BUILD_TOOLS}/buildpy
        RESULT_VARIABLE VERSION_RESULT
        OUTPUT_VARIABLE VERSION_DATA
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    list(GET VERSION_DATA 0 SOURCE_BRANCH)
    list(GET VERSION_DATA 1 SOURCE_VERSION)
    list(GET VERSION_DATA 2 SOURCE_COMMIT_ID)

    SET(SOURCE_BRANCH ${SOURCE_BRANCH} CACHE STRING "Branch name from sources" FORCE)
    SET(SOURCE_VERSION ${SOURCE_VERSION} CACHE STRING "Version discovered from sources" FORCE)
    SET(SOURCE_COMMIT_ID ${SOURCE_COMMIT_ID} CACHE STRING "Commit Id from sources" FORCE)

endfunction()

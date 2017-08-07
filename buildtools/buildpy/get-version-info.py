#!/usr/bin/env python
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
from __future__ import print_function

from pyart.version_info import VersionInfo
from pyprelude.file_system import make_path

import argparse
import os
import sys

SCRIPT_PATH = os.path.realpath(__file__)


def main_inner(args):
    v = VersionInfo(args.source_path)
    print("{};{};{}".format(v.branch, v, v.hash_long))

def main():
    parser = argparse.ArgumentParser(description="Compute version number")
    parser.add_argument("--source_path", default=make_path(SCRIPT_PATH, "..", ".."), help="path git source repository", type=os.path.abspath)
    args = parser.parse_args()

    result = main_inner(args)
    if result is None:
        sys.exit(0)
    else:
        sys.stderr.write("{} validation errors were detected.\n".format(len(result)))
        for error in result:
            sys.stderr.write("  {}\n".format(error))
        sys.exit(1)

if __name__ == "__main__":
    main()

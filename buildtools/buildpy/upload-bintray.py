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

import argparse
import os
import os.path
import re
import requests
import sys

API_BASE = os.environ.get("BINTRAY_URL", "https://api.bintray.com/content")
PACKAGE_REGEX = re.compile(r"([^-]+)-([^-]+)-([^-]+)-([^-]+)-([^.]+?)\.(([^.]+)\.([^.]+?)\.([^.]+?)\.([^.]+?))\.(tgz|rpm|deb|tgz|zip)")
SCRIPT_PATH = os.path.realpath(__file__)

def upload(package_path, url, apiuser, apikey):

    print("Starting upload {0} to {1}".format(package_path, url))

    parameters = {"publish": "1", "override": "1"}

    with open(package_path, "rb") as fp:
        response = requests.put(url, auth=(apiuser, apikey), params=parameters, data=fp)

    if response.status_code != 201:
        raise RuntimeError("Uploading package {0} to {1} - code: {2} msg: {3}".format(package_path, url, response.status_code, response.text))
    else:
        print("Completed uploading package {0} to {1}".format(package_path, url))

def main_inner(args):

    for (path, dirs, files) in os.walk(args.path):
        for file in files:
            file_full = os.path.join(path, file)
            file_name = file.replace(".tar.gz", ".tgz")

            version = PACKAGE_REGEX.match(file_name)
            if version is None:
                raise RuntimeError("Unable to parse version from filename {0}".format(file_name))

            if args.package is None:
                args.package = version.group(1)
            if args.branch is None:
                args.branch = version.group(5)
            if args.version is None:
                args.version = version.group(6)

            if args.apikey is None and "apikey" in os.environ:
                args.apikey = os.environ["apikey"]
            else:
                raise RuntimeError("No apikey provided")
            if args.apiuser is None and "apiuser" in os.environ:
                args.apiuser = os.environ["apiuser"]
            else:
                raise RuntimeError("No apiuser provided")

            url = "{url}/{repository}/{package}/{branch}/{version}/{basename}".format(
                       url = args.apiurl, repository = args.repository, package = args.package,
                       branch = args.branch, version = args.version, basename = file_name, path = path)

            upload(file_full, url, args.apiuser, args.apikey)

def main():

    parser = argparse.ArgumentParser(description="Upload to bintray")
    parser.add_argument("--path", help="package path to upload", type=os.path.abspath)
    parser.add_argument("--apiuser", help="API user")
    parser.add_argument("--apikey", help="API key")
    parser.add_argument("--apiurl", default="https://api.bintray.com/content", help="API url")
    parser.add_argument("--repository", default="joerghall", help="Name of the package")
    parser.add_argument("--package", default=None, help="Name of the package")
    parser.add_argument("--branch", default=None, help="Name of the branch")
    parser.add_argument("--version", default=None, help="Version of the package")

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

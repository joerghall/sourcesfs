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
import os

from pysimplevcs.git import Git

ON_OSX = True
ON_LIN = False
ON_WIN = False

def _parse_int(s, details):
    try:
        return int(s)
    except ValueError:
        raise ValueError("invalid {} \"{}\"".format(details, s))

def _parse_branch_abbrev(s):
    major_minor = parse_release_branch(s)
    return "0" if major_minor is None else major_minor

def _get_revision_count(git):
    output = git.rev_list("HEAD")
    return len(output.splitlines())

def _get_upstream_branch(git):
    sym_name = "@{u}" if ON_LIN or ON_OSX else "\\@\\{u\\}"
    status, output, error = git.rev_parse("--abbrev-ref", "--symbolic-full-name", sym_name, can_fail=True)
    segments = output.split("/")
    if status == 0 and len(segments) == 2:
        return True, output.rstrip('\n'), segments[1].rstrip('\n')
    else:
        output = git.rev_parse("--abbrev-ref", "HEAD")
        output = output.rstrip('\n')
        return False, output, output

def _parse_release_branch(s):
    components = s.split("_")
    if len(components) != 3 or components[0] != "release":
        return (0, 0)
    try:
        major = _parse_int(components[1], "major")
    except ValueError:
        return (0, 0)
    try:
        minor = _parse_int(components[2], "minor")
    except ValueError:
        return (0, 0)
    return (major, minor)

class VersionInfo(object):

    def __init__(self, cwd=None, args=[]):

        git = Git(cwd, args)

        if "TRAVIS_BRANCH" in os.environ:
            self._branch = os.environ["TRAVIS_BRANCH"]
        else:
            (_, _, self._branch) = _get_upstream_branch(git)

        self._revision = _get_revision_count(git)

        (self._major, self._minor) = _parse_release_branch(self._branch)

        if self._major > 0 or self._minor > 0:
            self._branch = self._branch.split("_")[0]

        self._hash_long = git.rev_parse("HEAD").strip()
        self._hash = self._hash_long[0 : 8]

        self._version = "{}.{}.{}.r{}".format(self._major, self._minor, self._revision, self._hash)

    @property
    def branch(self): return self._branch

    @property
    def version(self): return self._version

    @property
    def major(self): return self._major

    @property
    def minor(self): return self._minor

    @property
    def revision(self): return self._revision

    @property
    def hash(self): return self._hash

    @property
    def hash_long(self): return self._hash_long

    def __repr__(self):
        return self._version

    def __eq__(self, other):
        return self._version == other._version

    def __lt__(self, other):
        temp = cmp(self._major, other._major)
        if temp < 0:
            return True
        elif temp > 0:
            return False

        temp = cmp(self._minor, other._minor)
        if temp < 0:
            return True
        elif temp > 0:
            return False

        temp = cmp(self._revision, other._revision)
        if temp < 0:
            return True
        elif temp > 0:
            return False

        if self._hash == other._hash:
            return False

        raise RuntimeError("Cannot compare {} with {}".format(self, other))

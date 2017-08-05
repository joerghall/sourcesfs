############################################################
#
# pyprelude.temp_util
# Copyright (C) 2017, Richard Cook
# Released under MIT License
# https://github.com/rcook/pyprelude
#
############################################################

from __future__ import print_function
import contextlib
import os
import tempfile

from .file_system import remove_dir

@contextlib.contextmanager
def _temp_helper(context_class):
    obj = None
    try:
        obj = context_class()
        yield obj.path
    finally:
        if obj:
            obj.close()

class TempDir(object):
    def __init__(self):
        self._path = tempfile.mkdtemp()

    @property
    def path(self): return self._path

    def close(self):
        if self._path and os.path.isdir(self._path):
            remove_dir(self._path)

class TempFile(object):
    def __init__(self):
        fd, self._path = tempfile.mkstemp()
        os.close(fd)

    @property
    def path(self): return self._path

    def close(self):
        if self._path and os.path.isfile(self._path):
            os.unlink(self._path)

def temp_dir():
    return _temp_helper(TempDir)

def temp_file():
    return _temp_helper(TempFile)

@contextlib.contextmanager
def temp_cwd(dir):
    try:
        saved_dir = os.getcwd()
        os.chdir(dir)
        yield
    finally:
        os.chdir(saved_dir)

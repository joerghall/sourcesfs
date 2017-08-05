############################################################
#
# pyprelude.file_system
# Copyright (C) 2017, Richard Cook
# Released under MIT License
# https://github.com/rcook/pyprelude
#
############################################################

from __future__ import print_function
import os
import shutil
import stat

from .util import unpack_args

def _on_error(func, path, exc_info):
    if os.access(path, os.W_OK):
        raise
    else:
        os.chmod(path, stat.S_IWUSR)
        func(path)

def remove_dir(dir):
    # TODO: Consider using robocopy on Windows
    shutil.rmtree(dir, onerror=_on_error)

def make_path(*args):
    """
    >>> make_path("/a", "b")
    '/a/b'
    >>> make_path(["/a", "b"])
    '/a/b'
    >>> make_path(*["/a", "b"])
    '/a/b'
    >>> make_path("/a")
    '/a'
    >>> make_path(["/a"])
    '/a'
    >>> make_path(*["/a"])
    '/a'
    """
    paths = unpack_args(*args)
    return os.path.abspath(os.path.join(*[p for p in paths if p is not None]))

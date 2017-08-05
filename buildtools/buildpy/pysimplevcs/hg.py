############################################################
#
# pysimplevcs.hg
# Copyright (C) 2017, Richard Cook
# Released under MIT License
# https://github.com/rcook/pysimplevcs
#
############################################################

from __future__ import print_function
import os

from pyprelude.process import execute, proxy_command

class Hg(object):
    def __init__(self, cwd=None):
        if cwd is None:
            cwd = os.getcwd()

        cwd = os.path.abspath(cwd)

        self._repo_dir = execute("hg", "root", cwd=cwd).strip()

    @property
    def repo_dir(self): return self._repo_dir

    def __getattr__(self, name):
        def _missing_method(*args, **kwargs):
            return proxy_command(name, self._repo_dir, lambda n, args: ["hg", n] + args, *args, **kwargs)

        return _missing_method

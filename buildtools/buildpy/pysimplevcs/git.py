############################################################
#
# pysimplevcs.git
# Copyright (C) 2017, Richard Cook
# Released under MIT License
# https://github.com/rcook/pysimplevcs
#
############################################################

from __future__ import print_function
import os

from pyprelude.file_system import make_path
from pyprelude.process import execute, proxy_command
from pyprelude.util import unpack_args

class _Git(object):
    def __init__(self, is_bare, repo_dir, git_dir, args):
        self._is_bare = is_bare
        self._repo_dir = repo_dir
        self._git_dir = git_dir
        self._args = args

    @property
    def is_bare(self): return self._is_bare

    @property
    def repo_dir(self): return self._repo_dir

    @property
    def git_dir(self): return self._git_dir

    def with_args(self, *args):
        return _Git(
            self._is_bare,
            self._repo_dir,
            self._git_dir,
            self._args + unpack_args(*args))

    def __getattr__(self, name):
        def _missing_method(*args, **kwargs):
            return proxy_command(
                name,
                self._repo_dir,
                lambda n, args: ["git"] + self._args + [n] + args,
                *args,
                **kwargs)

        return _missing_method

class Git(_Git):
    def __init__(self, cwd=None, args=[]):
        if cwd is None:
            cwd = os.getcwd()

        cwd = os.path.abspath(cwd)

        is_bare = execute(
            "git",
            "rev-parse",
            "--is-bare-repository",
            cwd=cwd).strip() == "true"

        if is_bare:
            repo_dir = cwd
            git_dir = repo_dir
        else:
            repo_dir = execute("git", "rev-parse", "--show-toplevel", cwd=cwd).strip()
            git_dir = make_path(repo_dir, ".git")

        super(Git, self).__init__(is_bare, repo_dir, git_dir, args)

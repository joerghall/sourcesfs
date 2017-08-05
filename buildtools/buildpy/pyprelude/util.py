############################################################
#
# pyprelude.util
# Copyright (C) 2017, Richard Cook
# Released under MIT License
# https://github.com/rcook/pyprelude
#
############################################################

from __future__ import print_function
import contextlib
import os

def try_pop(d, key, default):
    """
    >>> d = {"a": "b", "c": "d", "e": "f"}
    >>> try_pop(d, "g", "default")
    'default'
    >>> d
    {'a': 'b', 'c': 'd', 'e': 'f'}
    >>> try_pop(d, "c", "default")
    'd'
    >>> d
    {'a': 'b', 'e': 'f'}
    """
    value = d.get(key, default)
    if key in d:
        d.pop(key)
    return value

def unpack_args(*args):
    l = list(args)
    return l[0] if len(l) == 1 and isinstance(l[0], list) else l

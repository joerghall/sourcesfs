############################################################
#
# pyprelude.process
# Copyright (C) 2017, Richard Cook
# Released under MIT License
# https://github.com/rcook/pyprelude
#
############################################################

from __future__ import print_function
import pipes
import subprocess

from .util import try_pop, unpack_args

def _format_command(command):
    return " ".join(map(pipes.quote, map(str, command)))

def proxy_command(name, cwd, build_command, *args, **kwargs):
    command_args = unpack_args(*args)
    can_fail = try_pop(kwargs, "can_fail", False)
    env = try_pop(kwargs, "env", None)
    stdin = try_pop(kwargs, "stdin", None)
    if len(kwargs) > 0:
        raise ValueError("Unsupported keyword argument(s): {}".format(", ".join(kwargs.keys())))

    sanitized_name = name.replace("_", "-")
    command = build_command(sanitized_name, command_args)

    return execute(
        command,
        can_fail=can_fail,
        cwd=cwd,
        stdin=stdin)

def execute(*args, **kwargs):
    command = map(str, unpack_args(*args))
    can_fail = try_pop(kwargs, "can_fail", False)
    cwd = try_pop(kwargs, "cwd", None)
    env = try_pop(kwargs, "env", None)
    stdin = try_pop(kwargs, "stdin", None)
    if len(kwargs) > 0:
        raise ValueError("Unsupported keyword argument(s): {}".format(", ".join(kwargs.keys())))

    p = subprocess.Popen(
        command,
        cwd=cwd,
        env=env,
        stdin=subprocess.PIPE if stdin else None,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE)

    output, error = p.communicate(input=stdin)
    if can_fail:
        return p.returncode, output, error

    if p.returncode != 0:
        raise RuntimeError("Command \"{}\" failed with status {}: {}".format(
            _format_command(command),
            p.returncode,
            error))

    return output

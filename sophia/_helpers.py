'''
  ' File: _helpers.py
  ' Project: sophia
  ' File Created: 2023-03-03 19:16:43
  ' Author: Rob Graham (robgrahamdev@gmail.com)
  ' Last Modified: 2023-04-09 16:16:48
  ' ------------------
  ' Copyright 2023 Rob Graham
  ' ==================
  ' This program is free software: you can redistribute it and/or modify
  ' it under the terms of the GNU Affero General Public License as published
  ' by the Free Software Foundation, either version 3 of the License, or
  ' (at your option) any later version.
  '
  ' This program is distributed in the hope that it will be useful,
  ' but WITHOUT ANY WARRANTY; without even the implied warranty of
  ' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  ' GNU Affero General Public License for more details.
  '
  ' You should have received a copy of the GNU Affero General Public License
  ' along with this program.  If not, see <https://www.gnu.org/licenses/>.
  ' ==================
'''

from multipledispatch import dispatch

from _types import Parsed_Args


def process_args(argv) -> Parsed_Args:
    dict = {}  # key:value dictionary

    for arg in argv[1:]:
        # key || key=value
        if arg.startswith('-'):
            try:
                k = arg[1:arg.index('=')]
                v = arg[arg.index('=') + 1:]
            except:
                k = arg[1:]
                v = True
            dict[k] = v

    return Parsed_Args(argv[0], argv, dict)


@dispatch(Parsed_Args, list, object)
def get_arg_value(args: Parsed_Args, names, default):
    for name in names:
        if name in args.dict:
            return args.dict[name]

    return default


@dispatch(Parsed_Args, list)
def get_arg_value(args: Parsed_Args, names):
    return get_arg_value(args, names, None)

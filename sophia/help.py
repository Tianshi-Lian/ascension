'''
  ' File: help.py
  ' Project: sophia
  ' File Created: 2023-03-03 20:44:58
  ' Author: Rob Graham (robgrahamdev@gmail.com)
  ' Last Modified: 2023-04-09 16:18:32
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

import importlib
import os
import sys

import _globals
import _helpers
import _types


def run():
    args = _helpers.process_args(sys.argv)

    info = _helpers.get_arg_value(args, ['info', 'i'], False)

    print('Valid commands: \n')
    for f in os.scandir('./scripts'):
        if f.is_file() and f.name.endswith('.py') and not f.name.startswith('_'):
            module_name = os.path.splitext(f.name)[0]
            module = importlib.import_module(module_name)

            help_text = module.get_help_description()

            print(module_name + ':')
            print('  ' + help_text.description)
            if info and len(help_text.params) > 0:
                print('    ' + help_text.params)

    return _globals.SUCCESS


def get_help_description():
    return _types.Help_Text('Outputs command information', '-i (more information)')

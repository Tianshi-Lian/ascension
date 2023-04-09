'''
  ' File: install.py
  ' Project: sophia
  ' File Created: 2023-04-07 15:18:44
  ' Author: Rob Graham (robgrahamdev@gmail.com)
  ' Last Modified: 2023-04-09 16:18:40
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

import _types
import subprocess
import sys

import _globals
import _helpers


def run():
    exit_code = _globals.SUCCESS

    args = _helpers.process_args(sys.argv)

    build_type = _helpers.get_arg_value(
        args, ['build-type', 'bt'], _globals.BUILD_TYPE)

    process = subprocess.run(
        ['cmake', '--install', f'build/{_globals.BUILD_PROJECT_NAME}/', '--config', f'{build_type}'])
    exit_code = process.returncode

    return exit_code


def get_help_description():
    return _types.Help_Text('Install the project', '')

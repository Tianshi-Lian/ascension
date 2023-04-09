'''
  ' File: build.py
  ' Project: sophia
  ' File Created: 2023-03-03 19:14:10
  ' Author: Rob Graham (robgrahamdev@gmail.com)
  ' Last Modified: 2023-04-09 16:18:12
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
import subprocess
import sys

import _globals
import _types
import _helpers


def run():
    args = _helpers.process_args(sys.argv)

    build_type = _helpers.get_arg_value(
        args, ['build-type', 'bt'], _globals.BUILD_TYPE)
    threads = _helpers.get_arg_value(
        args, ['threads', 't'], str(int(os.cpu_count() / 1.2))
    )

    exit_code = _globals.SUCCESS
    if not os.path.isfile('./build/build.ninja'):
        module = importlib.import_module('configure')
        exit_code = module.run()
    else:
        print('ninja: Using previous `build.ninja`')

    if exit_code == _globals.SUCCESS:

        print(
            f'ninja: Building with config `{build_type}` using {str(os.cpu_count()) if threads == "0" else threads} threads', flush=True)

        process = subprocess.run(
            ['ninja', '-C./build', f'-j{threads}', f'-fbuild-{build_type}.ninja', f'{_globals.BUILD_PROJECT_NAME}'])
        exit_code = process.returncode

    return exit_code


def get_help_description():
    return _types.Help_Text('Build the project', '-bt=<build_type>')

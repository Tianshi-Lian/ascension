'''
  ' File: configure.py
  ' Project: sophia
  ' File Created: 2023-03-11 10:38:54
  ' Author: Rob Graham (robgrahamdev@gmail.com)
  ' Last Modified: 2023-04-09 16:18:24
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

import subprocess
import sys

import _helpers
import _types


def run():
    args = _helpers.process_args(sys.argv)

    c_compiler = _helpers.get_arg_value(args, ['cc'], 'clang')
    cxx_compiler = _helpers.get_arg_value(args, ['cxx'], 'clang++')
    c_launcher = _helpers.get_arg_value(args, ['cl'], 'ccache')
    cxx_launcher = _helpers.get_arg_value(args, ['cxxl'], 'ccache')

    process = subprocess.run(['cmake', '-S .',  '-B ./build',  '-GNinja Multi-Config',
                              f'-DCMAKE_C_COMPILER={c_compiler}', f'-DCMAKE_CXX_COMPILER={cxx_compiler}',
                              f'-DCMAKE_C_COMPILER_LAUNCHER={c_launcher}', f'-DCMAKE_CXX_COMPILER_LAUNCHER={cxx_launcher}'])

    exit_code = process.returncode

    return exit_code


def get_help_description():
    return _types.Help_Text('Configure the project', '-cc=<c_compiler> -cxx=<cpp_compiler>')

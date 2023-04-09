'''
  ' File: clean.py
  ' Project: sophia
  ' File Created: 2023-03-03 20:33:35
  ' Author: Rob Graham (robgrahamdev@gmail.com)
  ' Last Modified: 2023-04-09 16:18:16
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

import os
import shutil
import sys

import _globals
import _helpers
import _types


folders_to_clean = ('ints', 'bin', 'dist')
files_to_clean = ('.obj', '.hxx.pch')


def clean_directory(dirname):
    for dirpath, dirs, files in os.walk(dirname, followlinks=False):
       # hard guard against deleting any of our git
        dirs[:] = [d for d in dirs if '.git' not in d]

        for folder in dirs:
            if folder in folders_to_clean:
                shutil.rmtree(os.path.join(dirpath, folder))
                dirs[:] = [d for d in dirs if folder not in d]
        for file in files:
            if file.endswith(files_to_clean):
                os.remove(os.path.join(dirpath, file))
    return


def run():
    exit_code = _globals.SUCCESS
    build_dir = os.path.join('.\\', 'build')

    args = _helpers.process_args(sys.argv)
    clean_all = _helpers.get_arg_value(args, ['a'], False)
    force_clean = _helpers.get_arg_value(args, ['y'])

    try:
        if clean_all:
            if not force_clean:
                print("Clean will delete all build & CMake config files.")
                if input("Are you sure you wish to continue? (y/n): ") != "y":
                    return exit_code

            if (os.path.exists(build_dir)):
                shutil.rmtree(build_dir)
        else:
            clean_directory(build_dir)

        print("Project cleaned")
    except OSError as e:
        exit_code = -1
        print(e)

    return exit_code


def get_help_description():
    return _types.Help_Text('Clean the project build files', '')

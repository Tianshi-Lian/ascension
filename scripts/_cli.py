'''
  ' File: _cli.py
  ' Project: scripts
  ' File Created: 2023-03-03 19:36:23
  ' Author: Rob Graham (robgrahamdev@gmail.com)
  ' Last Modified: 2023-04-09 16:16:40
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

def import_or_install(package):
    try:
        __import__(package)
    except ImportError:
        process = subprocess.run(
            ['python', '-m', 'pip', 'install', '-r', f'./scripts/requirements.txt'])

# TODO: setup some awareness of "command chains/dependencies" where we can register command dependencies and ensure
# /t    all of the dependencies run before trying to execute that command. E.g. if we send `dev run` we ensure and/or try to run
# /t    configure -> build -> install, before run, if needed.


def run_command(commands):
    exit_code = 0

    script = "./scripts/{}.py".format(commands[0])

    # As we just run any file specified by cmd we should at least check it's not one _we_ are ignoring
    if script.startswith("_") or not script.endswith('.py'):
        print("Invalid CLI command! Type `cli help` for a list of commands.")
        exit_code = -1
    else:
        print("==============================================")
        if os.path.exists(script):
            command_name = commands[0]
            print("Executing {} ({}):".format(
                command_name, ", ".join(commands[1:])))

            module = importlib.import_module(command_name)
            exit_code = module.run()
        else:
            print("Invalid CLI command! Type `cli help` for a list of commands.")
            exit_code = -1

    return exit_code


def main():
    import_or_install('dotenv')

    argc = len(sys.argv)
    if argc < 2:
        print("Invalid CLI command! Type `cli help` for a list of commands.")
        sys.exit(1)
    else:
        i = 1
        while i < argc:
            commands = [sys.argv[i]]

            while True:
                if i < argc - 1 and sys.argv[i + 1][0] == "-":
                    commands.append(sys.argv[i + 1][1:])
                    i += 1
                else:
                    break

            command_result = run_command(commands)
            if (command_result != 0):
                # this will stop us running any more commands if we failed one
                sys.exit(command_result)
            i += 1


if __name__ == "__main__":
    main()

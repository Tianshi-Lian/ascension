import importlib
import os
import subprocess
import sys

import _globals
import _helpers


def run():
    args = _helpers.process_args(sys.argv)

    build_type = _helpers.get_arg_value(
        args, ['build-type', 'bt'], _globals.BUILD_TYPE)

    exit_code = _globals.SUCCESS
    if not os.path.isfile('./build/build.ninja'):
        module = importlib.import_module('configure')
        exit_code = module.run()
    else:
        print('ninja: Using previous `build.ninja`')

    if exit_code == _globals.SUCCESS:
        # use 80% of available threads
        wanted_threads = str(int(os.cpu_count() / 1.2))

        print(
            f'ninja: Building with config `{build_type}` using {wanted_threads} threads', flush=True)

        process = subprocess.run(
            ['ninja', '-C./build', f'-j{wanted_threads}', f'-fbuild-{build_type}.ninja'])
        exit_code = process.returncode

    return exit_code


def get_help_description():
    return _globals.Help_Text('Build the project', '-bt=<build_type>')

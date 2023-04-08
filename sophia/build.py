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

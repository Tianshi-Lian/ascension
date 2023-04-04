import importlib
import os
import subprocess
import sys

import _globals
import _helpers
import _types

def run():
    args = _helpers.process_args(sys.argv)

    build_type = _helpers.get_arg_value(
        args, ['build-type', 'bt'], _globals.BUILD_TYPE)
    app_name = _helpers.get_arg_value(
        args, ['name', 'n'], _globals.BUILD_PROJECT_NAME)

    exit_code = _globals.SUCCESS

    libs_dir = f'build/bin/libs/{build_type}'
    executable_dir = f'build/bin/{app_name}/{build_type}'
    executable_ext = '.exe' if _globals.PLATFORM == 'windows' else ''
    executable = f'{executable_dir}/{_globals.BUILD_PROJECT_NAME}'

    if not os.path.isfile(f'{executable}{executable_ext}'):
        print(
            f'Failed to find executable {executable}, attempting to build...')
        module = importlib.import_module('build')
        exit_code = module.run()

    if exit_code != _globals.SUCCESS:
        return exit_code

    if _globals.PLATFORM == 'windows':
        process = subprocess.run([executable], cwd=libs_dir)
        exit_code = process.returncode
    elif _globals.PLATFORM == 'linux':
        process = subprocess.run(
            [f'./{_globals.BUILD_PROJECT_NAME}'], cwd=libs_dir)
        exit_code = process.returncode

    return exit_code


def get_help_description():
    return _types.Help_Text('Run the project', '')

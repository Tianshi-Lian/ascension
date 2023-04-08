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

import importlib
import os
import subprocess
import sys

import _globals
import _helpers
import _types

def check_and_run_dependency(built_executable_path, command):
    exit_code = _globals.SUCCESS

    if not os.path.isfile(f'{built_executable_path}'):
        print(
            f'Failed to find dependency `{built_executable_path}`, attempting to run {command}...')
        module = importlib.import_module(command)
        exit_code = module.run()

    return exit_code

def run():
    exit_code = _globals.SUCCESS

    args = _helpers.process_args(sys.argv)

    build_type = _helpers.get_arg_value(
        args, ['build-type', 'bt'], _globals.BUILD_TYPE)
    app_name = _helpers.get_arg_value(
        args, ['name', 'n'], _globals.BUILD_PROJECT_NAME)

    executable_ext = '.exe' if _globals.PLATFORM == 'windows' else ''

    built_executable_path = f'build/{app_name}/bin/{build_type}/{app_name}{executable_ext}'
    exit_code = check_and_run_dependency(built_executable_path, 'build')
    if exit_code != _globals.SUCCESS:
        return exit_code

    # TODO: check if there is a newer binary to copy too.
    dist_dir = f'build/dist'
    dist_executable_path = f'{dist_dir}/{app_name}{executable_ext}'
    exit_code = check_and_run_dependency(dist_executable_path, 'install')
    if exit_code != _globals.SUCCESS:
        return exit_code

    if _globals.PLATFORM == 'windows':
        process = subprocess.run([dist_executable_path], cwd=dist_dir)
        exit_code = process.returncode
    elif _globals.PLATFORM == 'linux':
        print(f'{os.getcwd()}/{dist_executable_path}')
        process = subprocess.run(
            [f'{os.getcwd()}/{dist_executable_path}'], cwd=f'{os.getcwd()}/{dist_dir}')
        exit_code = process.returncode

    return exit_code


def get_help_description():
    return _types.Help_Text('Run the project', '')

import _globals
import subprocess
import sys
from collections import namedtuple

def run():
    exit_code = _globals.SUCCESS
    workingDir = f'build/{_globals.BUILD_PROJECT_NAME}/bin'
    if _globals.PLATFORM == "windows":
        process = subprocess.run([f'{workingDir}/{_globals.BUILD_PROJECT_NAME}'], cwd=workingDir)
        exit_code = process.returncode
    elif _globals.PLATFORM == "linux":
        process = subprocess.run([f'./{_globals.BUILD_PROJECT_NAME}'], cwd=workingDir)
        exit_code = process.returncode

    return exit_code

def get_help_description():
    return _globals.Help_Text('Run the project', '')

import _globals
import subprocess
import sys

def run():
    exit_code = 0
    workingDir = f'build/{_globals.BUILD_PROJECT_NAME}/bin'
    if _globals.PLATFORM == "windows":
        process = subprocess.run([f'{workingDir}/{_globals.BUILD_PROJECT_NAME}'], cwd=workingDir)
        exit_code = process.returncode
    elif _globals.PLATFORM == "linux":
        process = subprocess.run([f'./{_globals.BUILD_PROJECT_NAME}'], cwd=workingDir)
        exit_code = process.returncode

    return exit_code

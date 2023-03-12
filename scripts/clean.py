import os
import shutil

import _globals


def clean_directory(dirname):
    for dirpath, dirs, _ in os.walk(dirname, followlinks=False):
       # hard guard against deleting any of our git
        dirs[:] = [d for d in dirs if '.git' not in d]

        for folder in dirs:
            if folder == 'build':
                print(f'Removing: {dirpath}/{folder}/*...')
                shutil.rmtree(os.path.join(dirname, folder))
                dirs[:] = [d for d in dirs if folder not in d]
    return


def run():
    exit_code = _globals.SUCCESS
    try:
        clean_directory('.')

    except OSError as e:
        exit_code = -1
        print(e)

    return exit_code


def get_help_description():
    return _globals.Help_Text('Clean the project build files', '')

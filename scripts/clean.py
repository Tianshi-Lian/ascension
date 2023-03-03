import os, sys
import shutil

def clean_directory(dirname):
    for roots, dirs, files in os.walk(dirname):
        for folder in dirs:
            if folder == 'build':
                print(f'Removing: {dirname}/build/*...')
                shutil.rmtree(os.path.join(dirname, folder), ignore_errors=True)
            else:
                clean_directory(os.path.join(dirname, folder))

        for name in files:
            if name.endswith('.sln') or '.vcxproj' in name or name == 'Makefile':
                filename = os.path.join(dirname, name)
                print(f'Removing: {filename}...')
                os.remove(filename)

exit_code = 0
try:
    clean_directory('.')

except OSError as e:
    exit_code = -1

sys.exit(exit_code)

import importlib
import os
import subprocess
import sys

SCRIPTS_DIR = "dunois"

def import_or_install(package):
    try:
        __import__(package)
    except ImportError:
        process = subprocess.run(
            ['python', '-m', 'pip', 'install', '-r', f'./{SCRIPTS_DIR}/requirements.txt'])

# TODO: setup some awareness of "command chains/dependencies" where we can register command dependencies and ensure
# /t    all of the dependencies run before trying to execute that command. E.g. configure -> build -> install -> run.
def run_command(commands):
    exit_code = 0

    script = "./{}/{}.py".format(SCRIPTS_DIR, commands[0])

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

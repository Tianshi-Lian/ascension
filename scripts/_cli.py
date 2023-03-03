# CLI tool for handling build pipeline
# cli help      - displays a list of CLI commands
# cli build     - builds project for active platform

import sys, os
import subprocess

SCRIPTS_DIR = "scripts"
SUCCESS = 0

def run_command(cmds):
    exit_code = 0

    script = "./{}/{}.py".format(SCRIPTS_DIR, cmds[0])

    # As we just run any file specified by cmd we should at least check it's not one _we_ are ignoring
    if script.startswith("_"):
        print("Invalid CLI command! Type `cli help` for a list of commands.")
        exit_code = -1
    else:
        print("==============================================")
        if os.path.exists(script):
            print("Executing {}({}):".format(cmds[0], ", ".join(cmds[1:])))

            cmds[0] = script
            cmds.insert(0, "python")

            process = subprocess.run(cmds)
            exit_code = process.returncode
        else:
            print("Invalid CLI command! Type `cli help` for a list of commands.")
            exit_code = -1

    return exit_code


argc = len(sys.argv)
if argc < 2:
    print("Invalid CLI command! Type `cli help` for a list of commands.")
else:
    i = 1
    while i < argc:
        cmds = [sys.argv[i]]

        while True:
            if i < argc - 1 and sys.argv[i + 1][0] == "-":
                cmds.append(sys.argv[i + 1][1:])
                i += 1
            else:
                break

        if (run_command(cmds) != SUCCESS):
            break # this will stop us running any more commands if we failed one
        i += 1

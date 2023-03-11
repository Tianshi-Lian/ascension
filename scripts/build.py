import _globals
import _helpers
import os, sys
import subprocess
import importlib

def run():
    args = _helpers.process_args(sys.argv)

    build_type = _helpers.get_arg_value(args, ["build-type", "bt"], _globals.BUILD_TYPE)
    c_compiler = _helpers.get_arg_value(args, ["cc"], "clang")
    cxx_compiler = _helpers.get_arg_value(args, ["cxx", "cxx"], "clang++")

    exit_code = _globals.SUCCESS
    if not os.path.isfile("./build/build.ninja"):
        module = importlib.import_module('config')
        exit_code = module.run()
    else:
        print("Using previous build.ninja")

    if exit_code == _globals.SUCCESS:
        wanted_threads = str(int(os.cpu_count() / 1.2)) # use 80% of available threads

        process = subprocess.run(["ninja", "-C./build", f'-j{wanted_threads}', f'-fbuild-{build_type}.ninja'])

        exit_code = process.returncode

    return exit_code

def get_help_description():
    return _globals.Help_Text('Build the project', '-bt=<build_type> -cc=<c_compiler> -cxx=<cpp_compiler>')

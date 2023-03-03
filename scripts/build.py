import _globals
import _helpers
import multiprocessing
import os, sys
import subprocess

def run():
    args = _helpers.process_args(sys.argv)

    build_type = _helpers.get_arg_value(args, ["build-type", "bt"], _globals.BUILD_TYPE)
    c_compiler = _helpers.get_arg_value(args, ["cc"], "clang")
    cxx_compiler = _helpers.get_arg_value(args, ["cxx", "cxx"], "clang++")

    exit_code = _globals.SUCCESS
    if not os.path.isfile("./build/build.ninja"):
        process = subprocess.run(["cmake", "-S .",  "-B ./build",  "-GNinja", f"-DCMAKE_C_COMPILER={c_compiler}", f"-DCMAKE_CXX_COMPILER={cxx_compiler}"])
        exit_code = process.returncode
    else:
        print("Using previous build.ninja")

    if exit_code == _globals.SUCCESS:
        wanted_threads = int(os.cpu_count() / 1.2) # use 80% of available threads
        build_args =f"-j {str(wanted_threads)}"

        process = subprocess.run(["ninja", "-C./build", build_args])
        exit_code = process.returncode

    return exit_code

def get_help_description():
    return _globals.Help_Text('Build the project', '-bt=<build_type> -cc=<c_compiler> -cxx=<cpp_compiler>')

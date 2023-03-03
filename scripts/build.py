import _globals
import _helpers
import multiprocessing
import subprocess
import sys
import os

args = _helpers.process_args(sys.argv)

build_type = _helpers.get_arg_value(args, ["build-type", "bt"], _globals.BUILD_TYPE)
c_compiler = _helpers.get_arg_value(args, ["compiler", "c"], "clang")
cxx_compiler = _helpers.get_arg_value(args, ["compiler", "cxx"], "clang++")

exit_code = 0
if not os.path.isfile("./build/build.ninja"):
    process = subprocess.run(["cmake", "-S .",  "-B ./build",  "-GNinja", f"-DCMAKE_C_COMPILER={c_compiler}", f"-DCMAKE_CXX_COMPILER={cxx_compiler}"])
    exit_code = process.returncode
else:
    print("Using previous build.ninja")

if exit_code == 0:
    wanted_threads = int(os.cpu_count() / 1.2) # use 80% of available threads
    build_args =f"-j {str(wanted_threads)}"

    process = subprocess.run(["ninja", "-C./build", build_args])
    exit_code = process.returncode

sys.exit(exit_code)

import subprocess
import sys

import _helpers
import _types


def run():
    args = _helpers.process_args(sys.argv)

    c_compiler = _helpers.get_arg_value(args, ['cc'], 'clang')
    cxx_compiler = _helpers.get_arg_value(args, ['cxx'], 'clang++')

    process = subprocess.run(['cmake', '-S .',  '-B ./build',  '-GNinja Multi-Config',
                             f'-DCMAKE_C_COMPILER={c_compiler}', f'-DCMAKE_CXX_COMPILER={cxx_compiler}'])
    exit_code = process.returncode

    return exit_code


def get_help_description():
    return _types.Help_Text('Configure the project', '-cc=<c_compiler> -cxx=<cpp_compiler>')

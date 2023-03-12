import importlib
import os
import sys

import _globals
import _helpers


def run():
    args = _helpers.process_args(sys.argv)

    info = _helpers.get_arg_value(args, ['info', 'i'], False)

    print('Valid commands: \n')
    for f in os.scandir('./scripts'):
        if f.is_file() and f.name.endswith('.py') and not f.name.startswith('_'):
            module_name = os.path.splitext(f.name)[0]
            module = importlib.import_module(module_name)

            help_text = module.get_help_description()

            print(module_name + ':')
            print('  ' + help_text.description)
            if info and len(help_text.params) > 0:
                print('    ' + help_text.params)

    return _globals.SUCCESS


def get_help_description():
    return _globals.Help_Text('Outputs command information', '-i (more information)')

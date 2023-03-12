from _types import Parsed_Args


def process_args(argv) -> Parsed_Args:
    dict = {}  # key:value dictionary

    for arg in argv[1:]:
        # key || key=value
        if arg.startswith('-'):
            try:
                k = arg[1:arg.index('=')]
                v = arg[arg.index('=') + 1:]
            except:
                k = arg[1:]
                v = True
            dict[k] = v

    return Parsed_Args(argv[0], argv, dict)


def get_arg_value(args: Parsed_Args, names, default):
    for name in names:
        if name in args.dict:
            return args.dict[name]

    return default

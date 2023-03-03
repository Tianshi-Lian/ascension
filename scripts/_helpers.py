from collections import namedtuple

ParsedArgs = namedtuple("ParsedArgs", "command original dict")

def process_args(argv) -> ParsedArgs:
    dict = {} # key:value dictionary

    for arg in argv[1:]:
        #key || key=value
        if arg.startswith('-'):
            try:
                k = arg[1:arg.index("=")]
                v = arg[arg.index("=") + 1:]
            except:
                k = arg
                v = 0
            dict[k] = v

    return ParsedArgs(argv[0], argv, dict)

def get_arg_value(args: ParsedArgs, names, default):
    for name in names:
        if name in args.dict:
            return args.dict[name]

    return default

import _globals
import distutils.dir_util

def run():
    # TODO: Remove this script and work out how to do exactly what we want in CMake.
    # /t    We definitely don't want to have these hard-coded paths in here as it removes any reusability of our scripts.
    # Copy all of erika plugins to the ascension executable directory
    plugin_dir='build/erika/plugins/bin'
    output_dir='build/ascension/bin'
    distutils.dir_util.copy_tree(plugin_dir, output_dir, update=True)

    return _globals.SUCCESS

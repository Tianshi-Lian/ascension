import _globals
import distutils.dir_util

def run():
    # Copy all of erika plugins to the ascension executable directory
    plugin_dir='build/erika/plugins/bin'
    output_dir='build/ascension/bin'
    distutils.dir_util.copy_tree(plugin_dir, output_dir, update=True)

    return _globals.SUCCESS

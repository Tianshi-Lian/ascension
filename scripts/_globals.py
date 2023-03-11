import sys
from collections import namedtuple

BUILD_TYPE = "Debug"
BUILD_PROJECT_NAME = "ascension"

SUCCESS = 0

PLATFORM = sys.platform

if "linux" in PLATFORM: # this could be linux or linux2
    PLATFORM = "linux"
elif "win" in PLATFORM:
    PLATFORM = "windows"

Help_Text = namedtuple('Help_Text', ['description', 'params'])

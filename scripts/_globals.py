import sys

BUILD_TYPE = "debug"
BUILD_PROJECT_NAME = "ascension"

PLATFORM = sys.platform

if "linux" in PLATFORM: # this could be linux or linux2
    PLATFORM = "linux"
elif "win" in PLATFORM:
    PLATFORM = "windows"

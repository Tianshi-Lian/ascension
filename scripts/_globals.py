import os
import sys

from dotenv import load_dotenv


load_dotenv()

BUILD_TYPE = os.getenv('BUILD_TYPE')
BUILD_PROJECT_NAME = os.getenv('PROJECT_NAME')

SUCCESS = 0

PLATFORM = \
    'linux' if 'linux' in sys.platform else \
    'windows' if 'win' in sys.platform else \
    'unknown'

import os
import sys

from dotenv import load_dotenv

SUCCESS = 0

PLATFORM = \
    'linux' if 'linux' in sys.platform else \
    'windows' if 'win' in sys.platform else \
    'unknown'

load_dotenv()

BUILD_TYPE = os.getenv(
    'BUILD_TYPE') if 'BUILD_TYPE' in os.environ else 'Debug'

BUILD_PROJECT_NAME = os.getenv(
    'PROJECT_NAME') if 'PROJECT_NAME' in os.environ else 'unknown'

C_COMPILER = os.getenv(
    'C_COMPILER') if 'BUILD_TYPE' in os.environ else 'clang'

CXX_COMPILER = os.getenv(
    'CXX_COMPILER') if 'CXX_COMPILER' in os.environ else 'clang++'

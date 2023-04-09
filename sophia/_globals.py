'''
  ' File: _globals.py
  ' Project: sophia
  ' File Created: 2023-03-03 19:15:53
  ' Author: Rob Graham (robgrahamdev@gmail.com)
  ' Last Modified: 2023-04-09 16:16:45
  ' ------------------
  ' Copyright 2023 Rob Graham
  ' ==================
  ' This program is free software: you can redistribute it and/or modify
  ' it under the terms of the GNU Affero General Public License as published
  ' by the Free Software Foundation, either version 3 of the License, or
  ' (at your option) any later version.
  '
  ' This program is distributed in the hope that it will be useful,
  ' but WITHOUT ANY WARRANTY; without even the implied warranty of
  ' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  ' GNU Affero General Public License for more details.
  '
  ' You should have received a copy of the GNU Affero General Public License
  ' along with this program.  If not, see <https://www.gnu.org/licenses/>.
  ' ==================
'''

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

# ----------------------------------------------------------------------------
# Copyright (C) 2018 Verizon.  All Rights Reserved.
# All Rights Reserved
#
#   Author: Reed P Morrison
#   Date:   09/30/2015  
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# ------------------------------------------------------------------------------
# Project setup
project(is2)
cmake_minimum_required(VERSION 2.8.0)
# ------------------------------------------------------------------------------
# build catch
# ------------------------------------------------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ../../ext)
add_library(compiled_catch STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/catch/catch.hpp _catch.cc)
# ------------------------------------------------------------------------------
# build tests
# ------------------------------------------------------------------------------
add_subdirectory(support)
add_subdirectory(url_router)
add_subdirectory(srvr)


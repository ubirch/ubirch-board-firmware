#!
# FRDM-K82F specific library cmake declarations
#
# @author Matthias L. Jugel
# @date 2016-05-03
#
# Copyright 2016 ubirch GmbH (https://ubirch.com)
#
# == LICENSE ==
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

add_library(board board.c clock_config.c)
target_include_directories(board PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(board ksdk13)

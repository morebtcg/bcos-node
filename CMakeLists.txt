#------------------------------------------------------------------------------
# Top-level CMake file for bcos-node
# ------------------------------------------------------------------------------
# Copyright (C) 2021 bcos-node
# SPDX-License-Identifier: Apache-2.0
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.10)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.3" CACHE STRING "Minimum OS X deployment version")

if (NOT DEFINED URL_BASE)
    set(URL_BASE "github.com")
    # set(URL_BASE "github.com.cnpmjs.org")
endif()

include(FetchContent)
set(BCOS_CMAKE_SCRIPTS_DIR ${CMAKE_CURRENT_BINARY_DIR}/bcos-cmake-scripts)
FetchContent_Declare(bcos-cmake-scripts
    GIT_REPOSITORY https://${URL_BASE}/FISCO-BCOS/bcos-cmake-scripts.git
    GIT_TAG dev
    SOURCE_DIR ${BCOS_CMAKE_SCRIPTS_DIR}
)
FetchContent_MakeAvailable(bcos-cmake-scripts)

# set(HUNTER_PACKAGE_LOG_BUILD ON CACHE STRING "Hunter output build info to log")
# set(HUNTER_PACKAGE_LOG_INSTALL ON CACHE STRING "Hunter output install info to log")
# set(HUNTER_STATUS_DEBUG OFF CACHE STRING "Hunter debug output")
# set(HUNTER_USE_CACHE_SERVERS NO CACHE STRING "Download cache from cache server" FORCE)
# set(HUNTER_STATUS_PRINT OFF CACHE STRING "Hunter status output")
# set(HUNTER_CONFIGURATION_TYPES Release CACHE STRING "Build type of Hunter packages")

set(BCOS_NODE_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake" CACHE PATH "The cmake path for the library")
list(APPEND CMAKE_MODULE_PATH ${BCOS_CMAKE_SCRIPTS_DIR})
list(APPEND CMAKE_MODULE_PATH ${BCOS_NODE_CMAKE_DIR})
set(HUNTER_CONFIGURATION_TYPES Release CACHE STRING "Build type of Hunter packages")
set(HUNTER_USE_CACHE_SERVERS NO CACHE STRING "Download cache from cache server" FORCE)
#init hunter
include(HunterGate)
HunterGate(
    URL "https://${URL_BASE}/FISCO-BCOS/hunter/archive/e427ab73280d9488bca736c53b8a52284896c759.tar.gz"
    SHA1 "cd5b639d31eb775a5adda5f4d4a19b027d97d174"
    FILEPATH "${BCOS_NODE_CMAKE_DIR}/config.cmake"
)
project(bcos-node VERSION "3.0.0")
set(VERSION_SUFFIX "")

# basic settings
include(ExternalProject)
include(Options)
configure_project()
include(CompilerSettings)
include(BuildInfoGenerator)

include_directories(${CMAKE_INSTALL_INCLUDEDIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#install dependencies
include(InstallBcosFrameworkDependencies)
include(InstallBcosCryptoDependencies)
include(InstallBcosTxPoolDependencies)
include(InstallBcosPBFTDependencies)
include(InstallBcosStorageDependencies)
include(InstallBcosLedgerDependencies)
include(InstallBcosSyncDependencies)
include(InstallBcosFrontDependencies)
include(InstallBcosGateWayDependencies)
include(InstallBcosDispatcherDependencies)
include(InstallBcosRpcDependencies)
include(ProjectBCOSExecutor)
include(InstallBcosTarsProtocolDependencies)

# copy .clang-format into the project path
include(CopyClangFormat)

if (DEFINED USE_PB)
    add_definitions(-DUSE_PB)
endif()

add_subdirectory(fisco-bcos)
add_subdirectory(libinitializer)

if (TESTS)
    enable_testing()
    set(CTEST_OUTPUT_ON_FAILURE TRUE)
endif()

# for doxygen:
include(BuildDocs)
buildDoc(bcos-node-doc)

# for code coverage
if (COVERAGE)
    include(Coverage)
    config_coverage("node-cov" "'/usr*' '${CMAKE_CURRENT_SOURCE_DIR}/bcos-cmake-scripts*' '${CMAKE_CURRENT_SOURCE_DIR}/test/bcos-test*'")
endif ()

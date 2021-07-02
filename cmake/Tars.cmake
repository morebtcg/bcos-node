cmake_minimum_required(VERSION 3.15)

set (DEPENDENCIES_DIR ${CMAKE_CURRENT_BINARY_DIR}/deps)

include(FetchContent)
FetchContent_Declare(bcos-tars-services
    GIT_REPOSITORY https://${URL_BASE}/FISCO-BCOS/bcos-tars-services.git
    GIT_TAG dev
    SOURCE_DIR ${DEPENDENCIES_DIR}/bcos-tars-services
)
FetchContent_Populate(bcos-tars-services)
include_directories(${DEPENDENCIES_DIR})

include(ExternalProject)
ExternalProject_Add(tarscpp
    GIT_REPOSITORY https://${URL_BASE}/TarsCloud/TarsCpp.git
    GIT_TAG release/2.4
    SOURCE_DIR ${DEPENDENCIES_DIR}/tarscpp
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_DIR}/tarscpp-install
    PATCH_COMMAND bash ${DEPENDENCIES_DIR}/bcos-tars-services/patchTars.sh ${DEPENDENCIES_DIR}/tarscpp ${DEPENDENCIES_DIR}/tarscpp-install
)
include_directories(${DEPENDENCIES_DIR}/tarscpp-install/include)
link_directories(${DEPENDENCIES_DIR}/tarscpp-install/lib)
set(TARS2CPP ${DEPENDENCIES_DIR}/tarscpp-install/tools/tars2cpp)

set(TARS_HEADER_DIR ${DEPENDENCIES_DIR}/tars-generated)

file(GLOB_RECURSE TARS_INPUT ${DEPENDENCIES_DIR}/bcos-tars-services/tars/*.tars)
if (TARS_INPUT)
    foreach(TARS_FILE ${TARS_INPUT})
        get_filename_component(TARS_NAME ${TARS_FILE} NAME_WE)
        get_filename_component(TARS_PATH ${TARS_FILE} PATH)

        add_custom_command(
            OUTPUT ${TARS_HEADER_DIR}/${TARS_NAME}.h
            WORKING_DIRECTORY ${TARS_PATH}
            COMMAND ${TARS2CPP} ${TARS_TOOL_FLAG} ${TARS_FILE} --dir=${TARS_HEADER_DIR}
            COMMENT "${TARS2CPP} ${TARS_TOOL_FLAG} ${TARS_FILE} --dir=${TARS_HEADER_DIR}"
            DEPENDS ${TARS2CPP} ${TARS_FILE}
        )
        list(APPEND OUT_TARS_H_LIST ${TARS_HEADER_DIR}/${TARS_NAME}.h)
    endforeach()
endif()

add_custom_target(bcostars ALL DEPENDS ${OUT_TARS_H_LIST})
add_dependencies(bcostars tarscpp)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${OUT_TARS_H_LIST}")

include_directories(${TARS_HEADER_DIR})

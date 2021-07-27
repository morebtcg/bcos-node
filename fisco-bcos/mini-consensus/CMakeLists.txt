file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

set(MINI_CONSENSUS_TARGET "mini-consensus")
add_executable(${MINI_CONSENSUS_TARGET} ${SRC_LIST} ${HEADERS})
find_package(Boost CONFIG QUIET REQUIRED unit_test_framework)
target_link_libraries(${MINI_CONSENSUS_TARGET} PUBLIC Boost::program_options Boost::unit_test_framework initializer)
target_compile_options(${MINI_CONSENSUS_TARGET} PRIVATE -Wno-error -Wno-unused-variable)
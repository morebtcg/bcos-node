file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

set(FISCO_BCOS_TARGET "fisco-bcos")
add_executable(${FISCO_BCOS_TARGET} ${SRC_LIST} ${HEADERS})
target_link_libraries(${FISCO_BCOS_TARGET} PUBLIC Boost::program_options initializer)
target_compile_options(${FISCO_BCOS_TARGET} PRIVATE -Wno-error -Wno-unused-variable)

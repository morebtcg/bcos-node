file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_executable(consensus-demo ${SRC_LIST} ${HEADERS})
find_package(Boost CONFIG QUIET REQUIRED unit_test_framework)
target_link_libraries(consensus-demo  PUBLIC bcos-framework::utilities bcos-framework::table bcos-framework::protocol bcos-framework::protocol-pb bcos-crypto::bcos-crypto bcos-txpool::bcos-txpool bcos-framework::sealer bcos-framework::sync bcos-pbft::bcos-consensus-core bcos-sync::block-sync bcos-pbft::bcos-pbft Boost::unit_test_framework)
file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(initializer ${SRC_LIST} ${HEADERS})

list(APPEND InitDeps bcos-executor::executor)

list(APPEND InitLibs bcos-gateway::bcos-gateway bcos-framework::utilities bcos-framework::tool bcos-framework::table
OpenSSL::SSL OpenSSL::Crypto bcos-framework::protocol-pb bcos-framework::sealer bcos-crypto::bcos-crypto
bcos-front::bcos-front bcos-storage::bcos-storage bcos-txpool::bcos-txpool bcos-pbft::bcos-pbft
bcos-sync::block-sync bcos-ledger::bcos-ledger bcos-dispatcher::dispatcher bcos-executor::executor)

if (DEFINED USE_TARS)
list(APPEND InitDeps bcostars tarscpp)
list(APPEND InitLibs tarsservant tarsutil)
target_compile_options(initializer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
endif()

add_dependencies(initializer ${InitDeps})
target_link_libraries(initializer PUBLIC ${InitLibs})

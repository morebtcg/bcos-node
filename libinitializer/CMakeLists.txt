file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(initializer ${SRC_LIST} ${HEADERS})
target_link_libraries(initializer PUBLIC bcos-gateway::bcos-gateway bcos-framework::utilities OpenSSL::SSL OpenSSL::Crypto bcos-framework::protocol-pb bcos-framework::sealer bcos-crypto::bcos-crypto bcos-front::bcos-front bcos-storage::bcos-storage bcos-txpool::bcos-txpool bcos-pbft::bcos-pbft bcos-sync::block-sync bcos-ledger::bcos-ledger)

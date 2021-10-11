# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
    URL https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/df18aa28dbfea3f1508b1a9009daacb301a6cc00.tar.gz
    SHA1 408cafd738427d72085aef2efc12b709bb81351d
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON HUNTER_KEEP_PACKAGE_SOURCES=ON  #DEBUG=ON
)

hunter_config(bcos-crypto
    VERSION 3.0.0-local-43df7523
    URL https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/255002b047b359a45c953d1dab29efd2ff6eb080.tar.gz
    SHA1 4d02de20be1f9bf79d762c5b8686368286504e07
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-tars-protocol
    VERSION 3.0.0-local
    URL https://${URL_BASE}/FISCO-BCOS/bcos-tars-protocol/archive/0b90f70b6a298874d9908b5bb501307b850fa135.tar.gz
    SHA1 06acb4fad6295f01024ba8330377cca347b3b38b
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/7bfcab8f2f0ed91ff4fe4d201af50f86ee919974.tar.gz"
    SHA1 f5762b63687847c4effef6ec67e6136c8272a64a
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-pbft VERSION 3.0.0-local-a2a9f7d2
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/2782b9cd502b668734bc770480aa5293aac3c564.tar.gz"
    SHA1 60eca1e79cb4c96bf580ec3d54a3e16561cbe9c2
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/003fc290d24b672416a4c847af8b391e8284d55a.tar.gz"
    SHA1 e4153b88709849e1ad0f87c068b72a9b91d8d8e3
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(rocksdb VERSION 6.20.3
	URL https://${URL_BASE}/facebook/rocksdb/archive/refs/tags/v6.20.3.tar.gz
    SHA1 64e4e6031820026c051d6e2072c0197e3bce1643
    CMAKE_ARGS WITH_TESTS=OFF WITH_GFLAGS=OFF WITH_BENCHMARK_TOOLS=OFF WITH_CORE_TOOLS=OFF
    WITH_TOOLS=OFF PORTABLE=ON FAIL_ON_WARNINGS=OFF WITH_ZSTD=ON BUILD_SHARED_LIBS=OFF
)

hunter_config(bcos-storage VERSION 3.0.0-local-900836da
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/e8325fadf450a469603167f0fe59091fdc305ac8.tar.gz"
    SHA1 8afa9dad4022dbfaf60fb05fcc4cd208d29486a3
    CMAKE_ARGS URL_BASE=${URL_BASE} CMAKE_BUILD_TYPE=Debug HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/cd7286d7dfd2680bc695fa308f0ebf2c85625ee9.tar.gz"
    SHA1 9b6aac0802fb8493b73d108afa19722f04b09069
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/10880d313161ae12e6e5d5bc63876fd274c4bb7c.tar.gz"
    SHA1 e9c0cd550f411a5f577f9e2e97ad395d502b2fb3
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/ec2642e55daf114d97272e354f24869a5575ab1b.tar.gz"
    SHA1 9cd6608f882c6d745a8f3af6390ed629d4df43ea
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-dispatcher VERSION 3.0.0-local-2903b298
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-dispatcher/archive/40fa82c68fc6ef27044ccdd80e9254109fb695ad.tar.gz"
    SHA1 cd0e15ecd4305ef993937a3dcf1a8b4524bc9c0b
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-rpc VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-rpc/archive/97f4256f6b98337d9d267195848f60d8bb7d4aec.tar.gz"
    SHA1 5c7170666e8756953643e4489bfec956d3d15af5
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

# hunter_config(bcos-executor VERSION 3.0.0-local-ac6d5d18
#     URL "https://${URL_BASE}/FISCO-BCOS/bcos-executor/archive/8a2dc4e32fc4de6a9a95ed2fbc83540e7ce589b4.tar.gz"
#     SHA1 c8254cd8a943ea21ab2bdbacd5e54d50ad20f778
#     CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON DEBUG=ON
# )

hunter_config(evmc VERSION 7.3.0-c7feb13f
	URL https://${URL_BASE}/FISCO-BCOS/evmc/archive/c7feb13f582919242da9f4f898ed4578785c9ecc.tar.gz
	SHA1 28ab1c74dd3340efe101418fd5faf19d34c9f7a9
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(intx VERSION 0.4.1 URL https://${URL_BASE}/chfast/intx/archive/v0.4.0.tar.gz
    SHA1 8a2a0b0efa64899db972166a9b3568a6984c61bc
	CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++17
)

hunter_config(ethash VERSION 0.7.0-4576af36 URL https://${URL_BASE}/chfast/ethash/archive/4576af36f8ebb9bee2d5f04be692f295c64a7211.tar.gz
	SHA1 2001a265177c722b4cbe91c4160f3f582e0c9938
	CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++17
)

hunter_config(evmone VERSION 0.4.1-b726a1e1
	URL https://${URL_BASE}/FISCO-BCOS/evmone/archive/b726a1e1722109291ac18bc7c5fad5aac9d2e8c5.tar.gz
	SHA1 e41fe0514a7a49a9a5e7eeb1b42cf2c3ced67f5d
	CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++17 BUILD_SHARED_LIBS=OFF
)

hunter_config(
    Boost
    VERSION ${HUNTER_Boost_VERSION}
    CMAKE_ARGS
    CONFIG_MACRO=BOOST_UUID_RANDOM_PROVIDER_FORCE_POSIX
)

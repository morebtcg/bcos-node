# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/ef4559ced68cd96fef8baf084552dee5b9150e00.tar.gz"
    SHA1 f047458e57adb68722a976e63d6d3c71eb7c1495
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON #DEBUG=ON
)
hunter_config(bcos-crypto VERSION 3.0.0-local-43df7523
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/25c8edb7d5cbadb514bbce9733573c8ffdb3600d.tar.gz"
    SHA1 4a1649e7095f5db58a5ae0671b2278bcccc25f1d
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/0a0f64d26b7f688828ff928ad90aed6825e04b66.tar.gz"
    SHA1 9a0dab8858c3e4883d2387e97eb7f72e7297a832
)

hunter_config(bcos-pbft VERSION 3.0.0-local-bddd4b4e
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/9bc7f9acf32316f13e642c527e74e8bb8773ef6b.tar.gz"
    SHA1 d0d881b90f58e9bb5dc4089380472f57744a3b27
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/9de6ed5b76c220f140b340648bd876bde3024420.tar.gz"
    SHA1 b7bc4a9987fd536b3c8d92c9a858585303f97137
)

hunter_config(bcos-storage VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/b83313aac5fab7e0420c8b5777b3ca0a2342f436.tar.gz"
    SHA1 95ad73a304608a9f2022c34f0066f9b7975c606a
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/8cf0922ae5a0f0bb95ded2991f4e79ec92387e12.tar.gz"
    SHA1 c36f39055f43b93e351af024b0b7c429c0c4414d
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/15e18804aab90def4c2ba7d811024df921f935de.tar.gz"
    SHA1 1485c64a31b106f912aa1d1878da5d91dc0a2975
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/dee049993d276a238072cb24a3d98b038c4e8137.tar.gz"
    SHA1 d5e2661b6d0789f6bfa98ef0585c514c4abc4b42
)

hunter_config(bcos-dispatcher VERSION 3.0.0-local-2903b298
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-dispatcher/archive/4fbc714b9ce38da0b8bc3a1e051dde53563113e3.tar.gz"
    SHA1 6d027b862f5b39f94e6fc68aa8234d6723f3b6f6
)

hunter_config(bcos-executor VERSION 3.0.0-local-ac6d5d18
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-executor/archive/ac6d5d18bddfee86bcc41bedc5636bf7e11dc02e.tar.gz"
    SHA1 69bfbeeb058f07fc7af4139b4e6923c533a78305
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON #DEBUG=ON
)

hunter_config(evmc VERSION 7.3.0-d951b1ef
		URL https://${URL_BASE}/FISCO-BCOS/evmc/archive/d951b1ef088be6922d80f41c3c83c0cbd69d2bfa.tar.gz
		SHA1 0b39b36cd8533c89ee0b15b6e94cff1111383ac7
)

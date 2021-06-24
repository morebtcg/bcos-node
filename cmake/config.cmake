# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/75f19d3b9a745c2154d457dfe2bb3350ec606276.tar.gz"
    SHA1 9645f0c8f044dac18941a4a948193a61792878d9
)
hunter_config(bcos-crypto VERSION 3.0.0-local-43df7523
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/f350ea889a0ad44b7efbd528d4829446b80e9665.tar.gz"
    SHA1 692989c6369d7085559f48264894b115241e8dc7
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/e7759806f4f4449effa2a6ab7b466c530d75b292.tar.gz"
    SHA1 22703ecda6580144aabd9142652e9564d5e84bae
)

hunter_config(bcos-pbft VERSION 3.0.0-local-bddd4b4e
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/945bd962b52b50d310b10886341c33ced1088e53.tar.gz"
    SHA1 7d9e7b129a806d3f7dcec170b1ff209ce7625e68
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/9f91070404f47c95ca18b6d3a22b6260576ed7c5.tar.gz"
    SHA1 6eaa3649b8dd44bba5061ad66bdc7092d47a0fd7
)

hunter_config(bcos-storage VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/77f3a29304526d6e804b398ac3b72510bcabd1c9.tar.gz"
    SHA1 20a7963b47ed3bc8bc3ed06edce484c26ea9acb0
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/636232c956edcb7d2897f3e9053761d60abbba10.tar.gz"
    SHA1 32041436db8e1d46222f92150ed3beb7ff816aa8
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/599e6ed3ca5da17e56f8c051915481785f586c87.tar.gz"
    SHA1 ad83f5e099cedb5eecec38f6549886bb7e61fbfc
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/95e3f51eb01b587415dfe8138dace0720450baa1.tar.gz"
    SHA1 c33df53d62c6b681a355be407d0765822878133e
)

hunter_config(bcos-dispatcher VERSION 3.0.0-local-2903b298
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-dispatcher/archive/c9983a45a8504fbd27096052a94de1d732f10484.tar.gz"
    SHA1 60eb5978ffa75ed7e4308d6c74eb5dcc850e612c
)

hunter_config(bcos-executor VERSION 3.0.0-local-d02a7649
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-executor/archive/153a50bbffce4c9b3f99a088e6843b37f853be04.tar.gz"
    SHA1 8c39ba1bf14f0bae61e1971b0690420ff2f188d4
)

hunter_config(evmc VERSION 7.3.0-d951b1ef
		URL https://${URL_BASE}/FISCO-BCOS/evmc/archive/d951b1ef088be6922d80f41c3c83c0cbd69d2bfa.tar.gz
		SHA1 0b39b36cd8533c89ee0b15b6e94cff1111383ac7
)

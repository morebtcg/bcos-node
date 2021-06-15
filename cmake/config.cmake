# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework
VERSION 3.0.0-local
URL "https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/62642fa202886e1470d78ad521002b36a2e86160.tar.gz"
SHA1 8eb63a8919041e8d2c432dcbc626d6c34934b5ae
)
hunter_config(bcos-crypto
VERSION 3.0.0-local-43df7523
URL "https://${URL_BASE}/cyjseagull/bcos-crypto/archive/01fc4acce648539dfa966d50d5b95cb92196d02b.tar.gz"
SHA1 9cc926c88855c4e4e6b957ce1bb8b24a26e2e94d
)

hunter_config(bcos-txpool
VERSION 3.0.0-local-beda0a00
URL "https://${URL_BASE}/cyjseagull/bcos-txpool/archive/bea94291ddaec41ec58af9423284a1391f6156b6.tar.gz"
SHA1 f8d989dea1443e9d6c06a0ac22a0bbdadff2f496
)

hunter_config(bcos-pbft
VERSION 3.0.0-local-bddd4b4e
URL "https://${URL_BASE}/cyjseagull/bcos-pbft/archive/0a00ac3181b93ee2359e7d860f4349799b0d9164.tar.gz"
SHA1 ff1f66eb0148388a5a89f6505fc7bff53572a67f
)

hunter_config(bcos-sync
VERSION 3.0.0-local-50e0e264
URL "https://${URL_BASE}/cyjseagull/bcos-sync/archive/7de92c842229a39a62522884d3d38119f756b733.tar.gz"
SHA1 563585a469a3a99039ada24a834cc98d652388cf
)

hunter_config(bcos-storage
VERSION 3.0.0-local
URL "https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/feb23a8e4a3c767ecece3a135b5d0dbfb27d4ee4.tar.gz"
SHA1 cd2842833ec1167056073b01b8fdd71bca2554af
)

hunter_config(bcos-ledger
VERSION 3.0.0-local-1956c515f
URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/1efb2b67375fda2acd8c05c7166763ac226476d4.tar.gz"
SHA1 55fcd1de9321037fe5540400faeda7b3829bc040
)

hunter_config(bcos-front
VERSION 3.0.0-local-2ed687bb
URL "https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/6b0bb4b3adfb431b9ce2f23a67d12cde3890f00d.tar.gz"
SHA1 d5f5ac604b8898df7f6cd2bb008991bbc39b98fb
)

hunter_config(bcos-gateway
VERSION 3.0.0-local-1fb592e4
URL "https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/453ba072cdfed4b985fd1600a1ce78311420816b.tar.gz"
SHA1 60795576836e872de9c80917cf110d33f143db5c
)
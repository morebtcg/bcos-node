# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework
VERSION 3.0.0-local
URL "https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/8719bff6d78f9f5523e45c30ebe18806c87d25d1.tar.gz"
SHA1 39f1f520c0afeda92d365417d06344ec3024cd03
)
hunter_config(bcos-crypto
VERSION 3.0.0-local-43df7523
URL "https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/43df75238d143b9c892b5810f10997841ed01ef3.tar.gz"
SHA1 0d041d392a113339b0e2f2bbc7a7ba9e326c99d3
)

hunter_config(bcos-txpool
VERSION 3.0.0-local-beda0a00
URL "https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/3f52f2a166b751378fc1fe29dd5fb7c0e88bcedd.tar.gz"
SHA1 eb50442ab0478d139f452ae2d99a47fb1cd90618
)

hunter_config(bcos-pbft
VERSION 3.0.0-local-bddd4b4e
URL "https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/4c1901b867e3cfeed1bd1ae46bc2ae5b1862d003.tar.gz"
SHA1 aa6060a55ad33345d3f7d856b19a08352aa900a7
)

hunter_config(bcos-sync
VERSION 3.0.0-local-50e0e264
URL "https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/b31fc58aac6d1ec1998bc474402be683bb9b382d.tar.gz"
SHA1 f83a9977c2443dcf74112b225a94fd3c64587cd9
)

hunter_config(bcos-ledger
VERSION 3.0.0-local-1956c515f
URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/1956c515fbcd68cdd6721173d77c9d19f41ed846.tar.gz"
SHA1 d1c3814b72c5648e656c15ab48bf8a8ce7fa8e43
)

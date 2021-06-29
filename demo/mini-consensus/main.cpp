/**
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @brief demo for PBFT integrated with the network module
 * @file main.cpp
 * @author: yujiechen
 * @date 2021-06-10
 */
#include "demo/utilities/Common.h"
#include "libinitializer/Initializer.h"
#include <bcos-framework/testutils/protocol/FakeTransaction.h>
using namespace bcos;
using namespace bcos::initializer;
using namespace bcos::crypto;
using namespace bcos::node;
using namespace bcos::protocol;

inline bytes fakeHelloWorldSet()
{
    return *fromHexString(
        "0x4ed3885e00000000000000000000000000000000000000000000000000000000000000200000000000000000"
        "000000000000000000000000000000000000000000000005666973636f00000000000000000000000000000000"
        "0000000000000000000000");
}

inline bytes fakeHelloWorldDeployInput()
{
    std::string helloBin =
        "0x60806040526040805190810160405280600181526020017f3100000000000000000000000000000000000000"
        "0000000000000000000000008152506001908051906020019061004f9291906100ae565b5034801561005c5760"
        "0080fd5b506040805190810160405280600d81526020017f48656c6c6f2c20576f726c64210000000000000000"
        "0000000000000000000000815250600090805190602001906100a89291906100ae565b50610153565b82805460"
        "0181600116156101000203166002900490600052602060002090601f016020900481019282601f106100ef5780"
        "5160ff191683800117855561011d565b8280016001018555821561011d579182015b8281111561011c57825182"
        "5591602001919060010190610101565b5b50905061012a919061012e565b5090565b61015091905b8082111561"
        "014c576000816000905550600101610134565b5090565b90565b6104ac806101626000396000f3006080604052"
        "60043610610057576000357c0100000000000000000000000000000000000000000000000000000000900463ff"
        "ffffff1680634ed3885e1461005c57806354fd4d50146100c55780636d4ce63c14610155575b600080fd5b3480"
        "1561006857600080fd5b506100c3600480360381019080803590602001908201803590602001908080601f0160"
        "208091040260200160405190810160405280939291908181526020018383808284378201915050505050509192"
        "9192905050506101e5565b005b3480156100d157600080fd5b506100da61029b565b6040518080602001828103"
        "825283818151815260200191508051906020019080838360005b8381101561011a578082015181840152602081"
        "0190506100ff565b50505050905090810190601f1680156101475780820380516001836020036101000a031916"
        "815260200191505b509250505060405180910390f35b34801561016157600080fd5b5061016a610339565b6040"
        "518080602001828103825283818151815260200191508051906020019080838360005b838110156101aa578082"
        "01518184015260208101905061018f565b50505050905090810190601f1680156101d757808203805160018360"
        "20036101000a031916815260200191505b509250505060405180910390f35b80600090805190602001906101fb"
        "9291906103db565b507f93a093529f9c8a0c300db4c55fcd27c068c4f5e0e8410bc288c7e76f3d71083e816040"
        "518080602001828103825283818151815260200191508051906020019080838360005b8381101561025e578082"
        "015181840152602081019050610243565b50505050905090810190601f16801561028b57808203805160018360"
        "20036101000a031916815260200191505b509250505060405180910390a150565b600180546001816001161561"
        "01000203166002900480601f016020809104026020016040519081016040528092919081815260200182805460"
        "0181600116156101000203166002900480156103315780601f1061030657610100808354040283529160200191"
        "610331565b820191906000526020600020905b81548152906001019060200180831161031457829003601f1682"
        "01915b505050505081565b606060008054600181600116156101000203166002900480601f0160208091040260"
        "200160405190810160405280929190818152602001828054600181600116156101000203166002900480156103"
        "d15780601f106103a6576101008083540402835291602001916103d1565b820191906000526020600020905b81"
        "54815290600101906020018083116103b457829003601f168201915b5050505050905090565b82805460018160"
        "0116156101000203166002900490600052602060002090601f016020900481019282601f1061041c57805160ff"
        "191683800117855561044a565b8280016001018555821561044a579182015b8281111561044957825182559160"
        "200191906001019061042e565b5b509050610457919061045b565b5090565b61047d91905b8082111561047957"
        "6000816000905550600101610461565b5090565b905600a165627a7a723058204736027ad6b97d7cd2685379ac"
        "b35b386dcb18799934be8283f1e08cd1f0c6ec0029";
    return *fromHexString(helloBin);
}

inline void createAndSubmitTx(Initializer::Ptr _initializer, float txSpeed)
{
    auto cryptoSuite = _initializer->protocolInitializer()->cryptoSuite();
    auto keyPair = cryptoSuite->signatureImpl()->generateKeyPair();

    uint16_t sleepInterval = (uint16_t)(1000.0 / txSpeed);
    auto blockLimit = _initializer->nodeConfig()->blockLimit() / 2;
    auto chainId = _initializer->nodeConfig()->chainId();
    auto groupId = _initializer->nodeConfig()->groupId();

    auto txpool = _initializer->pbftInitializer()->txpool();
    auto ledger = _initializer->ledgerInitializer()->ledger();
    std::atomic<BlockNumber> blockNumber = {0};
    u256 nonce = utcTime();
    ledger->asyncGetBlockNumber([&](Error::Ptr _error, BlockNumber _blockNumber) {
        if (_error)
        {
            return;
        }
        blockNumber = _blockNumber;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // deploy helloworld
    auto helloWorldDeployInput = fakeHelloWorldDeployInput();
    size_t txsNum = 0;
    while (true)
    {
        try
        {
            if (txsNum % 500 == 0)
            {
                ledger->asyncGetBlockNumber([&](Error::Ptr _error, BlockNumber _blockNumber) {
                    if (_error)
                    {
                        return;
                    }
                    blockNumber = _blockNumber;
                });
            }
            int64_t txBlockLimit = blockNumber + blockLimit;
            Transaction::Ptr tx;
#if 0
            if (txsNum % 10000 == 0)
            {
                std::string to = "0x1003";
                tx = bcos::test::fakeTransaction(cryptoSuite, keyPair, bytes(to.begin(), to.end()),
                    helloWorldDeployInput, nonce, txBlockLimit, chainId, groupId);
            }
            else
            {
                tx = bcos::test::fakeTransaction(cryptoSuite, keyPair, bytes(),
                    helloWorldDeployInput, nonce, txBlockLimit, chainId, groupId);
            }
#endif
            tx = bcos::test::fakeTransaction(cryptoSuite, keyPair, bytes(), helloWorldDeployInput,
                nonce, txBlockLimit, chainId, groupId);
            auto encodedTxData = tx->encode();
            auto txData = std::make_shared<bytes>(encodedTxData.begin(), encodedTxData.end());
            txpool->asyncSubmit(
                txData, [tx](Error::Ptr _error, TransactionSubmitResult::Ptr _result) {
                    if (_error == nullptr)
                    {
                        BCOS_LOG(TRACE) << LOG_DESC("submit transaction success")
                                        << LOG_KV("hash", tx->hash().abridged())
                                        << LOG_KV("status", _result->status());
                        return;
                    }
                    BCOS_LOG(TRACE) << LOG_DESC("submit transaction failed")
                                    << LOG_KV("code", _error->errorCode())
                                    << LOG_KV("msg", _error->errorMessage());
                });
            nonce = tx->nonce() + utcTimeUs();
            txsNum++;
        }
        catch (std::exception const& e)
        {
            BCOS_LOG(TRACE) << LOG_DESC("submit tx failed")
                            << LOG_KV("error", boost::diagnostic_information(e));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepInterval));
    }
}

int main(int argc, const char* argv[])
{
    auto param = initCommandLine(argc, argv);
    auto initializer = std::make_shared<Initializer>();
    printVersion();
    std::cout << "[" << getCurrentDateTime() << "] ";
    std::cout << "The mini-consensus is running..." << std::endl;

    initializer->init(param.configFilePath, param.genesisFilePath);
    initializer->start();
    createAndSubmitTx(initializer, param.txSpeed);

    std::cout << "[" << getCurrentDateTime() << "] ";
    std::cout << "mini-consensus program exit normally." << std::endl;
}
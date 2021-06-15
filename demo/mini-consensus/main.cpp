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

inline void createAndSubmitTx(Initializer::Ptr _initializer, float txSpeed)
{
    auto cryptoSuite = _initializer->protocolInitializer()->cryptoSuite();
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
    while (true)
    {
        try
        {
            ledger->asyncGetBlockNumber([&](Error::Ptr _error, BlockNumber _blockNumber) {
                if (_error)
                {
                    return;
                }
                blockNumber = _blockNumber;
            });
            int64_t txBlockLimit = blockNumber + blockLimit;
            auto tx =
                bcos::test::fakeTransaction(cryptoSuite, nonce, txBlockLimit, chainId, groupId);
            auto encodedTxData = tx->encode();
            auto txData = std::make_shared<bytes>(encodedTxData.begin(), encodedTxData.end());
            txpool->asyncSubmit(
                txData,
                [tx](Error::Ptr _error, TransactionSubmitResult::Ptr _result) {
                    if (_error == nullptr)
                    {
                        LOG(DEBUG) << LOG_DESC("submit transaction success")
                                   << LOG_KV("hash", tx->hash().abridged())
                                   << LOG_KV("status", _result->status());
                        return;
                    }
                    LOG(DEBUG) << LOG_DESC("submit transaction failed")
                               << LOG_KV("code", _error->errorCode())
                               << LOG_KV("msg", _error->errorMessage());
                },
                [](Error::Ptr _error) {
                    if (_error == nullptr)
                    {
                        return;
                    }
                    LOG(DEBUG) << LOG_DESC("submit transaction exception")
                               << LOG_KV("code", _error->errorCode())
                               << LOG_KV("msg", _error->errorMessage());
                });
            nonce = utcTime() + tx->nonce();
        }
        catch (std::exception const& e)
        {
            LOG(TRACE) << LOG_DESC("submit tx failed")
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
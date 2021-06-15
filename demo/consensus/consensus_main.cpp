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
 * @brief demo for PBFT
 * @file consensus_main.cpp
 * @author: yujiechen
 * @date 2021-06-04
 */
#include "Common.h"
#include <bcos-framework/interfaces/crypto/CryptoSuite.h>
#include <bcos-framework/libprotocol/TransactionSubmitResultFactoryImpl.h>
#include <bcos-framework/libprotocol/protobuf/PBBlockFactory.h>
#include <bcos-framework/libprotocol/protobuf/PBBlockHeaderFactory.h>
#include <bcos-framework/libprotocol/protobuf/PBTransactionFactory.h>
#include <bcos-framework/libprotocol/protobuf/PBTransactionReceiptFactory.h>
#include <bcos-framework/libsealer/SealerFactory.h>
#include <bcos-framework/testutils/faker/FakeDispatcher.h>
#include <bcos-framework/testutils/faker/FakeFrontService.h>
#include <bcos-framework/testutils/faker/FakeStorage.h>
#include <bcos-txpool/TxPool.h>

using namespace bcos;
using namespace bcos::crypto;
using namespace bcos::protocol;
using namespace bcos::consensus;
using namespace bcos::txpool;
using namespace bcos::dispatcher;
using namespace bcos::storage;
using namespace bcos::node;
using namespace bcos::front;
using namespace bcos::test;
using namespace bcos::sealer;
using namespace bcos::sync;
using namespace std;

inline NodeObjects::Ptr createNode(CryptoSuite::Ptr _cryptoSuite, BlockFactory::Ptr _blockFactory,
    FakeGateWay::Ptr _gateWay, std::string const& _groupId, std::string const& _chainId,
    int64_t _blockLimit, unsigned _minSealTime, size_t _txCountLimit, unsigned _consensusTimeout,
    bool _connected = true)
{
    auto keyPair = _cryptoSuite->signatureImpl()->generateKeyPair();
    // create the faked network
    auto frontService = std::make_shared<FakeFrontService>(keyPair->publicKey());
    frontService->setGateWay(_gateWay);
    // create the faked storage
    auto storage = std::make_shared<FakeStorage>();
    // create faked dispatcher
    auto dispatcher = std::make_shared<FakeDispatcher>();

    // create the ledger(with same genesis block among all nodes)
    auto ledger = std::make_shared<FakeLedger>(_blockFactory, 0, 0, 0, vector<bytes>());
    ledger->setSystemConfig(SYSTEM_KEY_CONSENSUS_TIMEOUT, std::to_string(_consensusTimeout));
    ledger->setSystemConfig(SYSTEM_KEY_TX_COUNT_LIMIT, std::to_string(_txCountLimit));
    ledger->ledgerConfig()->setConsensusTimeout(_consensusTimeout * 1000);
    ledger->ledgerConfig()->setBlockTxCountLimit(_txCountLimit);
    // create txpool
    auto txResultFactory = std::make_shared<TransactionSubmitResultFactoryImpl>();
    auto txpoolFactory = std::make_shared<TxPoolFactory>(keyPair->publicKey(), _cryptoSuite,
        txResultFactory, _blockFactory, frontService, ledger, _groupId, _chainId, _blockLimit);
    // create sealer
    auto sealerFactory =
        std::make_shared<SealerFactory>(_blockFactory, txpoolFactory->txpool(), _minSealTime);

    // create pbft
    auto pbftFactory = std::make_shared<PBFTFactory>(_cryptoSuite, keyPair, frontService, storage,
        ledger, txpoolFactory->txpool(), sealerFactory->sealer(), dispatcher, _blockFactory,
        txResultFactory);

    // create sync
    auto blockSyncFactory = std::make_shared<BlockSyncFactory>(keyPair->publicKey(), _blockFactory,
        ledger, frontService, dispatcher, pbftFactory->consensus());
    if (_connected)
    {
        _gateWay->addConsensusInterface(keyPair->publicKey(), pbftFactory->consensus());
        _gateWay->addTxPool(keyPair->publicKey(), txpoolFactory->txpool());
    }
    return std::make_shared<NodeObjects>(
        pbftFactory, txpoolFactory, ledger, sealerFactory, blockSyncFactory);
}

int main()
{
    auto cryptoSuite = createCryptoSuite(false);
    auto blockFactory = createBlockFactory(cryptoSuite);
    auto fakeGateWay = std::make_shared<FakeGateWay>();
    std::string groupId = "testGroup";
    std::string chainId = "testChain";
    int64_t blockLimit = 1000;
    unsigned minSealTime = 500;
    size_t txCountLimit = 1000;
    unsigned consensusTimeout = 3;
    size_t nodeSize = 4;
    std::vector<NodeObjects::Ptr> nodeList;
    std::vector<PublicPtr> consensusNodeIdList;
    NodeIDSet connectedNodeList;
    for (size_t i = 0; i < nodeSize; i++)
    {
        auto nodeConfig = createNode(cryptoSuite, blockFactory, fakeGateWay, groupId, chainId,
            blockLimit, minSealTime, txCountLimit, consensusTimeout, true);
        nodeList.push_back(nodeConfig);
        auto nodeId = nodeConfig->pbftFactory()->pbftConfig()->keyPair()->publicKey();
        consensusNodeIdList.push_back(nodeId);
        connectedNodeList.insert(nodeId);
    }
    // init consensusNodeList
    for (size_t i = 0; i < nodeSize; i++)
    {
        auto nodeConfig = nodeList[i];
        appendConsensusNodeList(nodeConfig->ledger(), consensusNodeIdList);
        auto txpool = std::dynamic_pointer_cast<TxPool>(nodeConfig->txpoolFactory()->txpool());
        txpool->transactionSync()->config()->setConnectedNodeList(connectedNodeList);
        initAndStartNode(nodeConfig);
    }
    createAndSubmitTx(cryptoSuite, nodeList, blockLimit, groupId, chainId);
    return 0;
}

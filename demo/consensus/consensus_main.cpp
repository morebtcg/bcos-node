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
#include <bcos-crypto/hash/Keccak256.h>
#include <bcos-crypto/hash/SM3.h>
#include <bcos-crypto/signature/secp256k1/Secp256k1Crypto.h>
#include <bcos-crypto/signature/sm2/SM2Crypto.h>
#include <bcos-framework/interfaces/consensus/ConsensusNode.h>
#include <bcos-framework/interfaces/crypto/CryptoSuite.h>
#include <bcos-framework/libprotocol/TransactionSubmitResultFactoryImpl.h>
#include <bcos-framework/libprotocol/protobuf/PBBlockFactory.h>
#include <bcos-framework/libprotocol/protobuf/PBBlockHeaderFactory.h>
#include <bcos-framework/libprotocol/protobuf/PBTransactionFactory.h>
#include <bcos-framework/libprotocol/protobuf/PBTransactionReceiptFactory.h>
#include <bcos-framework/libsealer/SealerFactory.h>
#include <bcos-framework/testutils/faker/FakeDispatcher.h>
#include <bcos-framework/testutils/faker/FakeFrontService.h>
#include <bcos-framework/testutils/faker/FakeLedger.h>
#include <bcos-framework/testutils/faker/FakeStorage.h>
#include <bcos-pbft/pbft/PBFTFactory.h>
#include <bcos-sync/BlockSyncFactory.h>
#include <bcos-txpool/TxPool.h>
#include <bcos-txpool/TxPoolFactory.h>

using namespace bcos;
using namespace bcos::crypto;
using namespace bcos::protocol;
using namespace bcos::consensus;
using namespace bcos::txpool;
using namespace bcos::dispatcher;
using namespace bcos::storage;
using namespace bcos::front;
using namespace bcos::test;
using namespace bcos::sealer;
using namespace bcos::sync;
using namespace std;

inline CryptoSuite::Ptr createCryptoSuite(bool _sm)
{
    Hash::Ptr hashImpl;
    SignatureCrypto::Ptr signatureImpl;
    if (!_sm)
    {
        hashImpl = std::make_shared<Keccak256>();
        signatureImpl = std::make_shared<Secp256k1Crypto>();
    }
    else
    {
        hashImpl = std::make_shared<SM3>();
        signatureImpl = std::make_shared<SM2Crypto>();
    }
    return std::make_shared<CryptoSuite>(hashImpl, signatureImpl, nullptr);
}

class NodeConfig
{
public:
    using Ptr = std::shared_ptr<NodeConfig>;
    NodeConfig(PBFTFactory::Ptr _pbftFactory, TxPoolFactory::Ptr _txpoolFactory,
        LedgerInterface::Ptr _ledger, std::shared_ptr<SealerFactory> _sealerFactory,
        BlockSyncFactory::Ptr _blockSyncFactory)
      : m_pbftFactory(_pbftFactory),
        m_txpoolFactory(_txpoolFactory),
        m_ledger(_ledger),
        m_sealerFactory(_sealerFactory),
        m_blockSyncFactory(_blockSyncFactory)
    {}

    PBFTFactory::Ptr pbftFactory() { return m_pbftFactory; }
    TxPoolFactory::Ptr txpoolFactory() { return m_txpoolFactory; }
    LedgerInterface::Ptr ledger() { return m_ledger; }
    std::shared_ptr<SealerFactory> sealerFactory() { return m_sealerFactory; }

    BlockSyncFactory::Ptr blockSyncFactory() { return m_blockSyncFactory; }

private:
    PBFTFactory::Ptr m_pbftFactory;
    TxPoolFactory::Ptr m_txpoolFactory;
    LedgerInterface::Ptr m_ledger;
    std::shared_ptr<SealerFactory> m_sealerFactory;
    BlockSyncFactory::Ptr m_blockSyncFactory;
};
inline NodeConfig::Ptr createNode(CryptoSuite::Ptr _cryptoSuite, BlockFactory::Ptr _blockFactory,
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
    return std::make_shared<NodeConfig>(
        pbftFactory, txpoolFactory, ledger, sealerFactory, blockSyncFactory);
}

inline void appendConsensusNodeList(LedgerInterface::Ptr _ledger, std::vector<PublicPtr>& _nodeList)
{
    auto ledger = std::dynamic_pointer_cast<FakeLedger>(_ledger);
    auto ledgerConfig = ledger->ledgerConfig();
    for (auto node : _nodeList)
    {
        auto nodeInfo = std::make_shared<ConsensusNode>(node, 1);
        ledgerConfig->mutableConsensusNodeList().push_back(nodeInfo);
    }
}

void initAndStartNode(NodeConfig::Ptr _nodeConfig, NodeIDSet const& _connectedNodes)
{
    auto pbftFactory = _nodeConfig->pbftFactory();
    auto sealer = pbftFactory->pbftConfig()->sealer();
    auto pbft = pbftFactory->consensus();
    auto txpool = std::dynamic_pointer_cast<TxPool>(_nodeConfig->txpoolFactory()->txpool());
    // init txpool
    _nodeConfig->txpoolFactory()->init(sealer);
    // init sealer
    _nodeConfig->sealerFactory()->init(pbft);
    // init PBFT
    _nodeConfig->blockSyncFactory()->init();
    pbftFactory->init(_nodeConfig->blockSyncFactory()->sync());

    // start txpool
    txpool->transactionSync()->config()->setConnectedNodeList(_connectedNodes);
    txpool->start();
    // start sealer
    sealer->start();
    // start PBFT
    pbft->start();
}

inline void createAndSubmitTx(CryptoSuite::Ptr _cryptoSuite, std::vector<NodeConfig::Ptr> _nodeList,
    int64_t _blockLimit, std::string const& _groupId, std::string const& _chainId)
{
    u256 nonce = utcTime();
    size_t txsNum = 0;
    while (true)
    {
        auto selectedNode = _nodeList[(txsNum % _nodeList.size())];
        auto ledger = std::dynamic_pointer_cast<FakeLedger>(selectedNode->ledger());
        int64_t blockLimit = ledger->blockNumber() + _blockLimit / 2;
        auto tx = fakeTransaction(_cryptoSuite, nonce, blockLimit, _chainId, _groupId);
        auto encodedTxData = tx->encode();
        auto txData = std::make_shared<bytes>(encodedTxData.begin(), encodedTxData.end());
        selectedNode->txpoolFactory()->txpool()->asyncSubmit(
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
        txsNum++;
        nonce++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
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
    std::vector<NodeConfig::Ptr> nodeList;
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
        initAndStartNode(nodeConfig, connectedNodeList);
    }
    createAndSubmitTx(cryptoSuite, nodeList, blockLimit, groupId, chainId);
    return 0;
}

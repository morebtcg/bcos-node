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
 * @file Common.h
 * @author: yujiechen
 * @date 2021-06-10
 */
#pragma once
#include <bcos-crypto/hash/Keccak256.h>
#include <bcos-crypto/hash/SM3.h>
#include <bcos-crypto/signature/secp256k1/Secp256k1Crypto.h>
#include <bcos-crypto/signature/sm2/SM2Crypto.h>
#include <bcos-framework/interfaces/consensus/ConsensusNode.h>
#include <bcos-framework/interfaces/ledger/LedgerInterface.h>
#include <bcos-framework/interfaces/sealer/SealerInterface.h>
#include <bcos-framework/libsealer/SealerFactory.h>
#include <bcos-framework/testutils/faker/FakeLedger.h>
#include <bcos-pbft/pbft/PBFTFactory.h>
#include <bcos-sync/BlockSyncFactory.h>
#include <bcos-txpool/TxPoolFactory.h>

using namespace bcos;
using namespace bcos::crypto;
using namespace bcos::protocol;
using namespace bcos::consensus;
using namespace bcos::txpool;
using namespace bcos::sealer;
using namespace bcos::ledger;
using namespace bcos::sync;
using namespace bcos::front;
using namespace bcos::test;

namespace bcos
{
namespace node
{
class NodeObjects
{
public:
    using Ptr = std::shared_ptr<NodeObjects>;
    NodeObjects(PBFTFactory::Ptr _pbftFactory, TxPoolFactory::Ptr _txpoolFactory,
        LedgerInterface::Ptr _ledger, SealerFactory::Ptr _sealerFactory,
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

void initAndStartNode(NodeObjects::Ptr _nodeConfig)
{
    auto pbftFactory = _nodeConfig->pbftFactory();
    auto sealer = pbftFactory->pbftConfig()->sealer();
    auto pbft = pbftFactory->consensus();

    // init txpool
    _nodeConfig->txpoolFactory()->init(sealer);
    // init sealer
    _nodeConfig->sealerFactory()->init(pbft);
    // init PBFT
    _nodeConfig->blockSyncFactory()->init();
    pbftFactory->init(_nodeConfig->blockSyncFactory()->sync());

    // start txpool
    _nodeConfig->txpoolFactory()->txpool()->start();
    // start sealer
    sealer->start();
    // start PBFT
    pbft->start();
}

inline void createAndSubmitTx(CryptoSuite::Ptr _cryptoSuite,
    std::vector<NodeObjects::Ptr> _nodeList, int64_t _blockLimit, std::string const& _groupId,
    std::string const& _chainId)
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
}  // namespace node
}  // namespace bcos

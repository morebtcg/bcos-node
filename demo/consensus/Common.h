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
    NodeObjects(PBFTImpl::Ptr _pbft, TxPool::Ptr _txpool, LedgerInterface::Ptr _ledger,
        Sealer::Ptr _sealer, BlockSync::Ptr _blockSync)
      : m_pbft(_pbft),
        m_txpool(_txpool),
        m_ledger(_ledger),
        m_sealer(_sealer),
        m_blockSync(_blockSync)
    {}

    PBFTImpl::Ptr pbft() { return m_pbft; }
    TxPool::Ptr txpool() { return m_txpool; }
    LedgerInterface::Ptr ledger() { return m_ledger; }
    std::shared_ptr<Sealer> sealer() { return m_sealer; }

    BlockSync::Ptr blockSync() { return m_blockSync; }

private:
    PBFTImpl::Ptr m_pbft;
    TxPool::Ptr m_txpool;
    LedgerInterface::Ptr m_ledger;
    std::shared_ptr<Sealer> m_sealer;
    BlockSync::Ptr m_blockSync;
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
    auto txpool = _nodeConfig->txpool();
    auto pbft = _nodeConfig->pbft();
    auto sealer = _nodeConfig->sealer();

    // init txpool
    txpool->init(sealer);
    // init sealer
    sealer->init(pbft);
    // init PBFT
    _nodeConfig->blockSync()->init();
    pbft->init(_nodeConfig->blockSync());

    // start txpool
    txpool->start();
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
        selectedNode->txpool()->asyncSubmit(
            txData,
            [tx](Error::Ptr _error, TransactionSubmitResult::Ptr _result) {
                if (_error == nullptr)
                {
                    BCOS_LOG(DEBUG) << LOG_DESC("submit transaction success")
                                    << LOG_KV("hash", tx->hash().abridged())
                                    << LOG_KV("status", _result->status());
                    return;
                }
                BCOS_LOG(DEBUG) << LOG_DESC("submit transaction failed")
                                << LOG_KV("code", _error->errorCode())
                                << LOG_KV("msg", _error->errorMessage());
            },
            [](Error::Ptr _error) {
                if (_error == nullptr)
                {
                    return;
                }
                BCOS_LOG(DEBUG) << LOG_DESC("submit transaction exception")
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

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
 * @brief initializer for the PBFT module
 * @file PBFTInitializer.cpp
 * @author: yujiechen
 * @date 2021-06-10
 */
#include "PBFTInitializer.h"

using namespace bcos;
using namespace bcos::protocol;
using namespace bcos::crypto;
using namespace bcos::consensus;
using namespace bcos::sealer;
using namespace bcos::txpool;
using namespace bcos::sync;
using namespace bcos::ledger;
using namespace bcos::storage;
using namespace bcos::dispatcher;
using namespace bcos::initializer;

void PBFTInitializer::start()
{
    m_txpool->init(m_sealer);
    m_sealer->init(m_pbft);
    m_blockSync->init();
    m_pbft->init(m_blockSync);

    m_txpool->start();
    m_sealer->start();
    m_pbft->start();
}

void PBFTInitializer::stop()
{
    m_txpool->stop();
    m_sealer->stop();
    m_pbft->stop();
}

void PBFTInitializer::init(NodeConfig::Ptr _nodeConfig,
    ProtocolInitializer::Ptr _protocolInitializer, NetworkInitializer::Ptr _networkInitializer,
    LedgerInterface::Ptr _ledger, DispatcherInterface::Ptr _dispatcher,
    StorageInterface::Ptr _storage)
{
    createTxPool(_nodeConfig, _protocolInitializer, _networkInitializer, _ledger);
    createSealer(_nodeConfig, _protocolInitializer);
    createPBFT(
        _nodeConfig, _protocolInitializer, _networkInitializer, _storage, _ledger, _dispatcher);
    createSync(_nodeConfig, _protocolInitializer, _networkInitializer, _ledger, _dispatcher);
}

void PBFTInitializer::createTxPool(NodeConfig::Ptr _nodeConfig,
    ProtocolInitializer::Ptr _protocolInitializer, NetworkInitializer::Ptr _networkInitializer,
    LedgerInterface::Ptr _ledger)
{
    auto keyPair = _protocolInitializer->keyPair();
    auto cryptoSuite = _protocolInitializer->cryptoSuite();
    auto txpoolFactory = std::make_shared<TxPoolFactory>(keyPair->publicKey(), cryptoSuite,
        _protocolInitializer->txResultFactory(), _protocolInitializer->blockFactory(),
        _networkInitializer->frontService(), _ledger, _nodeConfig->groupId(),
        _nodeConfig->chainId(), _nodeConfig->blockLimit());
    // init the txpool
    m_txpool = txpoolFactory->createTxPool();
    auto txpoolConfig = m_txpool->txpoolConfig();
    txpoolConfig->setPoolLimit(_nodeConfig->txpoolLimit());
    txpoolConfig->setNotifierWorkerNum(_nodeConfig->notifyWorkerNum());
    txpoolConfig->setVerifyWorkerNum(_nodeConfig->verifierWorkerNum());

    // register txpool message dispatcher
    std::weak_ptr<TxPoolInterface> weakTxPool = m_txpool;
    auto sendResponseHandler = _networkInitializer->sendResponseHandler();
    _networkInitializer->registerMsgDispatcher(
        ModuleID::TxsSync, [weakTxPool, sendResponseHandler](
                               NodeIDPtr _nodeID, std::string const& _id, bytesConstRef _data) {
            try
            {
                auto txpool = weakTxPool.lock();
                if (!txpool)
                {
                    return;
                }
                txpool->asyncNotifyTxsSyncMessage(
                    nullptr, _nodeID, _data,
                    [_id, _nodeID, sendResponseHandler](bytesConstRef _respData) {
                        sendResponseHandler(_id, ModuleID::TxsSync, _nodeID, _respData);
                    },
                    nullptr);
            }
            catch (std::exception const& e)
            {
                INITIALIZER_LOG(WARNING) << LOG_DESC("call TxPool message dispatcher exception")
                                         << LOG_KV("error", boost::diagnostic_information(e));
            }
        });

    _networkInitializer->registerGetNodeIDsDispatcher(
        ModuleID::TxsSync, [weakTxPool](std::shared_ptr<const NodeIDs> _nodeIDs,
                               bcos::front::ReceiveMsgFunc _receiveMsgCallback) {
            try
            {
                auto txpool = weakTxPool.lock();
                if (!txpool)
                {
                    return;
                }
                if (!_nodeIDs || _nodeIDs->empty())
                {
                    return;
                }
                auto nodeIdSet = NodeIDSet(_nodeIDs->begin(), _nodeIDs->end());
                txpool->notifyConnectedNodes(nodeIdSet, _receiveMsgCallback);
                INITIALIZER_LOG(DEBUG) << LOG_DESC("notifyConnectedNodes")
                                       << LOG_KV("connectedNodeSize", nodeIdSet.size());
            }
            catch (std::exception const& e)
            {
                INITIALIZER_LOG(WARNING)
                    << LOG_DESC("call TxPool notifyConnectedNodes dispatcher exception")
                    << LOG_KV("error", boost::diagnostic_information(e));
            }
        });
}

void PBFTInitializer::createSealer(
    NodeConfig::Ptr _nodeConfig, ProtocolInitializer::Ptr _protocolInitializer)
{
    // create sealer
    auto sealerFactory = std::make_shared<SealerFactory>(
        _protocolInitializer->blockFactory(), m_txpool, _nodeConfig->minSealTime());
    m_sealer = sealerFactory->createSealer();
}

void PBFTInitializer::createPBFT(NodeConfig::Ptr _nodeConfig,
    ProtocolInitializer::Ptr _protocolInitializer, NetworkInitializer::Ptr _networkInitializer,
    StorageInterface::Ptr _storage, LedgerInterface::Ptr _ledger,
    DispatcherInterface::Ptr _dispatcher)
{
    auto keyPair = _protocolInitializer->keyPair();
    // create pbft
    auto pbftFactory = std::make_shared<PBFTFactory>(_protocolInitializer->cryptoSuite(),
        _protocolInitializer->keyPair(), _networkInitializer->frontService(), _storage, _ledger,
        m_txpool, m_sealer, _dispatcher, _protocolInitializer->blockFactory(),
        _protocolInitializer->txResultFactory());
    m_pbft = pbftFactory->createPBFT();
    auto pbftConfig = m_pbft->pbftEngine()->pbftConfig();
    pbftConfig->setCheckPointTimeoutInterval(_nodeConfig->checkPointTimeoutInterval());

    // regist PBFT message dispatcher
    std::weak_ptr<ConsensusInterface> weakPBFT = m_pbft;
    auto sendResponseHandler = _networkInitializer->sendResponseHandler();
    _networkInitializer->registerMsgDispatcher(
        ModuleID::PBFT, [weakPBFT, sendResponseHandler](
                            NodeIDPtr _nodeID, std::string const& _id, bytesConstRef _data) {
            try
            {
                auto pbft = weakPBFT.lock();
                if (!pbft)
                {
                    return;
                }
                pbft->asyncNotifyConsensusMessage(
                    nullptr, _nodeID, _data,
                    [_id, _nodeID, sendResponseHandler](bytesConstRef _respData) {
                        sendResponseHandler(_id, ModuleID::PBFT, _nodeID, _respData);
                    },
                    nullptr);
            }
            catch (std::exception const& e)
            {
                INITIALIZER_LOG(WARNING) << LOG_DESC("call PBFT message dispatcher exception")
                                         << LOG_KV("error", boost::diagnostic_information(e));
            }
        });
}

void PBFTInitializer::createSync(NodeConfig::Ptr, ProtocolInitializer::Ptr _protocolInitializer,
    NetworkInitializer::Ptr _networkInitializer, LedgerInterface::Ptr _ledger,
    DispatcherInterface::Ptr _dispatcher)
{
    // create sync
    auto keyPair = _protocolInitializer->keyPair();
    auto blockSyncFactory = std::make_shared<BlockSyncFactory>(keyPair->publicKey(),
        _protocolInitializer->blockFactory(), _protocolInitializer->txResultFactory(), _ledger,
        m_txpool, _networkInitializer->frontService(), _dispatcher, m_pbft);
    m_blockSync = blockSyncFactory->createBlockSync();

    // register block sync message handler
    std::weak_ptr<BlockSyncInterface> weakSync = m_blockSync;
    auto sendResponseHandler = _networkInitializer->sendResponseHandler();
    _networkInitializer->registerMsgDispatcher(
        ModuleID::BlockSync, [weakSync, sendResponseHandler](
                                 NodeIDPtr _nodeID, std::string const& _id, bytesConstRef _data) {
            try
            {
                auto sync = weakSync.lock();
                if (!sync)
                {
                    return;
                }
                sync->asyncNotifyBlockSyncMessage(
                    nullptr, _nodeID, _data,
                    [_id, _nodeID, sendResponseHandler](bytesConstRef _respData) {
                        sendResponseHandler(_id, ModuleID::BlockSync, _nodeID, _respData);
                    },
                    nullptr);
            }
            catch (std::exception const& e)
            {
                INITIALIZER_LOG(WARNING) << LOG_DESC("call Sync message dispatcher exception")
                                         << LOG_KV("error", boost::diagnostic_information(e));
            }
        });
}
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
 * @brief initializer for the rpc
 * @file RPCInitializer.cpp
 * @author: octopus
 * @date 2021-07-15
 */
#include "RpcInitializer.h"
#include <bcos-rpc/rpc/RpcFactory.h>
#include <include/BuildInfo.h>

using namespace bcos;
using namespace bcos::initializer;

void RpcInitializer::init(bcos::tool::NodeConfig::Ptr _nodeConfig, const std::string& _configPath)
{
    bcos::rpc::NodeInfo nodeInfo;
    nodeInfo.nodeID = m_nodeID;
    nodeInfo.groupID = m_nodeConfig->groupId();
    nodeInfo.chainID = m_nodeConfig->chainId();
    nodeInfo.version = FISCO_BCOS_PROJECT_VERSION;
    nodeInfo.buildTime = FISCO_BCOS_BUILD_TIME;
    nodeInfo.gitCommitHash = FISCO_BCOS_COMMIT_HASH;
    nodeInfo.isSM = _nodeConfig->smCryptoType();
    nodeInfo.isWasm = _nodeConfig->isWasm();

    INITIALIZER_LOG(INFO) << LOG_BADGE("RpcInitializer::init") << LOG_KV("config", _configPath)
                          << LOG_KV("nodeID", nodeInfo.nodeID)
                          << LOG_KV("groupID", nodeInfo.groupID)
                          << LOG_KV("chainID", nodeInfo.chainID)
                          << LOG_KV("version", nodeInfo.version)
                          << LOG_KV("buildTime", nodeInfo.buildTime)
                          << LOG_KV("gitCommitHash", nodeInfo.gitCommitHash);

    auto factory = std::make_shared<bcos::rpc::RpcFactory>();
    factory->setLedger(m_ledgerInterface);
    factory->setTxPoolInterface(m_txPoolInterface);
    factory->setExecutorInterface(m_executorInterface);
    factory->setConsensusInterface(m_consensusInterface);
    factory->setBlockSyncInterface(m_blockSyncInterface);
    factory->setGatewayInterface(m_gatewayInterface);
    factory->setTransactionFactory(m_transactionFactory);
    auto rpc = factory->buildRpc(_configPath, nodeInfo);

    m_rpcInterface = rpc;
}

void RpcInitializer::start()
{
    if (m_rpcInterface)
    {
        m_rpcInterface->start();
    }
}

void RpcInitializer::stop()
{
    if (m_rpcInterface)
    {
        m_rpcInterface->stop();
    }
}
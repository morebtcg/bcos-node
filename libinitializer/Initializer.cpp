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
 * @brief Initializer for all the modules
 * @file Initializer.cpp
 * @author: yujiechen
 * @date 2021-06-11
 */
#include "Initializer.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-framework/libtool/NodeConfig.h>

using namespace bcos;
using namespace bcos::tool;
using namespace bcos::initializer;

void Initializer::init(std::string const& _configFilePath, std::string const& _genesisFile)
{
    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::read_ini(_configFilePath, pt);
        m_logInitializer = std::make_shared<BoostLogInitializer>();
        m_logInitializer->initLog(pt);

        boost::property_tree::ptree genesisConfig;
        boost::property_tree::read_ini(_genesisFile, genesisConfig);
        BCOS_LOG(INFO) << LOG_DESC("init config") << LOG_KV("ini", _configFilePath)
                       << LOG_KV("genesis", _genesisFile);

        // loadConfig
        m_nodeConfig =
            std::make_shared<NodeConfig>(std::make_shared<bcos::crypto::KeyFactoryImpl>());
        m_nodeConfig->loadConfig(pt, genesisConfig);

        // init the protocol
        m_protocolInitializer = std::make_shared<ProtocolInitializer>();
        m_protocolInitializer->init(m_nodeConfig);

        // init the network
        m_networkInitializer = std::make_shared<NetworkInitializer>();
        m_networkInitializer->init(
            _configFilePath, m_nodeConfig, m_protocolInitializer->keyPair()->publicKey());

        // init the storage
        auto storageInitializer = std::make_shared<StorageInitializer>();
        storageInitializer->init(m_nodeConfig);

        // init the ledger
        m_ledgerInitializer = std::make_shared<LedgerInitializer>();
        m_ledgerInitializer->init(
            m_protocolInitializer->blockFactory(), storageInitializer->storage(), m_nodeConfig);

        // init the dispatcher
        m_dispatcherInitializer = std::make_shared<DispatcherInitializer>();
        m_dispatcherInitializer->init(m_nodeConfig, m_protocolInitializer,
            m_ledgerInitializer->ledger(), storageInitializer->storage());

        auto dispatcher = m_dispatcherInitializer->dispatcher();

        // init the pbft related modules
        m_pbftInitializer = std::make_shared<PBFTInitializer>();
        m_pbftInitializer->init(m_nodeConfig, m_protocolInitializer, m_networkInitializer,
            m_ledgerInitializer->ledger(), dispatcher, storageInitializer->storage());

        dispatcher->init(m_pbftInitializer->txpool());
    }
    catch (std::exception const& e)
    {
        std::cout << "init bcos-node failed for " << boost::diagnostic_information(e);
        exit(-1);
    }
}

void Initializer::start()
{
    try
    {
        if (m_dispatcherInitializer)
            m_dispatcherInitializer->start();
        if (m_networkInitializer)
            m_networkInitializer->start();
        if (m_pbftInitializer)
            m_pbftInitializer->start();
    }
    catch (std::exception const& e)
    {
        std::cout << "start bcos-node failed for " << boost::diagnostic_information(e);
        exit(-1);
    }
}

void Initializer::stop()
{
    try
    {
        if (m_networkInitializer)
            m_networkInitializer->stop();
        if (m_pbftInitializer)
            m_pbftInitializer->stop();
        if (m_dispatcherInitializer)
            m_dispatcherInitializer->stop();
        if (m_logInitializer)
            m_logInitializer->stopLogging();
    }
    catch (std::exception const& e)
    {
        std::cout << "stop bcos-node failed for " << boost::diagnostic_information(e);
        exit(-1);
    }
}
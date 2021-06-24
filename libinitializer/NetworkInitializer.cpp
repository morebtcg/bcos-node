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
 * @brief initializer for the network
 * @file NetworkInitializer.cpp
 * @author: yujiechen
 * @date 2021-06-10
 */
#include "NetworkInitializer.h"
#include <bcos-front/FrontServiceFactory.h>
#include <bcos-gateway/GatewayConfig.h>
#include <bcos-gateway/GatewayFactory.h>

using namespace bcos;
using namespace bcos::initializer;
using namespace bcos::gateway;
using namespace bcos::front;
using namespace bcos::crypto;
using namespace bcos::protocol;

void NetworkInitializer::init(
    std::string const& _configFilePath, NodeConfig::Ptr _nodeConfig, NodeIDPtr _nodeID)
{
    INITIALIZER_LOG(INFO) << LOG_DESC("init the network") << LOG_KV("nodeId", _nodeID->shortHex())
                          << LOG_KV("groupId", _nodeConfig->groupId());
    initGateWay(_configFilePath);
    initFrontService(_nodeConfig, _nodeID);
    std::weak_ptr<FrontService> weakFrontService = m_frontService;
    m_sendResponseHandler = [weakFrontService](std::string const& _id, int _moduleID,
                                NodeIDPtr _dstNode, bytesConstRef _data) {
        try
        {
            auto frontService = weakFrontService.lock();
            if (!frontService)
            {
                return;
            }
            frontService->asyncSendResponse(
                _id, _moduleID, _dstNode, _data, [_id, _moduleID, _dstNode](Error::Ptr _error) {
                    if (_error)
                    {
                        INITIALIZER_LOG(WARNING)
                            << LOG_DESC("sendResonse failed") << LOG_KV("uuid", _id)
                            << LOG_KV("module", std::to_string(_moduleID))
                            << LOG_KV("dst", _dstNode->shortHex())
                            << LOG_KV("code", _error->errorCode())
                            << LOG_KV("msg", _error->errorMessage());
                    }
                });
        }
        catch (std::exception const& e)
        {
            INITIALIZER_LOG(WARNING) << LOG_DESC("sendResonse exception")
                                     << LOG_KV("error", boost::diagnostic_information(e));
        }
    };
}

FrontServiceInterface::Ptr NetworkInitializer::frontService()
{
    return m_frontService;
}

void NetworkInitializer::initGateWay(std::string const& _configFilePath)
{
    auto gateWayFactory = std::make_shared<GatewayFactory>();
    m_gateWay = gateWayFactory->buildGateway(_configFilePath);
}

void NetworkInitializer::initFrontService(NodeConfig::Ptr _nodeConfig, NodeIDPtr _nodeID)
{
    auto frontServiceFactory = std::make_shared<FrontServiceFactory>();
    frontServiceFactory->setGatewayInterface(m_gateWay);
    auto threadPool = std::make_shared<ThreadPool>("frontService", 1);
    frontServiceFactory->setThreadPool(threadPool);
    m_frontService = frontServiceFactory->buildFrontService(_nodeConfig->groupId(), _nodeID);
    m_gateWay->registerFrontService(_nodeConfig->groupId(), _nodeID, m_frontService);
}

void NetworkInitializer::start()
{
    m_gateWay->start();
    m_frontService->start();
}
void NetworkInitializer::stop()
{
    m_gateWay->stop();
    m_frontService->stop();
}

void NetworkInitializer::registerMsgDispatcher(ModuleID _moduleID,
    std::function<void(NodeIDPtr _nodeID, std::string const& _id, bytesConstRef _data)> _dispatcher)
{
    m_frontService->registerModuleMessageDispatcher(_moduleID, _dispatcher);
}

void NetworkInitializer::registerGetNodeIDsDispatcher(ModuleID _moduleID,
    std::function<void(std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
        bcos::front::ReceiveMsgFunc _receiveMsgCallback)>
        _dispatcher)
{
    m_frontService->registerModuleNodeIDsDispatcher(_moduleID, _dispatcher);
}
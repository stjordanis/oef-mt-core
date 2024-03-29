#pragma once
//------------------------------------------------------------------------------
//
//   Copyright 2018-2019 Fetch.AI Limited
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//------------------------------------------------------------------------------

#include "buffer_t.hpp"
#include "oef_search_response_t.hpp"

#include <functional>
//#include <system_error>
#include <boost/system/error_code.hpp>
#include <memory>

namespace boosts = boost::system;

namespace fetch {
namespace oef {
    /*
     * Defines type aliases for different continuation functions (callbacks) needed to handle asynchrounous
     * operations responses. All parameters are [in]. 
     */
    using BufferContinuation = std::function<void(boosts::error_code,std::shared_ptr<Buffer>)>;
    using VoidBuffContinuation = std::function<void(boosts::error_code,std::shared_ptr<void>)>;
    using LengthContinuation = std::function<void(boosts::error_code,std::size_t)>;
    using AgentSessionContinuation = std::function<void(boosts::error_code,oef::OefSearchResponse)>;
    class communicator_t;
    using CommunicatorContinuation = std::function<void(boosts::error_code,std::shared_ptr<communicator_t>)>;
} // oef
} // fetch

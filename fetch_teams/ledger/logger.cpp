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

#include "fetch_teams/ledger/logger.hpp"

namespace fetch {
  std::map<std::thread::id, int> fetch::log::ReadableThread::thread_number_ =
      std::map<std::thread::id, int>();
  int        fetch::log::ReadableThread::thread_count_ = 0;
  std::mutex fetch::log::ReadableThread::mutex_;

  log::details::LogWrapper logger;

  namespace log {
    Context::Context(void *instance)
    {
      created_ = std::chrono::high_resolution_clock::now();
      details_ = std::make_shared<ContextDetails>(instance);
      fetch::logger.SetContext(details_);
    }

    Context::Context(shared_type ctx, std::string const &context, std::string const &filename,
                     int const &line, void *instance)
    {
      created_ = std::chrono::high_resolution_clock::now();
      details_ = std::make_shared<ContextDetails>(ctx, fetch::logger.TopContext(), context, filename,
                                                  line, instance);
      fetch::logger.SetContext(details_);
    }

    Context::Context(std::string const &context, std::string const &filename, int const &line,
                     void *instance)
    {
      created_ = std::chrono::high_resolution_clock::now();
      details_ = std::make_shared<ContextDetails>(fetch::logger.TopContext(), context, filename, line,
                                                  instance);
      fetch::logger.SetContext(details_);
    }

    Context::~Context()
    {
      std::chrono::high_resolution_clock::time_point end_time =
          std::chrono::high_resolution_clock::now();
      double total_time =
          double(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - created_).count());
      fetch::logger.UpdateContextTime(details_, total_time);

      if (primary_ && details_->parent())
      {
        fetch::logger.SetContext(details_->parent());
      }
    }
  }  // namespace log

}  // namespace fetch

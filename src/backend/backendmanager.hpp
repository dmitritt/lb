/*
 * Copyright (c) 2016 Dmitri
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 */

#ifndef BACKEND_MANAGER_HPP
#define BACKEND_MANAGER_HPP

#include <boost/asio.hpp>

#include "../config.hpp"
#include "backend.hpp"
#include "../ipc/requestresponse.hpp"

namespace IPC {
  class Client;
}

class BackendManager { //: public IPC::BackendManager {
public:
  BackendManager(const Config& config, boost::asio::io_service& ioService);
  BackendManager(BackendManager&) = delete;
  BackendManager& operator=(const BackendManager&) = delete;
  
//  void sendHandshake(IPC::HandshakeRequest& request, IPC::Response& response);
//  void sendRequest(IPC::Request& request, IPC::Response& response);
private:
  friend class IPC::Client;
  Backend::Ptr getAvailableBackend();
private:
  boost::asio::io_service& ioService;
  Backend::List backends;  
};

#endif /* BACKEND_MANAGER_HPP */


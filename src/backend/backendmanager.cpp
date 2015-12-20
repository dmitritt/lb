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

#include <algorithm>
#include <bits/unique_ptr.h>
#include "backendmanager.hpp"

namespace {
  Backend::Ptr NO_BACKEND;  
}

BackendManager::BackendManager(const Config& config, boost::asio::io_service& ioService_) 
  : ioService{ioService_} {
  for (auto address : config.getBackendAddresses()) {
    Backend::Ptr backend = std::make_shared<Backend>(address);    
    backends.push_back(backend);
  }  
}

/*  
void BackendManager::sendHandshake(IPC::HandshakeRequest& request, IPC::Response& response) {
  auto backend = getAvailableBackend();
  if (!backend) {
    response.onDisconnect();
  } else {
    
  }
}

void BackendManager::sendRequest(IPC::Request& request, IPC::Response& response) {

}
*/
  
Backend::Ptr BackendManager::getAvailableBackend() {
  auto it = std::find_if(backends.begin(), backends.end(), [](Backend::Ptr backend) {return backend->take();});
  return it != backends.end() ? *it : Backend::Ptr{};
}

/*
void BackendManager::executeRequest(IPC::Request& request, IPC::Response& response) {
  auto it = std::find_if(backends.begin(), backends.end(), [](Backend::Ptr backend) {return backend->take();});
  if (it != backends.end()) {
    (*it)->executeRequest(request, response);
  } else {
    response.sendNoAvailableBackendsError();
  }
}

void BackendManager::handshake(IPC::HandshakeRequest request, IPC::Response& response) {
  auto it = std::find_if(backends.begin(), backends.end(), [](Backend::Ptr backend) {return backend->take();});
  if (it != backends.end()) {
    (*it)->handshake(request, response);
  } else {
    response.disconnect();
  }  
}
 */ 
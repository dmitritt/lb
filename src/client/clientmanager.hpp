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

#ifndef CLIENT_MANAGER_HPP
#define CLIENT_MANAGER_HPP

#include <mutex>
#include <vector>
#include <boost/asio.hpp>
#include "../backend/backendmanager.hpp"
#include "abstractclient.hpp"

class ClientManager {
public:
  ClientManager(const Config& config, BackendManager& backendManager);
  void add(AbstractClient::Ptr client);
  void remove(AbstractClient::Ptr client);
private:
  const Config& config;
  BackendManager& backendManager;
  std::mutex clientsLock;
  std::vector<AbstractClient::Ptr> clients;
};

#endif /* CLIENT_MANAGER_HPP */
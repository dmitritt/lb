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

#include <thread>
#include "protocoldetector.hpp"
#include "server.hpp"

Server::Server(const Config& config_)
  : config{config_},
    ioService{},  
    backendManager{config, ioService},
    clientManager{config, backendManager},
    acceptor{ioService, tcp::endpoint{tcp::v4(), config.getPort()}},
    socket{ioService} {  
}
  
void Server::accept() {
  acceptor.async_accept(socket,
    [this](boost::system::error_code ec) {
      if (!ec) {
        ProtocolDetector::start(backendManager, socket, [this](AbstractClient::Ptr client){clientManager.add(client);});
      }

      accept();
    });
}

void Server::run() {  
  accept();
  
  std::vector<std::thread> threads;
  for (int i = 1; i < config.getThreads();++i) {
    threads.emplace_back([this](){ioService.run();});
  }
  ioService.run();
}
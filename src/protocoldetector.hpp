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

#ifndef PROTOCOL_DETECTOR_HPP
#define PROTOCOL_DETECTOR_HPP

#include <exception>
#include <functional>
#include <vector>
#include <boost/asio.hpp>
#include "backend/backendmanager.hpp"
#include "client/abstractclient.hpp"

class ProtocolDetector : public std::enable_shared_from_this<ProtocolDetector> {
public:
  typedef std::function<void(AbstractClient::Ptr)> ClientProcessor;
  
  static void start(::BackendManager& backendManager, tcp::socket& socket, ClientProcessor&& clientProcessor);   
private:
  ProtocolDetector(::BackendManager& backendManager, tcp::socket& socket, ClientProcessor&& clientProcessor);
  void start();
  std::size_t detect(std::size_t bytes_transferred);
private:  
  ::BackendManager& backendManager;
  tcp::socket socket;
  ClientProcessor clientProcessor;
  
  std::vector<char> buffer;
};
#endif /* PROTOCOL_DETECTOR_HPP */
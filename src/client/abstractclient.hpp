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

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <atomic>
#include <functional>
#include <memory>
#include <boost/asio.hpp>
#include "../config.hpp"

using boost::asio::ip::tcp;

namespace {
  const std::size_t BUFFER_SIZE = 2 * 1024;
  const std::size_t BUFFERS_SIZE = 16;
}

class AbstractClient : public std::enable_shared_from_this<AbstractClient> {
public:
  typedef std::shared_ptr<AbstractClient> Ptr;  
  typedef std::function<void()> OnAbstractClientDisconnect;
    
  AbstractClient(tcp::socket&& socket);
  AbstractClient(const AbstractClient& other) = delete;
  AbstractClient& operator=(const AbstractClient& right) = delete;
  virtual ~AbstractClient();

  void setDisconnectListener(OnAbstractClientDisconnect listener);
  virtual void start() = 0;  
  virtual void disconnect();

protected:   
  tcp::socket socket;
  OnAbstractClientDisconnect disconnectListener;
};

#endif /* CLIENT_HPP */
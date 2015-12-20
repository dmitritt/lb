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

#ifndef BACKEND_HPP
#define BACKEND_HPP

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "../config.hpp"
#include "../ipc/requestresponse.hpp"

class Backend : public std::enable_shared_from_this<Backend> {
public:
  typedef std::shared_ptr<Backend> Ptr;
  typedef std::vector<Ptr> List;
  
  enum class State : char {DISCONNECTED, AVAILABLE, BUSY, BROKEN, REMOVED};
  Backend(const Address& address);
  const Address& getAddress() {return address;}
  void connect();
  void check();
  State getState() {return state;}
  bool take();
  void release();
  void handshake(IPC::HandshakeRequest& request, IPC::Response& response);
  void executeRequest(IPC::Request& request, IPC::Response& response);
private:
  const Address& address;
  std::atomic<State> state; 
};

#endif /* BACKEND_HPP */


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

#ifndef BACKEND_CONNECTION_HPP
#define BACKEND_CONNECTION_HPP

#include <memory>
#include <boost/asio.hpp>
#include "backend.hpp"
#include "../config.hpp"
#include "../ipc/requestresponse.hpp"

using boost::asio::ip::tcp;

namespace IPC {

class BackendConnection : public std::enable_shared_from_this<BackendConnection> {
public:
  typedef std::shared_ptr<BackendConnection> Ptr;  
  
  BackendConnection(Backend::Ptr backend, boost::asio::io_service& ioService);
  BackendConnection(const BackendConnection&) = delete;
  BackendConnection& operator=(const BackendConnection&) = delete;
  ~BackendConnection();
  
  bool isConnected() {return connected;}
  void start(IPC::HandshakeRequest& request, IPC::Response& response);
  bool take() {return backend->take();}
  void executeRequest(IPC::Request& request, IPC::Response& response); 
  void close();
  bool operator==(Backend::Ptr& other) {return backend==other;}
private:
  void markBroken();
  void connect(tcp::resolver::iterator i);
  void handshake();
  void readHandshakeResponse();
  void writeRequestHeader();
  void writeRequestBody();
  void readResponseHeader();
  void readResponseBody();
private:
  bool connected;
  Backend::Ptr backend;
  tcp::socket socket;
  IPC::HandshakeRequest* handshakeRequest;
  IPC::Request* request;
  IPC::Response* response;
  char shakehandResponse;
};

} // namespace IPC

#endif /* BACKEND_CONNECTION_HPP */


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

#ifndef IPC_CLIENT_HPP
#define IPC_CLIENT_HPP

#include <vector>
#include <boost/asio.hpp>
#include "../backend/backendconnection.hpp"
#include "../backend/backendmanager.hpp"
#include "../client/abstractclient.hpp"
#include "clientcontext.hpp"
#include "message.hpp"
#include "requestresponse.hpp"

namespace IPC {

class SHNSRResponse;  
  
class Client : public AbstractClient, public Request, public Response {
public:
  Client(ClientContext& clientContext, tcp::socket&& socket, std::vector<char>&& buffer);
  Client(const Client& other) = delete;
  Client& operator=(const Client& right) = delete;
  virtual ~Client();
  
  void start() override;
  // IPC::Request -->
  const char* getHeader() {return request.getHeader();}
  std::size_t getHeaderSize() {return request.getHeaderSize();;};
  const std::vector<buffer>& getBody() {return request.getBody();}
  std::size_t getBodySize() {return request.getBodySize();}  
  // <-- IPC::Request
  // IPC::Response -->
  void onHandshakeResponse(char) override;
  void onResponse(std::vector<char>&& buffer) override;
  void onDisconnect();
  // <-- IPC::Response
  void disconnect() override;
private:  
  void sendNoAvailableBackendsError();
  bool getAvailableConnection();
  void readRequestHeader();
  void ensureBuffersCapacity();
  void readRequestBody();
  void sendRequest();
  void doSendRequest();
private:
  ClientContext clientContext;
  HandshakeRequest handshakeRequest;
  char shakehandResponse;  
  Message request;
  BackendConnection::Ptr currentConnection;
  std::vector<BackendConnection::Ptr> connections;
  
  // shakehand and send request
  class R : public IPC::Response {
  public:
    R(IPC::Client& client_) : client{client_} {}  
    bool releaseAfterHandshake() {return false;}
    void onHandshakeResponse(char ignored) override {client.doSendRequest();}
    void onResponse(std::vector<char>&& buffer) override {/* TODO log error */}
    void onDisconnect() override {client.onDisconnect();}
  private:
    IPC::Client& client;
  } shNsRRequest;
};

} // namespace IPC

#endif /* CLIENT_HPP */
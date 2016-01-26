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
#include <utility>
#include "../backend/backendconnection.hpp"
#include "client.hpp"

namespace {  
  // 'no available backends
  const unsigned char NO_AVAILABLE_BACKENDS[] = {0x01,0x02,0x00,0x00,0x1f,0x00,0x00,0x00,0x80,0x6e,0x6f,0x20,0x61,0x76,0x61,0x69,0x6c,0x61,0x62,0x6c,0x65,0x20,0x62,0x61,0x63,0x6b,0x65,0x6e,0x64,0x73,0x00};
}  

namespace IPC {

namespace detail {

class SimpleHandshakeResponse : public IPC::HandshakeResponse {
public:
  SimpleHandshakeResponse(Client& client_) : client{client_} {}
  
  bool releaseAfterHandshake() override {return true;}
  void sendHandshakeResponse(char response) override {client.sendHandshakeResponse(response);}

  void onDisconnect() override {client.onDisconnect();}
private:
  Client& client;
};
  
// shake hand before sending request
class BeforeRequestHandshakeResponse : public IPC::HandshakeResponse {
public:
  BeforeRequestHandshakeResponse(Client& client_) : client{client_} {}
  
  bool releaseAfterHandshake() override {return false;}
  void sendHandshakeResponse(char ignored) override {client.doSendRequest();}

  void onDisconnect() override {client.onDisconnect();}
private:
  Client& client;
};

class ResponseImpl : public IPC::Response {
public:
  ResponseImpl(Client& client_) : client{client_} {}  

  Header& getResponseHeader() override {return client.request.getHeader();}
  message_size_t sendResponseHeader() override {
    return bytesToRead = client.sendResponseHeader();
  }  
  message_size_t sendResponseChunk(buffer chunk) override {    
    client.addResponseChunk(chunk);
    return std::max<message_size_t>(bytesToRead -= chunk->size(), 0);
  }

  void onDisconnect() override {client.onDisconnect();}
private:
  Client& client;
  message_size_t bytesToRead;  
};
  
} // namespace detail 
  
Client::Client(ClientContext& clientContext_, tcp::socket&& socket, std::vector<char>&& buffer) 
  : AbstractClient{std::move(socket)},
    clientContext{clientContext_},
    handshakeRequest{0UL, std::move(buffer)},
    simpleHandshakeResponse{new detail::SimpleHandshakeResponse{*this}},
    request{clientContext.bufferPool},
    beforeRequestHandshakeResponse{new detail::BeforeRequestHandshakeResponse{*this}},
    response{new detail::ResponseImpl{*this}},
    responseChunks([this](std::pair<IPC::buffer,message_size_t>& p) {
      sendResponseChunk(p);
    }){
}

Client::~Client() {
}

void Client::start() {
  assert(connections.empty());
  if (!getAvailableConnection()) {
    disconnect();
  } else {    
    assert(!currentConnection->isConnected());
    currentConnection->start(handshakeRequest, *(simpleHandshakeResponse.get()));
  }  
}

void Client::disconnect() {
  AbstractClient::disconnect();
}

bool Client::getAvailableConnection() {
  for (auto& c : connections) {
    if (c->take()) {
      currentConnection = c;
      return true;
    }
  }
  
  auto backend = clientContext.backendManager.getAvailableBackend();
  if (!backend) {
    return false;
  }
  
  for (auto& c : connections) {
    if (*c == backend) {
      currentConnection = c;
      return true;
    }
  }
  
  currentConnection.reset(new BackendConnection{clientContext.bufferPool, backend, clientContext.backendManager.ioService});
  connections.push_back(currentConnection);
  return true;
}

void Client::sendHandshakeResponse(char response) {
  auto self = shared_from_this();
  shakehandResponse = response;
  boost::asio::async_write(
    socket,
    boost::asio::buffer(&shakehandResponse, sizeof(shakehandResponse)),
    [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (ec) {
        // TODO log
        disconnect();
      } else {        
        readRequestHeader();
      }
    });
}

void Client::readRequestHeader() {
  auto self = shared_from_this();
  boost::asio::async_read(
    socket,
    boost::asio::buffer(request.getHeader().get(), request.getHeader().size()),
    [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (ec) {
        // TODO log
        disconnect();
      } else {
        readRequestBody();
      }
    });
}

void Client::readRequestBody() {
  auto self = shared_from_this();
  request.parseBodySize();
  request.ensureBodyBufferCapacity();
  boost::asio::async_read(
    socket,
    boost::asio::buffer(request.getBody(), request.getBodySize()),
    [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (ec) {
        // TODO log
        disconnect();
      } else {
        sendRequest();
      }
    });
}

void Client::sendRequest() {  
  if (!getAvailableConnection()) {
    sendNoAvailableBackendsError();
  } else if (currentConnection->isConnected()) {
    doSendRequest();
  } else {
    currentConnection->start(handshakeRequest, *(beforeRequestHandshakeResponse.get()));
  }
}

void Client::doSendRequest() {
  currentConnection->executeRequest(request, *(response.get()));
}

void Client::sendNoAvailableBackendsError() {
  auto self = shared_from_this();
  boost::asio::async_write(
    socket,
    boost::asio::buffer(NO_AVAILABLE_BACKENDS, sizeof(NO_AVAILABLE_BACKENDS)),
    [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (ec) {
        // TODO log
        disconnect();
      } else {
        readRequestHeader();
      }
    });
}

message_size_t Client::sendResponseHeader() {
  message_size_t bytesToSend = request.getHeader().getBodySize();
  responseChunks.setBytesToSend(bytesToSend);
  
  auto self = shared_from_this();
  boost::asio::async_write(
    socket,
    boost::asio::buffer(request.getHeader().get(), request.getHeader().size()),
    [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (ec) {
        // TODO log
        disconnect();
      } else {
        sendNextResponseChunk();
      }
    });
  
  return bytesToSend;
}

void Client::addResponseChunk(buffer bytes) {
  responseChunks.put(bytes);
}

void Client::sendResponseChunk(std::pair<buffer,message_size_t>& p) {
  if (p.second) {
    auto b = p.first;
    auto self = shared_from_this();
    boost::asio::async_write(
      socket,
      boost::asio::buffer(b->get(), p.second),
      [this, self, b](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec) {
          // TODO log
          disconnect();
        } else {
          sendNextResponseChunk();
        }
      });
  }
}

void Client::sendNextResponseChunk() {
  auto p = responseChunks.get();
  sendResponseChunk(p);
}


void Client::onDisconnect() {
  // TODO log
  disconnect();
}

} // namespace IPC
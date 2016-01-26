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

#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include "../backend/backendconnection.hpp"
#include "../backend/backendmanager.hpp"
#include "../client/abstractclient.hpp"
#include "bufferpool.hpp"
#include "clientcontext.hpp"
#include "message.hpp"
#include "requestresponse.hpp"
#include "responsechunkpipe.hpp"

namespace IPC {

namespace detail {
  class SimpleHandshakeResponse;
  class BeforeRequestHandshakeResponse;
  class ResponseImpl;
}  
  
class Client : public AbstractClient {
public:
  Client(ClientContext& clientContext, tcp::socket&& socket, std::vector<char>&& buffer);
  Client(const Client& other) = delete;
  Client& operator=(const Client& right) = delete;
  virtual ~Client();
  
  void start() override;
  void disconnect() override;
private:  
  void sendHandshakeResponse(char response);
  Header& getResponseHeader(){return request.getHeader();}
  bool releaseAfterHandshake(){return true;};
  void onDisconnect();
  message_size_t sendResponseHeader();
  void addResponseChunk(buffer bytes);
  void sendNextResponseChunk();
  void sendResponseChunk(std::pair<buffer,message_size_t>& p);

  void sendNoAvailableBackendsError();
  bool getAvailableConnection();
  void readRequestHeader();
  void ensureBuffersCapacity();
  void readRequestBody();
  void sendRequest();
  void doSendRequest();
    
  friend class detail::SimpleHandshakeResponse;
  friend class detail::BeforeRequestHandshakeResponse;
  friend class detail::ResponseImpl;
private:
  ClientContext clientContext;
  BackendConnection::Ptr currentConnection;
  std::vector<BackendConnection::Ptr> connections;

  HandshakeRequest handshakeRequest;
  const std::unique_ptr<HandshakeResponse> simpleHandshakeResponse;  
  char shakehandResponse;  
  Message request;
  const std::unique_ptr<HandshakeResponse> beforeRequestHandshakeResponse;  
  const std::unique_ptr<detail::ResponseImpl> response;  
  ResponseChunkPipe responseChunks;
};

} // namespace IPC

#endif /* CLIENT_HPP */
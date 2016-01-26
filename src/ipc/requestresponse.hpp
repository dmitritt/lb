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

#ifndef IPC_REQUEST_RESPONSE_HPP
#define IPC_REQUEST_RESPONSE_HPP

#include <memory>
#include <utility>
#include "bufferpool.hpp"
#include "message.hpp"

typedef unsigned long client_id_t;  

namespace IPC {
  
class HandshakeRequest {    
public:
  HandshakeRequest() {}
  HandshakeRequest(client_id_t clientId_, std::vector<char>&& buffer_) 
    : clientId{clientId_},
      buffer{std::move(buffer)} {}
      
  HandshakeRequest& operator=(HandshakeRequest&& other) {
    clientId = other.clientId;
    buffer = std::move(other.buffer);
  }
  
  client_id_t getClientId() {return clientId;}
  const std::vector<char>& getBuffer() {return buffer;}
private:
  client_id_t clientId;
  // null-terminated ascii string "username:password\N"
  std::vector<char> buffer;  
};  

class HandshakeResponse {
public:
  virtual ~HandshakeResponse() {}
  
  // returns if backend to be released after the handshake
  virtual bool releaseAfterHandshake() = 0;
  virtual void sendHandshakeResponse(char) = 0;
  
  virtual void onDisconnect() = 0;
};

class Response {
public:
  virtual ~Response() {}
  
  virtual Header& getResponseHeader() = 0;
  // returns number of bytes to be read from backend connected
  virtual message_size_t sendResponseHeader() = 0;
  // returns number of bytes to be read from backend connected, 0 if nothing to read
  virtual message_size_t sendResponseChunk(buffer chunk) = 0;
  
  virtual void onDisconnect() = 0;
};

} // namespace IPC
  
#endif /* IPC_REQUEST_RESPONSE_HPP */
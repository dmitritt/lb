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

#include "protocoldetector.hpp"
#include "ipc/client.hpp"

void ProtocolDetector::start(IPC::ClientContext clientContext, tcp::socket& socket, ClientProcessor&& clientProcessor) {
  std::shared_ptr<ProtocolDetector>(new ProtocolDetector{clientContext, socket, std::move(clientProcessor)})->start();
}

ProtocolDetector::ProtocolDetector(IPC::ClientContext clientContext_, tcp::socket& socket_, ClientProcessor&& clientProcessor_) 
  : clientContext{clientContext_},
    socket{std::move(socket_)},
    clientProcessor{std::move(clientProcessor_)},
    buffer(BUFFER_SIZE) {
}

void ProtocolDetector::start() {
  auto self{shared_from_this()};
  boost::asio::async_read(
    socket, 
    boost::asio::buffer(buffer),
    // A return value of 0 indicates that the read operation is complete. 
    // A non-zero return value indicates the maximum number of bytes to be read on the next call to the stream's async_read_some function.       
    [self](const boost::system::error_code& ec, std::size_t bytes_transferred) -> std::size_t {
      return ec ? 0 : self->detect(bytes_transferred);
    },
    [self](boost::system::error_code ec, std::size_t length) {
      // no ops
    });
}

// A return value of 0 indicates that the read operation is complete. 
// A non-zero return value indicates the maximum number of bytes to be read on the next call to the stream's async_read_some function.       
std::size_t ProtocolDetector::detect(std::size_t bytes_transferred) {
  const char* GET = "GET ";
  const char* OPTIONS = "OPTIONS ";
  if (buffer.back() == '\0') {
    // after a client has opened a socket to the server, it sends a null-terminated ascii string "username:password\N" where \N is a single byte (0..3) 
    // which represents the client's capability wrt compression, timestamp|timespan and uuid, 
    // e.g. "myname:mypassword\3".
    buffer.resize(bytes_transferred);
    clientProcessor(AbstractClient::Ptr{new IPC::Client(clientContext, std::move(socket), std::move(buffer))});
    return 0;
  } else if (std::strncmp(GET, &buffer.front(), sizeof(GET)) || std::strncmp(OPTIONS, &buffer.front(), sizeof(OPTIONS))) {      
    // TODO add more check here
    throw std::exception{};
  } else {      
    // TODO do something if buffer.size() == bytes_transferred
    return buffer.size() - bytes_transferred;
  }
}
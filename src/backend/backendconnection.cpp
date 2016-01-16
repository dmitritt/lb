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

#include "backendconnection.hpp"

using boost::asio::ip::tcp;

namespace IPC {
  
BackendConnection::BackendConnection(Backend::Ptr backend_, boost::asio::io_service& ioService)
  : connected{false},
    backend{backend_},
    socket{ioService} {
}

BackendConnection::~BackendConnection() {
  close();
}

void BackendConnection::start(IPC::HandshakeRequest& request, IPC::Response& response_) {
  handshakeRequest = &request;
  response = &response_;
  tcp::resolver resolver{socket.get_io_service()};
  resolver.async_resolve({backend->getAddress().host, backend->getAddress().port}, 
    [this](const boost::system::error_code& ec, tcp::resolver::iterator it) {
      if (ec) {
        markBroken();
      } else {
        connect(it);
      } 
    });
}

void BackendConnection::connect(tcp::resolver::iterator it) {
  boost::asio::async_connect(socket, it,
    [this](boost::system::error_code ec, tcp::resolver::iterator) {
      if (ec) {
        markBroken();
      } else {
        handshake();
      }
    });
}

void BackendConnection::markBroken() {
  // TODO implement
}

void BackendConnection::close() {
  response = nullptr;
  if (backend) {
    backend->release();
  }
  boost::system::error_code ec;
  socket.close(ec);
  if (ec) {
    // TODO log
  }
}

void BackendConnection::handshake() {
  boost::asio::async_write(
    socket, 
    boost::asio::buffer(handshakeRequest->getBuffer()),
    [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      handshakeRequest = nullptr;
      if (!ec) {
        // TODO log error
        close();
      } else {
        readHandshakeResponse();
      }
    });
}

void BackendConnection::readHandshakeResponse() {  
  boost::asio::async_read(
    socket,
    boost::asio::buffer(&shakehandResponse, sizeof(shakehandResponse)),
    [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (!ec) {
        response->onDisconnect();
        // TODO log error
        close();        
      } else {
        if (response->releaseAfterHandshake()) {
          backend->release();
        }
        connected = true;
        
        response->sendHandshakeResponse(shakehandResponse);
        response = nullptr;
      }      
    });
  }

void BackendConnection::executeRequest(const IPC::Message& request_, IPC::Response& response_) {
  request = &request_;
  response = &response_;
  writeRequestHeader();
}

void BackendConnection::writeRequestHeader() {
  boost::asio::async_write(
    socket, 
    boost::asio::buffer(request->getHeader(), request->getHeaderSize()),
    [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      handshakeRequest = nullptr;
      if (!ec) {
        // TODO log error
        close();
      } else {
        writeRequestBody();
      }
    });
}

void BackendConnection::writeRequestBody() {
  boost::asio::async_write(
    socket, 
    boost::asio::buffer(request->getHeader(), request->getHeaderSize()),
    [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      handshakeRequest = nullptr;
      if (!ec) {
        // TODO log error
        close();
      } else {
        readResponseHeader();
      }
    });
}

void BackendConnection::readResponseHeader() {

}

void BackendConnection::readResponseBody() {

}
  
  
} // namespace IPC
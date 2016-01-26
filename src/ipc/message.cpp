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

#include <utility>
#include "message.hpp"

namespace {
  constexpr std::size_t MESSAGE_SIZE_OFFSET = 4;  
}

namespace IPC {

Message::Message(BufferPool& bufferPool_)
  : bufferPool{bufferPool_} {
}

Message::Message(Message&& other)
  : bufferPool{other.bufferPool},
    header{std::move(other.header)},
    bodySize{other.bodySize},
    body{std::move(other.body)} {
  other.bodySize = 0;
}

Message::~Message() {
  releaseBodyBuffer();
  }

void Message::parseBodySize() {
  bodySize = header.getBodySize();
}

void Message::ensureBodyBufferCapacity() {
  uint32_t tmp = body.empty() ? 0 : body.size() * body.front()->size();
  
  while(tmp < bodySize) {
    buffer b = bufferPool.allocate();    
    tmp += sizeof(*b);
    body.emplace_back(std::move(b));
  }
}

void Message::releaseBodyBuffer() {
//  for(auto b : body) {
//    bufferPool.deallocate(b);
//  }
  body.clear();  
}

} // namespace IPC
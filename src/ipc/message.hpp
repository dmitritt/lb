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

#ifndef IPC_MESSAGE_HPP
#define IPC_MESSAGE_HPP

#include "bufferpool.hpp"

namespace IPC {
  
class Message {
public:
  static constexpr std::size_t HEADER_SIZE = 8;
  
  explicit Message(BufferPool& bufferPool);
  Message(const Message& orig) = delete;
  Message(Message&& other);
  
  ~Message(); 
  
  char* getHeader() {return header;}
  uint32_t getHeaderSize() {return HEADER_SIZE;}
  void parseBodySize();
  void ensureBodyBufferCapacity();
  uint32_t getBodySize() {return bodySize;}
  std::vector<buffer>& getBody() {return body;}
  void releaseBodyBuffer();
private:
  BufferPool& bufferPool;
  char header[HEADER_SIZE];
  uint32_t bodySize;
  std::vector<buffer> body;
};

}
#endif /* IPC_MESSAGE_HPP */


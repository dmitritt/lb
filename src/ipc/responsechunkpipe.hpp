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

#ifndef IPC_BUFFER_PIPE_HPP
#define IPC_BUFFER_PIPE_HPP

#include <atomic>
#include <functional>
#include <memory>
#include <utility>
#include <boost/lockfree/spsc_queue.hpp>
#include "bufferpool.hpp"
#include "header.hpp"
#include "types.hpp"

namespace IPC {

class ResponseChunkPipe {
public:
  typedef std::function<void(std::pair<buffer,message_size_t>&)> ChunkProcessor;
  
  ResponseChunkPipe(ChunkProcessor);
  ResponseChunkPipe(const ResponseChunkPipe& other) = delete;
  ResponseChunkPipe& operator=(const ResponseChunkPipe& other) = delete;
  
  void setBytesToSend(message_size_t count);
  void put(buffer chunk);
  std::pair<buffer,message_size_t> get();
private:
  ChunkProcessor chunkProcessor;
  std::atomic<bool> sending;
  message_size_t bytesToGet;
    
  boost::lockfree::spsc_queue<buffer> chunks;
};

} // namespace IPC

#endif /* IPC_BUFFER_PIPE_HPP */


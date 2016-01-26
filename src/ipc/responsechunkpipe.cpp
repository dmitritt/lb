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

#include "responsechunkpipe.hpp"

namespace IPC {

namespace {
  // TODO
  std::size_t RING_BUFFER_MAX_SIZE = 1024;
}  
  
ResponseChunkPipe::ResponseChunkPipe(ChunkProcessor chunkProcessor_)
  : chunkProcessor{chunkProcessor_},
    sending{false},
    bytesToGet{0},
    chunks{RING_BUFFER_MAX_SIZE}{
}

void ResponseChunkPipe::setBytesToSend(message_size_t count) {
  bytesToGet = count;
}

void ResponseChunkPipe::put(buffer chunk) {
  bool expected = false;
  if (!chunks.push(chunk)) {
    // TODO log
  } else if (sending.compare_exchange_strong(expected, true)) {
    auto r = get();
    if (r.second) {
      chunkProcessor(r);
    }
  }
}

std::pair<buffer,message_size_t> ResponseChunkPipe::get() {
  assert(sending);
  
  buffer b;
  message_size_t size = 0;
  if (!chunks.pop(b)) {
    sending = false;
  } else {
    size = (bytesToGet > b->size() ? b->size() : bytesToGet);
    bytesToGet -= size;
  }
  
  return std::make_pair(b, size);
}

} // namespace IPCres
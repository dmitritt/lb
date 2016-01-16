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

#ifndef IPC_BUFFER_POOL_HPP
#define IPC_BUFFER_POOL_HPP

#include <memory>
#include <vector>

namespace IPC {

static const std::size_t BUFFER_SIZE = 4 * 1024;

class BufferPool;

namespace priv {
  
struct BufferValue {
  char b[BUFFER_SIZE];
  char * get() {return b;}
};

class Deallocator;
}

typedef std::unique_ptr<priv::BufferValue,priv::Deallocator> buffer;

class BufferPool {
public:
  BufferPool();
  BufferPool(const BufferPool& orig) = delete;
  ~BufferPool();
  
  buffer allocate();
    
private:
  friend class priv::Deallocator;
  void deallocate(priv::BufferValue* bv);
//  std::vector<BufferValue*> freeBuffers;
public:
};

namespace priv {

class Deallocator {
public:
  Deallocator(BufferPool& parent_) : parent{parent_} {}
  void operator()(priv::BufferValue* bv) {parent.deallocate(bv);}    
private:
  BufferPool& parent;
};
  
}

} // namespace IPC

#endif /* BUFFERPOOL_HPP */


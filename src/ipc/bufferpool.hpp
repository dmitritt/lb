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

namespace detail {
  
struct BufferValue {
  char b[BUFFER_SIZE];
  char * get() {return b;}
  std::size_t size() {return BUFFER_SIZE;}
};

class Deallocator;
}

typedef std::shared_ptr<detail::BufferValue> buffer;

class BufferPool {
public:
  BufferPool();
  BufferPool(const BufferPool& other) = delete;
  BufferPool& operator=(const BufferPool& other) = delete;
  ~BufferPool();
  
  buffer allocate();
private:
  friend class detail::Deallocator;
  void deallocate(detail::BufferValue* bv);
};

namespace detail {

class Deallocator {
public:
  Deallocator(BufferPool& parent_) : parent{parent_} {}
  void operator()(detail::BufferValue* bv) {parent.deallocate(bv);}    
private:
  BufferPool& parent;
};
  
} // namespace detail

} // namespace IPC

#endif // IPC_BUFFER_POOL_HPP
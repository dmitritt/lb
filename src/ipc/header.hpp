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

#ifndef IPC_HEADER__HPP
#define IPC_HEADER__HPP

#include <boost/endian/conversion.hpp>
#include "types.hpp"

namespace IPC {

constexpr uint32_t HEADER_SIZE = 8;

/* 
 * First 8 bytes of an IPC message 
 * 0x01 - architecture used for encoding the message, big endian (0) or little endian (1)
 * 0x00 - message type (0 - async, 1 - sync, 2 - response)
 * 0x00 - non-compressed (0), compressed less than 4GB (1), compressed more than 4GB (2)
 * 0x00 - part of message length (for kdb 3.4)
 * 0x0d 00 00 00 - message length (13)
 */  
class Header {
private:
  static constexpr message_size_t MESSAGE_SIZE_OFFSET = 4; 
public:
  Header() = default;
  Header(Header&& other) {
    std::memcpy(h, other.h, sizeof(h));
    std::memset(other.h, '\0', sizeof(other.h));
  }
  char* get() {return h;}
  const char* get() const {return h;}
  static message_size_t size() {return sizeof(h);}
  operator char*() {return h;}
  operator const char*() const {return h;}
  message_size_t getBodySize() const {
    typedef message_size_t(*Converter)(message_size_t);
    // header[0] - big endian (0) or little endian (1)  
    Converter c = (h[0] ? 
      (Converter)&boost::endian::little_to_native : 
      (Converter)&boost::endian::big_to_native);  
  
    return c(*(reinterpret_cast<const message_size_t*>(h + MESSAGE_SIZE_OFFSET))) - HEADER_SIZE;
  }
private:  
  char h[HEADER_SIZE];
};


} // namespace IPC

#endif /* IPC_HEADER__HPP */


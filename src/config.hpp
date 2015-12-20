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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <utility>
#include <vector>

struct Address {
  const std::string host;
  const std::string port;
  const bool malformed;
  
  static Address create(const std::string& address) {
    auto colon = address.find(':');
    if (colon == std::string::npos) {
      return Address{"", "", true};
    } else {
      return Address{address.substr(0, colon), address.substr(colon+1)};
    }
  }
private:  
  Address(std::string&& host_, std::string&& port_, bool malformed_ = false)
    : host{std::move(host)},
      port{std::move(port)},
      malformed{malformed_} {}  
};

class Config {
public:
  enum USER_AUTH{PASS_THROUGH,LIB};
  enum Q_KILL{KILL_9,EXIT_0};

  Config(const std::string configFile) {}

  unsigned short getPort() const {return port;}
  unsigned short getThreads() const {return threads;}
  bool getEnableBackendBool() const {return enableBackendBool;}   
  const std::vector<Address>& getBackendAddresses() const {return backendAddresses;}
  
private:
  unsigned short port;
  unsigned short threads;
  USER_AUTH userAuthentication;
  bool enableBackendBool;
  unsigned short minPooledBackendCount; 
  unsigned short maxPooledBackendCount;
  std::string qCommand;
  Q_KILL qKill;
  std::vector<Address> backendAddresses;    
};

#endif /* CONFIG_HPP */


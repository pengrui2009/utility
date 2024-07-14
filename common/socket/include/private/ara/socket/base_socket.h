/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-01-11 17:34:19
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-08 17:41:22
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */
#pragma once

#include <boost/asio.hpp>

namespace ara {
namespace socket {

using boost::asio::io_context;
using boost::asio::thread_pool;
using boost::asio::local::stream_protocol;

class BaseSocket : private boost::asio::noncopyable {
 protected:
  /* data */
  io_context io_context_;
  /* number of threads for running io_text */
  uint32_t thread_nums_;
  /* thread pool used for io_text.run */
  thread_pool pool_;

 public:
  void Run();
  void Stop();
  BaseSocket(uint32_t n_threads);
  virtual ~BaseSocket();
};

}  // namespace socket
}  // namespace ara

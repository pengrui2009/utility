/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-01-11 17:32:56
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-14 16:49:57
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */
#include "ara/socket/base_socket.h"

#include <pthread.h>

namespace ara {
namespace socket {

BaseSocket::BaseSocket(uint32_t n_threads)
    : thread_nums_(n_threads), pool_(n_threads){};

void BaseSocket::Run() {
  for (uint32_t i = 0; i < thread_nums_; i++) {
    boost::asio::post(pool_, [this, i]() {
      pthread_setname_np(
          pthread_self(),
          std::string("ara-socket-" + std::to_string(i)).c_str());
      io_context_.run();
    });
  }
}

void BaseSocket::Stop() {
  io_context_.stop();
  // stop all pending handlers
  pool_.stop();
  pool_.join();
}

BaseSocket::~BaseSocket() {
  io_context_.stop();
  // wait all pending handlers
  pool_.join();
}

}  // namespace socket
}  // namespace ara

/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-17 15:47:31
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-06-13 10:24:28
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/src/client_domain_socket.cc
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include "ara/ipc/client_domain_socket.h"

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include "ara/ipc/connection_manager.h"
#include "ara/ipc/domain_socket.h"
#include "ara/ipc/ipc_logger.h"
#include "ara/ipc/ipc_message.h"
#include "ara/ipc/new_ipc_message.h"
#include "ara/ipc/request.h"

namespace ara {
namespace ipc {
IpcLogger logger;
using boost::asio::local::stream_protocol;
template <typename T>
using promise_type = ara::core::Promise<T>;

template <typename T>
class ClientDomainSocket<T>::pImpl : public BaseDomainSocket {
 private:
  bool first_connect = false;
  uint32_t reconnect_counts = 0;
  handler_manager<T> manager_;
  io_context io_context_conn_;
  stream_protocol::socket socket_;
  // boost::asio::strand<boost::asio::io_context::executor_type> strand_1_;
  boost::asio::io_service::strand strand_1_;
  // boost::asio::strand<boost::asio::io_context::executor_type> strand_2_;
  boost::asio::io_service::strand strand_2_;
  std::atomic_bool connected{false};
  std::string real_socket_path = "/tmp";
  stream_protocol::endpoint endpoint_;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work;
  uint32_t request_id = 0;

 public:
  pImpl(uint32_t n_threads, const std::string socket_file)
      : BaseDomainSocket(n_threads),
        socket_(io_context_),
        // strand_1_(io_context_.get_executor()),
        strand_1_(io_context_),
        // strand_2_(io_context_.get_executor()),
        strand_2_(io_context_),
        real_socket_path(
            std::string(BaseDomainSocket::getDomainSocketFilePrefixPath()) +
            "/" + socket_file),
        endpoint_(real_socket_path),
        work(boost::asio::make_work_guard(io_context_)) {}

  ~pImpl() {
    manager_.stop_all();
    work.reset();
    if (socket_.is_open()) {
      socket_.close();
    }
  }

  bool ConnectToServer() {
    pthread_setname_np(pthread_self(), "ara-ipc-connect");
#ifdef __QNX__
    char* environment_sock_prefix = getenv("AP_DOMAIN_SOCKET_SOCK");
    if (environment_sock_prefix) {
      if (setsockprefix(environment_sock_prefix) == NULL) {
        // set socket prefix error
        logger.LogWarn() << "IPC client domain socket set sock prefix:"
                         << environment_sock_prefix
                         << "error. Use default sock";
      } else {
        logger.LogInfo() << "IPC client domain socket sock prefix set to:"
                         << environment_sock_prefix;
      }
    }
#endif
    if (!connected) {
      disconnect();
      if (io_context_.stopped()) {
        io_context_.restart();
      }
      Run();
      // post do_connect to io_context_conn_
      boost::asio::post(io_context_conn_, std::bind(&pImpl::do_connect, this));
      reconnect_counts = 0;
      if (io_context_conn_.stopped()) {
        io_context_conn_.restart();
      }
      // run do_connect in this thread
      io_context_conn_.run();
      return connected;
    } else {
      return true;
    }
  }

  std::future<bool> connect() {
    return std::async(std::launch::async, &pImpl::ConnectToServer, this);
  }

  void do_connect() {
    try {
      socket_.async_connect(
          endpoint_,
          boost::asio::bind_executor(
              io_context_conn_, [this](boost::system::error_code ec) {
                if (!ec) {
                  reconnect_counts = 0;
                  connected = true;
                  logger.LogInfo() << "client connected to server";
                } else {
                  logger.LogWarn()
                      << ec.message() << " try reconnect:" << reconnect_counts;
                  connected = false;
                  reconnect_counts++;
                  if (reconnect_counts < 3) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    do_connect();
                  } else {
                    logger.LogWarn() << "connect failure";
                  }
                }
              }));
    } catch (boost::system::system_error& e) {
      logger.LogWarn() << e.what();
      connected = false;
    }
  }

  bool Connected() const noexcept { return connected; }

  void disconnect() {
    try {
      if (!io_context_.stopped()) {
        io_context_.stop();
      }
      if (!io_context_conn_.stopped()) {
        io_context_conn_.stop();
      }
      connected = false;
      manager_.stop_all();
      if (socket_.is_open()) {
        socket_.close();
      }
    } catch (const std::exception& e) {
      logger.LogWarn() << "close client error" << e.what();
    }
  }

  future_type<T> Send(T& msg) {
    promise_type<T> p;
    auto ret = p.get_future();
    manager_.start(std::shared_ptr<Handler<T>>(
        new request<T>(++request_id, socket_, connected, strand_1_, strand_2_,
                       std::move(p), msg, manager_)));
    return ret;
  }
};

template <typename T>
ClientDomainSocket<T>::ClientDomainSocket(uint32_t n_threads,
                                          const std::string socket_file)
    : pImpl_(std::make_unique<pImpl>(n_threads, socket_file)){};

template <typename T>
ClientDomainSocket<T>::~ClientDomainSocket(){};

template <typename T>
std::future<bool> ClientDomainSocket<T>::connect() {
  return pImpl_->connect();
}

template <typename T>
void ClientDomainSocket<T>::disconnect() {
  pImpl_->disconnect();
}

template <typename T>
bool ClientDomainSocket<T>::Connected() const noexcept {
  // return connected;
  return pImpl_->Connected();
}

template <typename T>
future_type<T> ClientDomainSocket<T>::Send(T& msg) {
  return pImpl_->Send(msg);
}

template class ClientDomainSocket<IPCMessage>;
template class ClientDomainSocket<NewIpcMessage>;

}  // namespace ipc
}  // namespace ara
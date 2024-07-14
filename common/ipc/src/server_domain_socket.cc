/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-17 15:30:19
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-06-13 10:22:59
 * @FilePath:
 * /aeg-adaptive-autosar/ara-api/common/ipc/src/server_domain_socket.cc
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#include "ara/ipc/server_domain_socket.h"

#include <boost/asio/socket_base.hpp>
#include <cstdio>

#include "ara/ipc/connection.h"
#include "ara/ipc/connection_manager.h"
#include "ara/ipc/domain_socket.h"
#include "ara/ipc/ipc_logger.h"
#include "ara/ipc/ipc_message.h"
#include "ara/ipc/new_ipc_message.h"
#ifdef __QNX__
#include <sys/socket.h>
#endif

namespace ara {
namespace ipc {
extern IpcLogger logger;

using boost::asio::local::stream_protocol;

template <typename T>
class ServerDomainSocket<T>::pImpl : public BaseDomainSocket {
 private:
  handler_manager<T> manager_;
  /* This acceptor will be bound to a unix socket when initialized */
  stream_protocol::acceptor acceptor_;
  uint32_t connection_nums_;
  std::string real_socket_path = "/tmp";
  stream_protocol::endpoint endpoint_;
  ProcessHandlerPtr<T> handler_;

 public:
  pImpl(uint32_t n_threads, const std::string socket_file,
        ProcessHandlerPtr<T> handler)
      : BaseDomainSocket(n_threads),
        acceptor_(io_context_),
        real_socket_path(
            std::string(BaseDomainSocket::getDomainSocketFilePrefixPath()) +
            "/" + socket_file),
        endpoint_(real_socket_path),
        handler_(handler) {
    // remove the socket file before acceptor is bound to it
    std::remove(real_socket_path.c_str());
#ifdef __QNX__
    auto p_set_sock_ =
        std::unique_ptr<std::promise<bool>>(new std::promise<bool>);
    auto f_set_sock_ = p_set_sock_->get_future();
    boost::asio::post(pool, [this, p = std::move(p_set_sock_)]() {
      char* environment_sock_prefix = getenv("AP_DOMAIN_SOCKET_SOCK");
      if (environment_sock_prefix) {
        if (setsockprefix(environment_sock_prefix) == NULL) {
          // set socket prefix error
          logger.LogWarn() << "IPC server domain socket set sock prefix:"
                           << environment_sock_prefix
                           << "error. Use default sock";
        } else {
          logger.LogInfo() << "IPC server domain socket sock prefix set to:"
                           << environment_sock_prefix;
        }
      }
      set_socket();
      p->set_value(true);
    });
    f_set_sock_.get();
#else
    set_socket();
#endif
    do_accept();
  }

  ~pImpl() {
    manager_.stop_all();
    if (acceptor_.is_open()) {
      acceptor_.close();
    }
    unlink(real_socket_path.c_str());
  }

  void set_socket() {
    acceptor_.open(endpoint_.protocol());
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
    acceptor_.bind(endpoint_);
    acceptor_.listen();
  }

  void do_accept() {
    acceptor_.async_accept(
        [this](std::error_code ec, stream_protocol::socket socket) {
          // Check whether the server was stopped by a signal before this
          // completion handler had a chance to run.
          if (!acceptor_.is_open()) {
            return;
          }

          if (!ec) {
            logger.LogInfo() << "server accept connections:" << manager_.size();
            manager_.start(std::shared_ptr<Handler<T>>(
                new connection<T>(std::move(socket), manager_, handler_)));
          }
          do_accept();
        });
  }
  uint32_t Connections() { return manager_.size(); }
};

template <typename T>
ServerDomainSocket<T>::ServerDomainSocket(uint32_t n_threads,
                                          const std::string socket_file,
                                          ProcessHandlerPtr<T> handler)
    : pImpl_(std::make_unique<pImpl>(n_threads, socket_file, handler)){};

template <typename T>
uint32_t ServerDomainSocket<T>::Connetions() {
  return pImpl_->Connections();
}

template <typename T>
void ServerDomainSocket<T>::Run() {
  pImpl_->Run();
}

template <typename T>
void ServerDomainSocket<T>::Stop() {
  pImpl_->Stop();
}

template <typename T>
ServerDomainSocket<T>::~ServerDomainSocket() {}

template class ServerDomainSocket<IPCMessage>;
template class ServerDomainSocket<NewIpcMessage>;

}  // namespace ipc
}  // namespace ara
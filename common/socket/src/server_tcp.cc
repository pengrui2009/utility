/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-01-12 16:25:20
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-06-13 10:38:50
 * @Description:
 *
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */
#include "ara/socket/server_tcp.h"

// #include <boost/asio/ip/address.hpp>
// #include <boost/asio/ip/tcp.hpp>

#include "ara/socket/base_socket.h"
#include "ara/socket/connection_manager.h"
#include "ara/socket/logger.h"
#include "ara/socket/util.h"

namespace ara {
namespace socket {
SocketLogger logger;
namespace tcp {

class ServerTcp::pImpl : public BaseSocket {
 private:
  std::string ip_;
  uint32_t port_;
  ::boost::asio::ip::tcp::endpoint endpoint_;
  ::boost::asio::ip::tcp::acceptor acceptor_;
  ConnectionManager connection_manager_;
  std::function<void(const ConnectionPtr& conn)> func_ = NULL;

 public:
  pImpl(const std::string& ip, const uint32_t& port,
        const uint32_t& thread_nums)
      : BaseSocket(thread_nums),
        ip_(ip),
        port_(port),
        endpoint_(::boost::asio::ip::make_address(ip), port_),
        acceptor_(io_context_) {
    std::string device = GetNameFromIp(ip_);
    acceptor_.open(endpoint_.protocol());
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
    if (device != "") {
      if (setsockopt(acceptor_.native_handle(), SOL_SOCKET, SO_BINDTODEVICE,
                     device.c_str(), device.size()) == -1) {
        logger.LogWarn() << "TCP Server: Could not bind to device:" << device
                         << " Error:" << strerror(errno);
      } else {
        logger.LogInfo() << "TCP Server: Socket bind to device:" << device;
      }
    }
    acceptor_.bind(endpoint_);
    acceptor_.listen();
    do_accept();
  }

  pImpl(const uint32_t& port, const uint32_t& thread_nums)
      : BaseSocket(thread_nums),
        ip_("0.0.0.0"),
        port_(port),
        endpoint_(
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_)),
        acceptor_(io_context_) {
    std::string device = GetNameFromIp(ip_);
    acceptor_.open(endpoint_.protocol());
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
    if (device != "") {
      if (setsockopt(acceptor_.native_handle(), SOL_SOCKET, SO_BINDTODEVICE,
                     device.c_str(), device.size()) == -1) {
        logger.LogWarn() << "TCP Server: Could not bind to device:" << device
                         << " Error:" << strerror(errno);
      } else {
        logger.LogInfo() << "TCP Server: Socket bind to device:" << device;
      }
    }
    acceptor_.bind(endpoint_);
    acceptor_.listen();
    do_accept();
  }
  ~pImpl() { Close(); }

  void SetAcceptorHandler(
      std::function<void(const ConnectionPtr& conn)>& func) {
    func_ = func;
  }

  void do_accept() {
    acceptor_.async_accept([this](::boost::system::error_code ec,
                                  ::boost::asio::ip::tcp::socket socket) {
      // Check whether the server was stopped by a signal before this
      // completion handler had a chance to run.
      if (!acceptor_.is_open()) {
        return;
      }
      if (!ec) {
        connection_manager_.start(std::move(socket), func_);
      }
      do_accept();
    });
  }

  std::string Mac() { return GetMacFromIp(ip_); }

  void Close() {
    connection_manager_.stop_all();
    acceptor_.close();
  }
};

ServerTcp::ServerTcp(const std::string& ip, const uint32_t& port,
                     const uint32_t& thread_nums)
    : pImpl_(std::make_unique<pImpl>(ip, port, thread_nums)){};

ServerTcp::ServerTcp(const uint32_t& port, const uint32_t& thread_nums)
    : pImpl_(std::make_unique<pImpl>(port, thread_nums)){};

void ServerTcp::SetAcceptorHandler(
    std::function<void(const ConnectionPtr& conn)> func) {
  pImpl_->SetAcceptorHandler(func);
}

ServerTcp::~ServerTcp() { pImpl_->Stop(); }

void ServerTcp::Run() { pImpl_->Run(); };

std::string ServerTcp::Mac() { return pImpl_->Mac(); }

void ServerTcp::Close() { return pImpl_->Close(); }

}  // namespace tcp
}  // namespace socket
}  // namespace ara
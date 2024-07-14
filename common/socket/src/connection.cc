/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2023-01-12 17:07:16
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-06-13 10:37:42
 * @Description:
 *
 * Copyright (c) 2023 by Tusimple, All Rights Reserved.
 */

#include <ara/core/promise.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <functional>

#include "ara/socket/connection_manager.h"
#include "ara/socket/logger.h"

namespace ara {
namespace socket {
extern SocketLogger logger;
namespace tcp {

using tcp = boost::asio::ip::tcp;

template <typename T>
// using promise_type = std::promise<T>;
using promise_type = ara::core::Promise<T>;

/********************************************Connection::pImpl********************************************/

class Connection::pImpl : public std::enable_shared_from_this<pImpl> {
 public:
  tcp::socket socket_;

 private:
  /// The manager for this connection.
  ConnectionManager& connection_manager_;
  Connection& conn_;

  /// Buffer for incoming data.
  SocketBufferType data_;
  static const uint32_t max_size = 10024U;

 public:
  pImpl(tcp::socket socket, ConnectionManager& manager, Connection& conn)
      : socket_(std::move(socket)),
        connection_manager_(manager),
        conn_(conn),
        data_(make_buffer(max_size)) {}

  ~pImpl() {
    if (socket_.is_open()) {
      logger.LogDebug() << "close socket";
      socket_.close();
    }
  };

  // void start() { do_read(); };

  void start(){};

  void close() {
    if (socket_.is_open()) {
      socket_.close();
    }
  };

  void stop() { connection_manager_.stop(conn_.shared_from_this()); };

  // void do_read() {
  //   boost::asio::async_read(
  //       socket_, boost::asio::buffer(buffer_),
  //       boost::asio::transfer_exactly(14),
  //       std::bind(&pImpl::read_payload, this, std::placeholders::_1));
  // };

  void do_write();

  // void read_payload(const boost::system::error_code& ec) {
  //   if (!ec) {
  //     logger.LogInfo() << "remote_endpoint:" << Ip();
  //     do_read();
  //   } else if (ec != ::boost::asio::error::operation_aborted) {
  //     logger.LogWarn() << ec.message();
  //     connection_manager_.stop(conn_.shared_from_this());
  //   } else {
  //     do_read();
  //   }
  // };

  void write(std::unique_ptr<promise_type<std::size_t>>&& p, const void* data,
             std::size_t size) {
    boost::asio::async_write(
        socket_, boost::asio::const_buffer(data, size),
        boost::asio::transfer_exactly(size),
        [p1 = std::move(p)](const boost::system::error_code& ec,
                            std::size_t t) {
          if (!ec) {
            logger.LogVerbose() << "connection write message. size:" << t;
            p1->set_value(t);
          } else {
            p1->SetError(static_cast<ara::core::CoreErrc>(ec.value()));
            logger.LogWarn() << ec.message();
          }
        });
  }

  void write(std::unique_ptr<promise_type<std::size_t>>&& p,
             SocketBufferType& data) {
    write(std::move(p), data.Data(), data.Size());
  }

  void write(std::unique_ptr<promise_type<std::size_t>>&& p,
             SocketBufferType&& data) {
    auto r_data = std::move(data);
    write(std::move(p), r_data);
  }

  void read(std::unique_ptr<promise_type<std::size_t>>&& p, void* data,
            std::size_t size) {
    boost::asio::async_read(
        socket_, boost::asio::buffer(data, size),
        boost::asio::transfer_exactly(size),
        [&data, size, p1 = std::move(p)](const boost::system::error_code& ec,
                                         std::size_t length) {
          if (!ec) {
            logger.LogVerbose()
                << "connection read message(void*). size:" << size;
            p1->set_value(size);
          } else {
            p1->SetError(static_cast<ara::core::CoreErrc>(ec.value()));
            logger.LogWarn() << ec.message();
          }
        });
  }

  void read(std::unique_ptr<promise_type<SocketBufferType>>&& p,
            std::size_t size) {
    auto data_ptr = std::make_shared<SocketBufferType>(make_buffer(size));
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(static_cast<void*>(data_ptr->Data()),
                            data_ptr->Size()),
        boost::asio::transfer_exactly(size),
        [data_ptr, p1 = std::move(p)](const boost::system::error_code& ec,
                                      std::size_t length) mutable {
          if (!ec) {
            logger.LogVerbose() << "connection read message. size:" << length;
            p1->set_value(std::move(*data_ptr));
          } else {
            p1->SetError(static_cast<ara::core::CoreErrc>(ec.value()));
            logger.LogWarn() << ec.message();
          }
        });
  }

  void readsome(std::unique_ptr<promise_type<std::size_t>>&& p, void* data,
                std::size_t max_size) {
    socket_.async_read_some(
        boost::asio::buffer(data, max_size),
        [p1 = std::move(p)](const boost::system::error_code& ec,
                            std::size_t t) {
          if (!ec) {
            logger.LogVerbose()
                << "connection readsome message(void*). size:" << t;
            p1->set_value(t);
          } else {
            p1->SetError(static_cast<ara::core::CoreErrc>(ec.value()));
            logger.LogWarn() << ec.message();
          }
        });
  }

  void readsome(std::unique_ptr<promise_type<SocketBufferType>>&& p) {
    socket_.async_read_some(
        boost::asio::buffer(static_cast<void*>(data_.Data()), data_.Size()),
        [this, p1 = std::move(p)](const boost::system::error_code& ec,
                                  std::size_t t) {
          if (!ec) {
            auto data = make_buffer(t);
            data.CopyFrom(data_.Data());
            logger.LogVerbose() << "connection readsome message. size:" << t;
            p1->set_value(std::move(data));
          } else {
            p1->SetError(static_cast<ara::core::CoreErrc>(ec.value()));
            logger.LogWarn() << ec.message();
          }
        });
  }

  const std::string Ip() {
    return socket_.remote_endpoint().address().to_string();
  }

  const std::string LocalIp() {
    return socket_.local_endpoint().address().to_string();
  }

  const int32_t Protocol() {
    return socket_.remote_endpoint().protocol().protocol();
  }

  const uint16_t Port() { return socket_.remote_endpoint().port(); }

  const uint16_t LocalPort() { return socket_.local_endpoint().port(); }

  const int32_t SockFd() { return socket_.native_handle(); }
};

/********************************************Connection********************************************/

Connection::Connection() {}

// void Connection::start() { pImpl_->start(); }

void Connection::Stop() { pImpl_->stop(); }

future_type<std::size_t> Connection::Write(SocketBufferType& data) {
  return Write(data.Data(), data.Size());
}

future_type<std::size_t> Connection::Write(SocketBufferType&& data) {
  auto p_send =
      std::make_unique<promise_type<std::size_t>>(promise_type<std::size_t>());
  auto f = p_send->get_future();
  pImpl_->write(std::move(p_send), std::move(data));
  return f;
}

future_type<std::size_t> Connection::Write(const void* data, std::size_t size) {
  auto p_send =
      std::make_unique<promise_type<std::size_t>>(promise_type<std::size_t>());
  auto f = p_send->get_future();
  pImpl_->write(std::move(p_send), data, size);
  return f;
}

future_type<SocketBufferType> Connection::Readsome() {
  auto p_read = std::make_unique<promise_type<SocketBufferType>>(
      promise_type<SocketBufferType>());
  auto f = p_read->get_future();
  // auto buf = make_buffer(SocketBufferType::max_size);
  pImpl_->readsome(std::move(p_read));
  return f;
}

future_type<std::size_t> Connection::Readsome(void* data,
                                              std::size_t max_size) {
  auto p_read =
      std::make_unique<promise_type<std::size_t>>(promise_type<std::size_t>());
  auto f = p_read->get_future();
  pImpl_->readsome(std::move(p_read), data, max_size);
  return f;
}

future_type<SocketBufferType> Connection::Read(std::size_t size) {
  auto p_read = std::make_unique<promise_type<SocketBufferType>>(
      promise_type<SocketBufferType>());
  auto f = p_read->get_future();
  pImpl_->read(std::move(p_read), size);
  return f;
}

future_type<std::size_t> Connection::Read(void* data, std::size_t size) {
  auto p_read =
      std::make_unique<promise_type<std::size_t>>(promise_type<std::size_t>());
  auto f = p_read->get_future();
  pImpl_->read(std::move(p_read), data, size);
  return f;
}

const std::string Connection::Ip() { return pImpl_->Ip(); };

const std::string Connection::LocalIp() { return pImpl_->LocalIp(); };

const int32_t Connection::Protocol() { return pImpl_->Protocol(); };

const uint16_t Connection::Port() { return pImpl_->Port(); };

const uint16_t Connection::LocalPort() { return pImpl_->LocalPort(); };

const int32_t Connection::UniqueId() { return pImpl_->SockFd(); };

/********************************************ConnectionManager********************************************/

ConnectionManager::ConnectionManager() {}

void ConnectionManager::start(
    boost::asio::ip::tcp::socket&& socket,
    std::function<void(const ConnectionPtr& conn)>& handler) {
  // construct Connection instance
  // std::make_shared<Connection>() is not to be used. Because the conn will be
  // constructed.
  auto sptr_conn = std::shared_ptr<Connection>(new Connection);
  std::unique_ptr<Connection::pImpl> uptr_impl =
      std::make_unique<Connection::pImpl>(std::move(socket), *this, *sptr_conn);
  sptr_conn->pImpl_ = std::move(uptr_impl);
  {
    std::lock_guard<std::mutex> lk(mtx_);
    connections_.insert(sptr_conn);
  }

  // sptr_conn->start();
  if (handler != NULL) {
    handler(sptr_conn);
  }
}

void ConnectionManager::stop(ConnectionPtr c) {
  std::lock_guard<std::mutex> lk(mtx_);
  c->pImpl_->close();
  connections_.erase(c);
}

void ConnectionManager::stop_all() {
  std::lock_guard<std::mutex> lk(mtx_);
  for (auto c : connections_) c->pImpl_->close();
  connections_.clear();
}

}  // namespace tcp
}  // namespace socket
}  // namespace ara
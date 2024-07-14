/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-16 19:02:12
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-07-31 15:53:15
 * @FilePath: /aeg-adaptive-autosar/ara-api/common/ipc/src/connection.cc
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include "ara/ipc/connection.h"

#include "ara/ipc/connection_manager.h"
#include "ara/ipc/ipc_logger.h"
#include "ara/ipc/ipc_message.h"
#include "ara/ipc/new_ipc_message.h"

namespace ara {
namespace ipc {
extern IpcLogger logger;

template <typename T>
using connection_ptr = std::shared_ptr<connection<T>>;

template <typename T>
connection<T>::connection(stream_protocol::socket socket,
                          handler_manager<T>& manager,
                          ProcessHandlerPtr<T>& handler)
    : Handler<T>(manager),
      socket_(std::move(socket)),
      process_handler_(handler),
      buffer_() {}

template <typename T>
void connection<T>::start() {
  do_read();
}

template <typename T>
void connection<T>::stop() {
  if (socket_.is_open()) {
    socket_.close();
  }
}

template <typename T>
void connection<T>::read_header(const boost::system::error_code& ec) {
  if (!ec) {
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(buffer_.payload(), buffer_.payload_length()),
        std::bind(&connection::read_payload, shared_from_this(),
                  std::placeholders::_1));
  } else {
    logger.LogWarn() << "server: read header error:" << ec.message();
    if (ec != boost::asio::error::operation_aborted) {
      logger.LogInfo() << "server close connection.";
      Handler<T>::handler_manager_.stop(shared_from_this());
    } else {
      do_read();
    }
  }
}

template <>
void connection<NewIpcMessage>::read_header(
    const boost::system::error_code& ec) {
  if (!ec) {
    buffer_.decode_header();
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(buffer_.payload(), buffer_.payload_length()),
        std::bind(&connection::read_payload, shared_from_this(),
                  std::placeholders::_1));
  } else {
    logger.LogWarn() << "server: read header error:" << ec.message();
    if (ec != boost::asio::error::operation_aborted) {
      logger.LogInfo() << "server close connection.";
      Handler<NewIpcMessage>::handler_manager_.stop(shared_from_this());
    } else {
      do_read();
    }
  }
}

template <typename T>
void connection<T>::read_payload(const boost::system::error_code& ec) {
  if (!ec) {
    if (buffer_.ipc_mode() == IPCMessageMode::RPC) {
      auto msg = process_handler_->OnReadCallBack(buffer_);
      do_write(msg);
    } else if (buffer_.ipc_mode() == IPCMessageMode::FIRE_AND_FORGET) {
      // ignore the return value
      (void)process_handler_->OnReadCallBack(buffer_);
      do_read();
    }
  } else {
    logger.LogWarn() << "server: read payload error:" << ec.message();
    if (ec != boost::asio::error::operation_aborted) {
      logger.LogWarn() << "server close connection.";
      Handler<T>::handler_manager_.stop(shared_from_this());
    } else {
      do_read();
    }
  }
}

template <typename T>
void connection<T>::do_read() {
  boost::asio::async_read(
      socket_, boost::asio::buffer(buffer_.header(), T::header_length),
      std::bind(&connection::read_header, shared_from_this(),
                std::placeholders::_1));
};

template <typename T>
void connection<T>::do_write(T& msg) {
  boost::system::error_code ec;
  for (;;) {
    boost::asio::write(
        socket_, boost::asio::const_buffer(msg.header(), T::header_length),
        boost::asio::transfer_exactly(T::header_length), ec);
    if (ec) break;
    boost::asio::write(
        socket_, boost::asio::const_buffer(msg.payload(), msg.payload_length()),
        boost::asio::transfer_exactly(msg.payload_length()), ec);
    if (ec)
      break;
    else
      do_read();
    return;
  }

  if (ec != boost::asio::error::operation_aborted) {
    logger.LogWarn() << "server: send response error:" << ec.message()
                     << "Close this connection.";
    Handler<T>::handler_manager_.stop(shared_from_this());
  } else {
    logger.LogWarn() << "server: send response error:" << ec.message();
    do_read();
  }
}

template class connection<IPCMessage>;
template class connection<NewIpcMessage>;

}  // namespace ipc
}  // namespace ara
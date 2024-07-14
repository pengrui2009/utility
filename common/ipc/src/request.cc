/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-08-23 11:51:39
 * @LastEditors: dongdong.sun dongdong.sun@tusen.ai
 * @LastEditTime: 2023-08-21 09:19:49
 * @FilePath: /aeg-adaptive-autosar/ara-api/common/ipc/src/request.cc
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */
#include "ara/ipc/request.h"

#include "ara/ipc/connection_manager.h"
#include "ara/ipc/ipc_logger.h"
#include "ara/ipc/ipc_message.h"
#include "ara/ipc/new_ipc_message.h"

namespace ara {
namespace ipc {
extern IpcLogger logger;

template <typename T>
request<T>::request(uint32_t id, stream_protocol::socket& socket,
                    std::atomic_bool& connected,
                    boost::asio::io_service::strand& strand_write,
                    boost::asio::io_service::strand& strand_read,
                    promise_type<T>&& p, T& req_msg,
                    handler_manager<T>& manager)
    : Handler<T>(manager),
      request_id(id),
      socket_(socket),
      connected_(connected),
      strand_write_(strand_write),
      strand_read_(strand_read),
      promise_(std::move(p)),
      req_msg_(req_msg),
      rec_msg_() {
  logger.LogDebug() << this << " create request: rec_msg:"
                    << static_cast<void*>(&rec_msg_)
                    << "rec_msg.data:" << static_cast<void*>(rec_msg_.header())
                    << " req_msg:" << static_cast<void*>(&req_msg_)
                    << "req_msg.data:" << static_cast<void*>(req_msg_.header());
};

template <typename T>
request<T>::~request() {
  logger.LogDebug() << this << "release request";
}

template <typename T>
void request<T>::start() {
  do_write(req_msg_);
}

template <typename T>
void request<T>::do_read() {
  boost::system::error_code ec;
  boost::asio::read(socket_,
                    boost::asio::buffer(rec_msg_.header(), T::header_length),
                    boost::asio::transfer_exactly(T::header_length), ec);
  if (!ec) {
    boost::asio::read(
        socket_,
        boost::asio::buffer(rec_msg_.payload(), rec_msg_.payload_length()),
        boost::asio::transfer_exactly(rec_msg_.payload_length()), ec);
    if (!ec) {
      logger.LogDebug() << "Client set promise: request id:" << request_id;
      promise_.set_value(std::move(rec_msg_));
    } else {
      logger.LogWarn() << "Client read payload error:" << ec.message()
                       << "request id:" << request_id;
      if (ec == boost::asio::error::broken_pipe) {
        connected_ = false;
        promise_.SetError(ara::core::CoreErrc::broken_pipe);
      } else {
        promise_.SetError(ara::core::CoreErrc::connection_aborted);
      }
    }

  } else {
    logger.LogWarn() << "Client read header error:" << ec.message()
                     << "request id:" << request_id;
    if (ec == boost::asio::error::broken_pipe) {
      connected_ = false;
      promise_.SetError(ara::core::CoreErrc::broken_pipe);
    } else {
      promise_.SetError(ara::core::CoreErrc::connection_aborted);
    }
  }
  /* stop handler whether success or failure */
  Handler<T>::handler_manager_.stop(shared_from_this());
}

template <>
void request<NewIpcMessage>::do_read() {
  boost::system::error_code ec;
  boost::asio::read(
      socket_,
      boost::asio::buffer(rec_msg_.header(), NewIpcMessage::header_length),
      boost::asio::transfer_exactly(NewIpcMessage::header_length), ec);
  if (!ec) {
    rec_msg_.decode_header();
    boost::asio::read(
        socket_,
        boost::asio::buffer(rec_msg_.payload(), rec_msg_.payload_length()),
        boost::asio::transfer_exactly(rec_msg_.payload_length()), ec);
    if (!ec) {
      logger.LogDebug() << "Client set promise: request id:" << request_id;
      promise_.set_value(std::move(rec_msg_));
    } else {
      logger.LogWarn() << "Client read payload error:" << ec.message()
                       << "request id:" << request_id;
      if (ec == boost::asio::error::broken_pipe) {
        connected_ = false;
        promise_.SetError(ara::core::CoreErrc::broken_pipe);
      } else {
        promise_.SetError(ara::core::CoreErrc::connection_aborted);
      }
    }

  } else {
    logger.LogWarn() << "Client read header error:" << ec.message()
                     << "request id:" << request_id;
    if (ec == boost::asio::error::broken_pipe) {
      connected_ = false;
      promise_.SetError(ara::core::CoreErrc::broken_pipe);
    } else {
      promise_.SetError(ara::core::CoreErrc::connection_aborted);
    }
  }
  /* stop handler whether success or failure */
  Handler<NewIpcMessage>::handler_manager_.stop(shared_from_this());
}

template <typename T>
void request<T>::write_msg(T& msg) {
  boost::system::error_code ec;

  for (;;) {
    ec = write_header(msg);
    if (ec) break;
    ec = write_payload(msg);
    if (ec)
      break;
    else
      boost::asio::post(strand_read_,
                        std::bind(&request::do_read, shared_from_this()));
    return;
  }

  logger.LogWarn() << "send error:" << ec.message()
                   << "request id:" << request_id;
  if (ec == boost::asio::error::broken_pipe) {
    connected_ = false;
    promise_.SetError(ara::core::CoreErrc::broken_pipe);
  } else {
    promise_.SetError(ara::core::CoreErrc::connection_aborted);
  }
  Handler<T>::handler_manager_.stop(shared_from_this());
}

template <typename T>
boost::system::error_code request<T>::write_header(T& msg) {
  boost::system::error_code ec;
  boost::asio::write(socket_,
                     boost::asio::const_buffer(msg.header(), T::header_length),
                     boost::asio::transfer_exactly(T::header_length), ec);
  return ec;
}

template <typename T>
boost::system::error_code request<T>::write_payload(T& msg) {
  boost::system::error_code ec;
  boost::asio::write(
      socket_, boost::asio::const_buffer(msg.payload(), msg.payload_length()),
      boost::asio::transfer_exactly(msg.payload_length()), ec);
  return ec;
}

template <typename T>
void request<T>::write_msg_without_response(T& msg) {
  boost::system::error_code ec;

  for (;;) {
    ec = write_header(msg);
    if (ec) break;
    ec = write_payload(msg);
    if (ec) {
      break;
    } else {
      T msg(IPCMessageMode::FIRE_AND_FORGET);
      promise_.set_value(std::move(msg));
    }
    Handler<T>::handler_manager_.stop(shared_from_this());
    return;
  }

  logger.LogWarn() << "send fire forget message error:" << ec.message()
                   << "request id:" << request_id;
  if (ec == boost::asio::error::broken_pipe) {
    connected_ = false;
    promise_.SetError(ara::core::CoreErrc::broken_pipe);
  } else {
    promise_.SetError(ara::core::CoreErrc::connection_aborted);
  }
  Handler<T>::handler_manager_.stop(shared_from_this());
}

template <typename T>
void request<T>::do_write(T& msg) {
  if (msg.ipc_mode() == IPCMessageMode::RPC) {
    logger.LogDebug() << "Client start write message. request id:"
                      << request_id;
    boost::asio::post(strand_write_,
                      std::bind(&request::write_msg, shared_from_this(), msg));
  } else if (msg.ipc_mode() == IPCMessageMode::FIRE_AND_FORGET) {
    logger.LogDebug()
        << "Client start write message without response. request id:"
        << request_id;
    boost::asio::post(strand_write_,
                      std::bind(&request::write_msg_without_response,
                                shared_from_this(), msg));
  } else {
    promise_.SetError(ara::core::CoreErrc::bad_message);
    logger.LogWarn() << "Unknown ipc message mode";
    Handler<T>::handler_manager_.stop(shared_from_this());
  }
}

template class request<IPCMessage>;
template class request<NewIpcMessage>;

}  // namespace ipc
}  // namespace ara
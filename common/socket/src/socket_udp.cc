#include "ara/socket/socket_udp.h"

#include <boost/asio/ip/multicast.hpp>
#include <boost/asio/ip/udp.hpp>

#include "ara/core/promise.h"
#include "ara/socket/base_socket.h"
#include "ara/socket/logger.h"
#include "ara/socket/util.h"

namespace ara {
namespace socket {
extern SocketLogger logger;
namespace udp {

template <typename T>
using promise_type = ara::core::Promise<T>;

class SocketUdp::pImpl final : public BaseSocket {
 private:
  std::string local_ip_;
  uint32_t local_port_;
  SocketBufferType data_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work_;
  boost::asio::ip::udp::endpoint local_endpoint_;
  boost::asio::ip::udp::endpoint remote_endpoint_;
  boost::asio::ip::udp::endpoint broadcast_endpoint_;
  std::function<void(SocketBufferType&&, const std::string&, const uint32_t&)>
      func_ = NULL;
  static const uint32_t max_size = 10024U;

 public:
  pImpl(const std::string& local_address, const uint32_t& local_port,
        const uint32_t& thread_nums)
      : BaseSocket(thread_nums),
        local_ip_(local_address),
        local_port_(local_port),
        data_(make_buffer(max_size)),
        socket_(io_context_),
        work_(boost::asio::make_work_guard(io_context_)) {
    local_endpoint_ =
        local_ip_ == "0.0.0.0"
            ? boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(),
                                             local_port)
            : boost::asio::ip::udp::endpoint(
                  boost::asio::ip::make_address(local_ip_), local_port);
    socket_.open(local_endpoint_.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket_.set_option(boost::asio::ip::udp::socket::broadcast(true));
    std::string device = GetNameFromIp(local_ip_);
    if (device != "") {
      if (setsockopt(socket_.native_handle(), SOL_SOCKET, SO_BINDTODEVICE,
                     device.c_str(), device.size()) == -1) {
        logger.LogWarn() << "UDP: Could not bind to device:" << device;
      } else {
        logger.LogInfo() << "UDP: Socket bind to device:" << device;
      }
    }
    logger.LogInfo() << "udp socket bind to local address: ip"
                     << local_endpoint_.address().to_string() << "port"
                     << local_endpoint_.port();
    socket_.bind(local_endpoint_);
    BaseSocket::Run();
  }

  pImpl(const std::string& local_address, const uint32_t& thread_nums)
      : pImpl(local_address, 0, thread_nums) {}

  pImpl(const std::uint32_t& local_port, const uint32_t& thread_nums)
      : pImpl("0.0.0.0", local_port, thread_nums) {}

  void StartReceiving() {
    logger.LogInfo() << "udp socket start listening on:"
                     << local_endpoint_.address().to_string() << "port"
                     << local_endpoint_.port();
    do_receive();
  }

  void do_receive() {
    socket_.async_receive_from(
        boost::asio::buffer(static_cast<void*>(data_.Data()), data_.Size()),
        remote_endpoint_,
        [this](boost::system::error_code ec, std::size_t length) {
          logger.LogVerbose() << "udp receive data. size:" << length;
          if (!ec) {
            auto data = make_buffer(length);
            data.CopyFrom(data_.Data());
            func_(std::move(data), remote_endpoint_.address().to_string(),
                  remote_endpoint_.port());
            do_receive();
          } else {
            logger.LogWarn() << ec.message();
          }
        });
  }

  ~pImpl() {
    work_.reset();
    Close();
  }

  void SetReceiveCallback(
      std::function<void(SocketBufferType&& data, const std::string& ip,
                         const uint32_t& port)>
          func) {
    func_ = func;
  }

  void JoinGroup(const std::string& multicast_ip,
                 const std::string& local_interface = "") {
    // Join the multicast group.
    local_interface == ""
        ? socket_.set_option(boost::asio::ip::multicast::join_group(
              boost::asio::ip::make_address_v4(multicast_ip)))
        : socket_.set_option(boost::asio::ip::multicast::join_group(
              boost::asio::ip::make_address_v4(multicast_ip),
              boost::asio::ip::make_address_v4(local_interface)));
  }

  std::string GetBroadcastAddr() {
    return GetInterfaceFromName(GetNameFromIp(local_ip_)).broadcast_address;
  }

  void SendTo(std::unique_ptr<promise_type<std::size_t>>&& p,
              const std::string& ip, const std::uint32_t& port,
              const void* data, std::size_t size) {
    try {
      /* code */
      socket_.async_send_to(
          boost::asio::const_buffer(data, size),
          boost::asio::ip::udp::endpoint(boost::asio::ip::make_address(ip),
                                         port),
          [p1 = std::move(p), this](boost::system::error_code ec,
                                    std::size_t length) {
            if (!ec) {
              logger.LogVerbose() << "udp send data. size:" << length;
              p1->set_value(length);
            } else {
              p1->SetError(static_cast<ara::core::CoreErrc>(ec.value()));
              logger.LogWarn() << ec.message();
            }
          });
    } catch (const std::exception& e) {
      logger.LogWarn() << e.what();
    }
  }

  std::string Mac() { return GetMacFromIp(local_ip_); }

  void Close() { socket_.close(); }
};

SocketUdp::SocketUdp(const std::string& local_address,
                     const uint32_t& local_port, const uint32_t& thread_nums)
    : pImpl_(std::make_unique<pImpl>(local_address, local_port, thread_nums)) {}

SocketUdp::SocketUdp(const uint32_t& local_port, const uint32_t& thread_nums)
    : pImpl_(std::make_unique<pImpl>(local_port, thread_nums)) {}

SocketUdp::SocketUdp(const std::string& local_address,
                     const uint32_t& thread_nums)
    : pImpl_(std::make_unique<pImpl>(local_address, thread_nums)) {}

SocketUdp::~SocketUdp() {}

std::string SocketUdp::Mac() { return pImpl_->Mac(); }

void SocketUdp::Close() { return pImpl_->Close(); }

void SocketUdp::StartReceiving() { pImpl_->StartReceiving(); }

void SocketUdp::SetReceiveCallback(
    std::function<void(SocketBufferType&& data, const std::string& ip,
                       const uint32_t& port)>
        func) {
  pImpl_->SetReceiveCallback(func);
}

void SocketUdp::JoinGroup(const std::string& multicast_ip,
                          const std::string& local_interface) {
  pImpl_->JoinGroup(multicast_ip, local_interface);
}

future_type<std::size_t> SocketUdp::SendTo(const std::string& ip,
                                           const std::uint32_t& port,
                                           SocketBufferType& data) {
  return SendTo(ip, port, data.Data(), data.Size());
}

future_type<std::size_t> SocketUdp::SendTo(const std::string& ip,
                                           const std::uint32_t& port,
                                           const void* data, std::size_t size) {
  auto p_send =
      std::make_unique<promise_type<std::size_t>>(promise_type<std::size_t>());
  auto f = p_send->get_future();
  pImpl_->SendTo(std::move(p_send), ip, port, data, size);
  return f;
}

std::string SocketUdp::GetBroadcastIP() { return pImpl_->GetBroadcastAddr(); }

}  // namespace udp
}  // namespace socket
}  // namespace ara
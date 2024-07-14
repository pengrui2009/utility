/*
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-11-21 18:13:41
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-10 15:27:14
 * @Description:
 *
 * Copyright (c) 2022 by Tusimple, All Rights Reserved.
 */

#include <ara/log/logger.h>
#include <ara/signal/SignalManager.h>
#include <gtest/gtest.h>
#include <signal.h>

#include <atomic>
#include <functional>
#include <thread>
using namespace ara::signal;

void send_signal(int signal_number) { raise(signal_number); }

void sigHandler_with_ec(const std::error_code& code, int signal_number,
                        std::atomic<bool>* stopped,
                        std::atomic<uint32_t>* count) {
  if (code.value() == 0) {
    switch (signal_number) {
      case SIGINT:
        *stopped = true;
        break;
      case SIGQUIT:
        *count = 1;
        break;
      case SIGSEGV:
        *count = 2;
        break;
      case SIGALRM:
        *count = 3;
        break;
      default:
        *stopped = true;
        break;
    }
  } else {
    std::cout << code.message() << std::endl;
    *stopped = true;
  }
}

void sigHandler_with_repeat(const std::error_code& code, int signal_number,
                            std::atomic<bool>* stopped,
                            std::atomic<uint32_t>* count) {
  if (code.value() == 0) {
    switch (signal_number) {
      case SIGINT:
        *stopped = true;
        break;
      case SIGQUIT:
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        *count = 1;
        break;
      case SIGSEGV:
        *count = 2;
        break;
      case SIGALRM:
        *count = 3;
        break;
      default:
        *stopped = true;
        break;
    }
  } else {
    std::cout << code.message() << std::endl;
    *count = 5;
    *stopped = true;
  }
}

void sigHandler_with_ec_2(const std::error_code& code, int signal_number,
                          std::atomic<bool>* stopped,
                          std::atomic<uint32_t>* count) {
  if (code.value() == 0) {
    switch (signal_number) {
      case SIGINT:
        *stopped = true;
        break;
      case SIGQUIT:
        *count = 1;
        break;
      case SIGSEGV:
        *count = 2;
        break;
      case SIGALRM:
        (*count)++;
        break;
      default:
        *stopped = true;
        break;
    }
  } else {
    std::cout << code.message() << std::endl;
    *stopped = true;
  }
}

void sigHandler_with_ec_3(const std::error_code& code, int signal_number,
                          int number, std::atomic<bool>* stopped,
                          std::atomic<uint32_t>* count) {
  if (code.value() == 0) {
    switch (signal_number) {
      case SIGINT:
        *stopped = true;
        break;
      case SIGQUIT:
        *count = 1;
        break;
      case SIGSEGV:
        *count = 2;
        break;
      case SIGALRM:
        if (number == 1) {
          (*count) += 1;
        } else {
          (*count) += 2;
        }
        break;
      default:
        *stopped = true;
        break;
    }
  } else {
    std::cout << code.message() << std::endl;
    *stopped = true;
  }
}

void sigHandler_remove(const std::error_code& code, int signal_number,
                       std::atomic<bool>* stopped,
                       std::atomic<uint32_t>* count) {
  if (code.value() == 0) {
    switch (signal_number) {
      case SIGINT:
        *stopped = true;
        break;
      case SIGALRM:
        (*count)++;
        break;
      default:
        *stopped = true;
        break;
    }
  } else {
    std::cout << code.message() << std::endl;
    *stopped = true;
  }
}

void sigHandler_no_ec(int signal_number, std::atomic<bool>* stopped,
                      std::atomic<uint32_t>* count) {
  switch (signal_number) {
    case SIGINT:
      *stopped = true;
      break;
    case SIGQUIT:
      *count = 1;
      break;
    case SIGSEGV:
      *count = 2;
      break;
    case SIGALRM:
      *count = 3;
      break;
    default:
      break;
  }
}

void sigHandler_2(int signal_number, std::atomic<bool>* stopped,
                  std::atomic<uint32_t>* count) {
  // std::cout << "receive signal:" << signal_number << " count:" << *count
  //           << std::endl;

  switch (signal_number) {
    case SIGINT:
      *stopped = true;
      break;
    case SIGQUIT:
      *count = 1;
      break;
    case SIGSEGV:
      *count = 2;
      break;
    case SIGALRM:
      *count = 3;
      break;
    default:
      break;
  }
}

TEST(SignalManager, MultiAdd_1) {
  try {
    /* code */
    SignalManager sig;
    std::atomic<bool> stopped{false};
    std::atomic<uint32_t> count{0};
    sig.AddSignal(SIGINT, SIGQUIT, SIGSEGV, SIGALRM);
    while (!stopped) {
      if (count == 0) {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        send_signal(SIGQUIT);
      } else if (count == 1) {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        send_signal(SIGSEGV);
      } else if (count == 2) {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        send_signal(SIGALRM);
      } else {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        send_signal(SIGINT);
      }
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

TEST(SignalManager, MultiAdd_2) {
  try {
    SignalManager sig;
    std::atomic<bool> stopped{false};
    std::atomic<uint32_t> count{0};
    sig.AddSignal(SIGINT, SIGQUIT, SIGSEGV, SIGALRM);
    while (!stopped) {
      if (count == 0) {
        sig.AsyncWait(std::bind(sigHandler_no_ec, std::placeholders::_1,
                                &stopped, &count));
        send_signal(SIGQUIT);
      } else if (count == 1) {
        sig.AsyncWait(std::bind(sigHandler_no_ec, std::placeholders::_1,
                                &stopped, &count));
        send_signal(SIGSEGV);
      } else if (count == 2) {
        sig.AsyncWait(std::bind(sigHandler_no_ec, std::placeholders::_1,
                                &stopped, &count));
        send_signal(SIGALRM);
      } else {
        sig.AsyncWait(std::bind(sigHandler_no_ec, std::placeholders::_1,
                                &stopped, &count));
        send_signal(SIGINT);
      }
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

TEST(SignalManager, MultiAdd_3) {
  try {
    SignalManager sig;
    std::atomic<bool> stopped{false};
    std::atomic<uint32_t> count{0};
    sig.AddSignal(SIGINT, SIGQUIT, SIGSEGV, SIGALRM);
    sig.AsyncWaitRepeatly(
        std::bind(sigHandler_no_ec, std::placeholders::_1, &stopped, &count));
    while (!stopped) {
      if (count == 0) {
        send_signal(SIGQUIT);
      } else if (count == 1) {
        send_signal(SIGSEGV);
      } else if (count == 2) {
        send_signal(SIGALRM);
      } else {
        send_signal(SIGINT);
      }
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
    sig.Clear();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

TEST(SignalManager, MultiAdd_4) {
  try {
    SignalManager sig;
    std::atomic<bool> stopped{false};
    std::atomic<uint32_t> count{0};
    sig.AddSignal(SIGINT, SIGQUIT, SIGSEGV, SIGALRM);
    sig.AsyncWaitRepeatly(
        std::bind(sigHandler_no_ec, std::placeholders::_1, &stopped, &count));
    while (!stopped) {
      if (count == 0) {
        send_signal(SIGQUIT);
      } else if (count == 1) {
        send_signal(SIGSEGV);
      } else if (count == 2) {
        send_signal(SIGALRM);
      } else {
        send_signal(SIGINT);
      }
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
    sig.Clear();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

TEST(SignalManager, AddIllegal) {
  try {
    SignalManager sig;
    std::atomic<bool> stopped{false};
    std::atomic<uint32_t> count{0};
    EXPECT_ANY_THROW(sig.AddSignal(SIGKILL));
    EXPECT_ANY_THROW(sig.AddSignal(SIGSTOP));
    EXPECT_ANY_THROW(sig.AddSignal(-1));
    EXPECT_ANY_THROW(sig.AddSignal(1024));
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

TEST(SignalManager, MultiThreads) {
  try {
    /* code */
    SignalManager sig(4);
    std::atomic<bool> stopped{false};
    std::atomic<uint32_t> count{0};
    sig.AddSignal(SIGINT, SIGQUIT, SIGSEGV, SIGALRM);
    while (!stopped) {
      if (count == 0) {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        send_signal(SIGQUIT);
      } else if (count == 1) {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        send_signal(SIGSEGV);
      } else if (count == 2) {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        send_signal(SIGALRM);
      } else {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        send_signal(SIGINT);
      }
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

TEST(SignalManager, Cancle) {
  try {
    /* code */
    SignalManager sig;
    std::atomic<bool> stopped{false};
    std::atomic<uint32_t> count{0};
    sig.AddSignal(SIGINT, SIGQUIT, SIGSEGV, SIGALRM);
    while (!stopped) {
      if (count == 0) {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        send_signal(SIGQUIT);
      } else if (count == 1) {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        send_signal(SIGSEGV);
      } else if (count == 2) {
        sig.AsyncWait(std::bind(sigHandler_with_ec, std::placeholders::_1,
                                std::placeholders::_2, &stopped, &count));
        // send_signal(SIGALRM);
        sig.Cancle();
      }
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

TEST(SignalManager, CancleRepeatly) {
  try {
    /* code */
    SignalManager sig;
    std::atomic<bool> stopped{false};
    std::atomic<uint32_t> count{0};
    sig.AddSignal(SIGINT, SIGQUIT, SIGSEGV, SIGALRM);
    sig.AsyncWaitRepeatly(std::bind(sigHandler_with_repeat,
                                    std::placeholders::_1,
                                    std::placeholders::_2, &stopped, &count));

    send_signal(SIGQUIT);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sig.Cancle();
    EXPECT_EQ(count, 5);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

TEST(SignalManager, MultiSignalManager) {
  try {
    /* code */
    SignalManager sig;
    SignalManager sig_2;
    std::atomic<bool> stopped{false};
    std::atomic<uint32_t> count{0};
    sig.AddSignal(SIGINT, SIGQUIT, SIGSEGV, SIGALRM);
    sig.AsyncWait(std::bind(sigHandler_with_ec_3, std::placeholders::_1,
                            std::placeholders::_2, 1, &stopped, &count));
    sig_2.AddSignal(SIGALRM);
    sig_2.AsyncWait(std::bind(sigHandler_with_ec_3, std::placeholders::_1,
                              std::placeholders::_2, 2, &stopped, &count));
    send_signal(SIGALRM);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(count, 3);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

TEST(SignalManager, Remove) {
  std::atomic<bool> stopped{false};
  std::atomic<uint32_t> count{0};
  SignalManager sig, sig2;
  sig.AddSignal(SIGINT, SIGALRM);
  sig2.AddSignal(SIGALRM);
  sig.AsyncWait(std::bind(sigHandler_remove, std::placeholders::_1,
                          std::placeholders::_2, &stopped, &count));
  sig2.AsyncWait(std::bind(sigHandler_remove, std::placeholders::_1,
                           std::placeholders::_2, &stopped, &count));
  send_signal(SIGALRM);
  // wait handlers end
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  sig.AsyncWait(std::bind(sigHandler_remove, std::placeholders::_1,
                          std::placeholders::_2, &stopped, &count));

  sig2.AsyncWait(std::bind(sigHandler_remove, std::placeholders::_1,
                           std::placeholders::_2, &stopped, &count));
  sig2.Remove(SIGALRM);
  send_signal(SIGALRM);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(count, 3);
}

// TEST(SignalManager, Remove) {
//   std::atomic<uint32_t> outer_count{0};
//   std::atomic<bool> stopped{false};
//   std::atomic<uint32_t> count{0};
//   try {
//     {
//       /* code */
//       SignalManager sig;
//       SignalManager sig_2;
//       sig.AddSignal(SIGINT, SIGQUIT, SIGSEGV, SIGALRM);
//       sig_2.AddSignal(SIGALRM);
//       sig_2.AsyncWait(std::bind(sigHandler_with_ec_2, std::placeholders::_1,
//                                 std::placeholders::_2, &stopped, &count));
//       while (!stopped) {
//         if (count == 0) {
//           sig.AsyncWait(std::bind(sigHandler_with_ec_2,
//           std::placeholders::_1,
//                                   std::placeholders::_2, &stopped, &count));
//           send_signal(SIGQUIT);
//           std::cout << "send SIGQUIT" << std::endl;

//         } else if (count == 1) {
//           sig.AsyncWait(std::bind(sigHandler_with_ec_2,
//           std::placeholders::_1,
//                                   std::placeholders::_2, &stopped, &count));
//           send_signal(SIGSEGV);
//           std::cout << "send SIGSEGV" << std::endl;
//           sig_2.Remove(SIGALRM);
//           std::cout << "sig 2 remove SIGALRM" << std::endl;
//         } else if (count == 2) {
//           if (outer_count == 0) {
//             outer_count++;
//             sig.AsyncWait(std::bind(sigHandler_with_ec_2,
//             std::placeholders::_1,
//                                     std::placeholders::_2, &stopped,
//                                     &count));
//             send_signal(SIGALRM);
//             std::cout << "send SIGALRM" << std::endl;
//           }

//         } else {
//           sig.AsyncWait(std::bind(sigHandler_with_ec_2,
//           std::placeholders::_1,
//                                   std::placeholders::_2, &stopped, &count));
//           send_signal(SIGINT);
//           std::cout << "send SIGINT" << std::endl;
//           // stopped = true;
//         }
//         std::this_thread::sleep_for(std::chrono::microseconds(10));
//         std::cout << "running count:" << count << std::endl;
//       }
//     }
//     EXPECT_EQ(count, 3 + outer_count - 1);
//   } catch (const std::exception& e) {
//     std::cerr << e.what() << '\n';
//   }
// }

int main(int argc, char** argv) {
  try {
    ara::log::InitLogging("sigl", "ara signal unit test",
                          ara::log::LogLevel::kInfo,
                          ara::log::LogMode::kConsole);
    ::testing::InitGoogleTest(&argc, argv);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  } catch (...) {
  }
  return RUN_ALL_TESTS();
}
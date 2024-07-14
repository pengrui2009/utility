# IPC Document

## **Overview**
### 1. **Introduction**
本IPC库是基于[Boost.Asio_1_71_0](https://www.boost.org/doc/libs/1_71_0/doc/html/boost_asio.html)开发的异步ipc library。当前需求仅满足ap-exec中的IPC 通信需求。
***
### 2. **Background and Requirements**
1. 每个控制器仅有一个em daemon进程，有多个应用进程;
2. em daemon作为服务端(server)，应用进程作为客户端(client);
3. 应用向em daemon发起请求，em根据收到的数据向客户端回复;
4. IPC应确保收发数据的完整性，确认server和client收到的数据是其对端刚好发送的数据；
5. 使用unix domain socket作为IPC通信方式；
6. 一个server端可同时接收多个client发出的请求；
7. server可指定监听的domain socket,client可指定连接的domain socket;
8. server端自动处理接收到的数据，根据处理结果向对应client发送响应数据；
9. server/client端可设置最大处理线程数；
10. client连接服务器的状态应当被正确反馈；
11. 正常情况下，client端的每次请求都将获取获得一个响应；
12. client应当知晓每次发送的请求，是否被server正确接收；
13. client应当获取与每次请求相对应的返回数据；
14. 同一个client在上一次请求未返回时，可再次发送新的请求；
15. client端能主动断开与服务器的连接；
***
## **Quick Start**
Samples参见[IPC Samples](../../../samples/ipc/).
### Server
```c++
#include <ara/ipc/ipc.h>
using namespace ara::ipc;
using MSG = ara::ipc::IPCMessage;

/* Define a ProcessHandler */
template <typename T=MSG>
class MyProcessor : public ara::ipc::ProcessHandler<T> {
 public:
  virtual T OnReadCallBack(T& msg) override {
    std::cout << " server callback:" << msg.timestamp() << std::endl;
    return msg;
  };
};

int main(int argc, int* argv[]) {
  try {
    /* Create a callback processor for server. */
    /* processor.OnReadCallBack will be called when receiving requests. */
    std::shared_ptr<MyProcessor<MSG>> processor(new MyProcessor<MSG>);
    ara::ipc::ServerDomainSocket<MSG> server(4, "/tmp/ipc.sock", processor);
    /* run the listening loop */
    server.Run();
    std::thread t([] {
      while (true) {
        std::cout << "running..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    });
    t.join();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
  return 1;
}

```
### Client
```c++
#include <ara/ipc/ipc.h>
using namespace ara::ipc;
using MSG = ara::ipc::IPCMessage;

int main(int argc, int* argv[]) {
  try {
    ara::ipc::ClientDomainSocket<MSG> client(1, "/tmp/ipc.sock");
    /* connect to the specific server */
    auto f = client.connect();
    if (f.get()) {
      std::cout << "connected" << std::endl;
    } else {
      std::cout << "connect over time" << std::endl;
      exit(1);
    }

    while (true) {
      std::cout << "Enter message: ";
      char request[max_length];
      std::cin.getline(request, max_length);
      /* create the transport message */
      MSG msg(std::strlen(request), request);
      std::cout  << " msg address: " << &msg << "msg payload len:" << msg.payload_length() << " ts:" << msg.timestamp() << std::endl;
      /* send msg to server */
      auto f = client.Send(msg);
      /* wait for response from server */
      auto r = f.get();
      std::cout  << "future get:" << r.timestamp() << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
  return 1;
}
```
***
## Turtial

### 1. **Architecture desgin**
<img src=./arch.jpg height="800" width="1000">

### 2. **IPCMessage**
`IPCMessage`是IPC底层默认的通信数据包。其包括两部分，分别为header和payload。

* message header:**至少**包括一个4字节的，表示payload size(字节数)的地址空间。

* payload:是一段连续的地址空间，长度等于message header中指定的payload size。

数据在IPCMessage的payload存储的字节序与编译平台指定的字节序一致，存储在栈区。

`IPCMessage`支持的接口如下：
```c++
/**
 * @brief 创建一个payload size为0的IPCMessage对象，时间戳会自动生成
 */
IPCMessage();
/**
 * @brief 拷贝构造，复制msg的data
 */
IPCMessage(const IPCMessage& msg);
/**
 * @brief 移动构造，复制msg的data，msg不再可用，但不释放内存
 */
IPCMessage(IPCMessage&& msg)；
/**
 * @brief 拷贝赋值，复制msg的data
 */
IPCMessage& operator=(const IPCMessage& msg)；
/**
 * @brief 移动赋值，复制msg的data，msg不再可用，但不释放内存
 */
IPCMessage& operator=(IPCMessage&& msg)；
```
***
### 3. **ClientDomainSocket**
每个client端创建一个连接socket用于和server通信。client的每次发送操作将会创建一个request。每个request执行的异步操作流程为**Write**(sending data to server)——>**Read**(receiving response from client)。若client端发送了多个request，client端将保证其接收的响应顺序也与发送顺序一致。**Write**为异步操作，**Read**操作为同步操作。

上述异步操作（Write——>Read），将被分配到线程池中任意的线程进行执行。若同时存在多个请求，多个请求的**Write**操作将被分配至同一个strand中，
即多个请求的**Write**操作不会同时发生，即使有多个线程执行**Write**操作。同理，多个请求的**Write**操作将被分配至同一个strand（与write的strand不同）中，即多个请求的**Read**操作不会同时发生，即使有多个线程执行**Read**操作。

client端支持手动断开重连。需要先调用client.disconnect()再调用client.connect()以完成重连。

```c++
/**
 * @brief client需要创建该类对象进行初始化。模型类型为传输使用的Message类型，如IPCMessage
 * 
 * @param n_threads 客户端使用的线程池数量
 * @param socket_file 要连接的socket_file
 */
template<typename T>
ClientDomainSocket<T>::ClientDomainSocket(uint32_t n_threads, const std::string socket_file)；

/**
 * @brief client向server发送消息
 *
 * @param msg 要发送的T类型的消息
 * @return future_type<T> 返回一个future<T>对象，用户可进一步通过future.get()获取返回的消息。
 */
template<typename T>
std::future<T> Send(T& msg);

/**
 * @brief 通过指定的socket_file连接至server
 * 
 * @return std::future<bool> 返回一个表明是否连接成功的future对象 
 */
std::future<bool> connect();
/**
 * @brief 停止当前所有的未完成的异步操作，移动所有未完成的request，停止io context，关闭client端socket
 *
 */
void disconnect();
```
***
### 4. **ServerDomainSocket**
server端创建一个监听socket(acceptor),并基于acceptor创建不同的通信connection。每个connection上的异步操作，将严格按照**Read**(receiving data from client)——>**Callback**——>**Write**(send data to client)——>**Read**的闭环顺序执行。
也就是说，同一个connection上收到的多个请求，将严格按照接收顺序向client返回消息。

上述异步操作（Read——>callback——>Write），将被分配到线程池中任意的线程进行执行。

```c++
/**
 * @brief 创建一个server对象实例，指定传输消息的类型，如IPCMessage
 * 
 * @param n_threads server端使用的线程池数量
 * @param socket_file 创建并监听的local socket file，路径需要有可读可写权限
 * @param handler server端的回调处理函数
 */
template<typename T>
ServerDomainSocket(uint32_t n_threads, const std::string socket_file, ProcessHandlerPtr<T> handler);

/**
 * @brief 在创建的线程池中开始接收和监听socket上的数据
 *
 */
void Run();
```
***
### 5. **ProcessHandler**
**对于Server端**，IPC提供一个通用的回调函数，用于在接收到数据时，主动调用该回调函数。用户可以手动继承IPC提供的ProcessHandler基类，自己实现其中的OnReadCallBack函数。该OnReadCallBack函数的入参为接收到的IPCMessage类型的数据，返回值为要发送给client端的IPCMessage类型的数据。

**该回调函数将在初始化server时，创建的线程池中被执行。处理---->发送为同一线程中顺序执行的过程。**
```c++
  /**
   * @brief server端在接收到数据时调用的接口
   * 
   * @param msg 接收到的数据
   * @return T 要发送给client的数据
   */
  virtual T OnReadCallBack(T& msg){};
```

***
## Issues
1. ~~暂不支持client同时发送多个请求；~~（[22022/08/27提交](https://ghe.tusimple.io/TuSimple/aeg-adaptive-autosar/tree/6fd1fc4c1db8fadf066918901576b9bf43ea6cea)已支持）
2. ~~暂不支持client主动断开连接；~~（[22022/08/28提交](https://ghe.tusimple.io/TuSimple/aeg-adaptive-autosar/tree/40cfdfefbd064280e07b21c91a0d021b117d0226)已支持）
3. 如果需要在server端注册多个回调函数，目前只能通过创建多个server对象的方式来实现；)已支持）
4. IPCMessage支持移动构造和移动赋值，但实现上执行的仍然是深拷贝，被移动对象的内存只有当其生命周期结束时才会释放；
5. 同一进程中连续多次创建server/client连接时，有概率导致core dump
6. ~~client多线程发送时，响应数据顺序可能错乱([AA-378](https://jira.tusen.ai/browse/AA-378))~~([已修复](https://ghe.tusimple.io/TuSimple/aeg-adaptive-autosar/commit/4607ab3f72183ce1b01aa324974a3ea3eb6f34dd))
***
## Future features
1. 支持client的Send操作设置超时时间；
2. IPCMessage支持内置类型；
3. server端支持为不同的message类型设置不同的回调；
4. 优化IPCMessage移动、拷贝效率；
## ChatServer
在Ubuntu20环境下基于muduo开发的集群聊天服务器。目前实现用户注册和登录接口。

## 运行环境
1. 安装[muduo库](https://github.com/chenshuo/muduo)

## 项目构建
```bash
git clone https://github.com/djdodsjsjx/ChatServer.git
cd ChatServer
mkdir build
cd build
cmake ..
make
```

## 项目运行
```bash
# 启动服务器
cd ../bin
./ChatServer 6000

# 启动客户端
telnet 127.0.0.1 6000

# 登录测试 {"msgId": 1000}
# 注册测试 {"msgId": 1001}
```

## 实现原理
### muduo网络库
muduo 是一个基于 C++ 的高性能网络库，主要用于构建网络服务器和客户端应用。它的设计理念是高效、简洁和易于使用。muduo 的核心线程模型是「one loop per thread + thread pool」，这种模型在处理网络事件时非常高效。

#### muduo 的基本线程模型
1. **One Loop per Thread**:
   - 每个线程都有一个独立的 `EventLoop`，负责处理该线程上的所有 I/O 事件和定时器事件。
   - `EventLoop` 使用 Reactor 模式，通过事件驱动的方式来处理事件，如读写事件、连接事件等。
2. **Thread Pool**:
   - 除了每个线程的 `EventLoop` 外，muduo 还使用一个线程池来处理计算密集型任务或需要异步执行的任务。
   - 线程池中的线程可以并行执行任务，提高整体的处理能力。

#### muduo 的主从 Reactor 模型
muduo 采用主从 Reactor 模型来进一步提高性能：
1. **mainReactor**:
   - `mainReactor` 负责监听新连接的到来。
   - 它使用 `Acceptor` 来接收新的连接请求，并将这些新连接分发到 `subReactor` 上。
2. **subReactor**:
   - `subReactor` 负责维护已建立的连接。
   - 每个 `subReactor` 通常运行在一个独立的线程上，处理连接的读写事件和其他相关事件。

#### 网络模块与业务模块解耦
使用哈希表对得到msgId调用不同的处理函数，实现网络模块和业务模块的解耦。
```cpp
void ChatServer::onMessage(const TcpConnectionPtr& conn, 
                            Buffer* buffer,
                            Timestamp time) {
    // 将json数据转换为string
    string buf = buffer->retrieveAllAsString();
    // 数据的反序列化
    json js = json::parse(buf);
    auto msgHandler = ChatService::instance()->getHandler(js["msgId"].get<int>());

    // 执行对应的业务处理
    msgHandler(conn, js, time);
}
```
## 业务模块
### 注册模块
从网络模块接受数据，根据msgId定位到注册模块。
### 登录模块
从网络模块接受数据，更具msgId定位到登录模块。
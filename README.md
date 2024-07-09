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

# 注册测试 {"msgId":1000,"name":"123","password":"123456"}
# 登录测试 {"msgId":1002,"id":22,"password":"123456"}
# 1v1对话测试 {"msgId":1004,"fromid":22,"toid":23,"msg":"hello"}
# 好友添加测试 {"msgId":1005,"id":22,"friendId":23,"msg":"add friend"}
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
从网络模块接受数据，根据msgId定位到注册模块。根据客户端传输过来的json对象，获取用户名字和密码生成User对象，在model层中插入到数据库中。
### 登录模块
从网络模块接受数据，更具msgId定位到登录模块。根据客户端传输过来的json对象，获取用户ID和密码，如果用户没有登录则登录该用户，并保存对应通信连接用于后续服务器接受到其他客户端消息后进行转发。
### 服务器异常退出模块
如果服务器异常退出，将所有的在线客户端状态设置为offline
### 客户端异常退出模块
如果客户端异常退出，将该退出的用户状态设置为offline，并删除对应的通信连接。

### 1对1聊天模块
客户端给对应的用户Id发送消息，如果该用户处于登录状态直接转发，否则存储在离线消息中，直到该用户再次登录时，从离线消息中获取并清除对应的离线消息。

### 好友添加模块
用户可以根据指定id号添加好友，并插入到数据库中，当该用户再次登录时可以显示好友列表，后续进行聊天。

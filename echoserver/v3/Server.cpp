/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-09 09:19
#
# Filename: Server.cpp
#
# Description: 
#
=============================================================================*/

#include "EventLoop.h"
#include "Acceptor.h"
#include "Server.h"
#include "Connection.h"
#include <functional>
#include <cstring>
#include <unistd.h>
#include <cerrno>

using namespace SocketUtil;

using Callback = std::function<void()>;

Server::Server(EventLoop* loop)
    : loop(loop)
{
    ac = new Acceptor(loop);
    ac->connCb = std::bind(&Server::acceptor, this, std::placeholders::_1);
}

Server::~Server() {
   delete ac;
}

void Server::acceptor(int cli_fd) {
    Connection *conn = new Connection(loop, cli_fd);
    conn->connCb = std::bind(&Server::disconnector, this, std::placeholders::_1);
    connections[cli_fd] = conn;
}

void Server::disconnector(int fd) {
    Connection *conn = connections[fd];
    connections.erase(fd);
    delete conn;
}



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
    : mainLoop(loop)
{
    ac = new Acceptor(loop);
    ac->connCb = std::bind(&Server::acceptor, this, std::placeholders::_1);
	
	int size = std::thread::hardware_concurrency();
	thP = new ThreadPool<EventLoop>(size);
	std::vector<EventLoop> els = thP->getObjects();
	for (int i = 0; i < size; ++i) {
		subReactors.push_back(&els[i]);
	}

	for (int i = 0; i < size; ++i) {
		thP->submit(std::bind(&EventLoop::loop, subReactors[i]));
	}
}

Server::~Server() {
   delete ac;
   for ( auto it = connections.begin(); it != connections.end(); ) {
		if ( it->second )
		{
			delete it->second;
		}
		/** @bug 直接erase it 会导致it为空，遍历出错*/
	    //connections.erase(it);
		connections.erase(it++);
   }
}

void Server::acceptor(int cli_fd) {
	int cur = cli_fd % subReactors.size();
    Connection *conn = new Connection(subReactors[cur], cli_fd);
    conn->delConnCb = std::bind(&Server::disconnector, this, std::placeholders::_1);
    connections[cli_fd] = conn;
}

void Server::disconnector(int fd) {
    Connection *conn = connections[fd];
    connections.erase(fd);
    delete conn;
}



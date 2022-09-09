/*=============================================================================
#
# Author: vilewind - luochengx2018@163.com
#
# Last modified: 2022-09-05 11:51
#
# Filename: Util.h
#
# Description:基础工具类
#
=============================================================================*/
#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace Util
{
	/**
	 * @brief 实现不可复制类，但可移动
	*/
	class noncopyable
	{
	public:
		noncopyable() = default;
		~noncopyable() = default;
		
		noncopyable(const noncopyable&) = delete;
		noncopyable& operator=(const noncopyable) = delete;
	};

	/**
	 * @brief 错误提示
	 * @param err 错误信息
	 * @param flag 判断是否错误的标志
	 * @param func 执行的函数，如socket等
	 * @param args 执行函数的必要参数
	 * 
	 * @returns 执行函数的返回值
	*/
	template<typename Func, typename ...Args>
	auto ERRIF(const char* err, int flag, Func&& func, Args&& ...args)->decltype(func(args...)) {
		auto res = func(args...);
		if (res < flag) {
			std::cerr << err;
			exit(EXIT_FAILURE);
		} 
		return res;
	}
}

namespace Socket
{
	struct Addr
	{
		struct sockaddr_in addr;
		socklen_t len { sizeof(sockaddr_in)};
		Addr() = default;
		Addr(const char* ip, uint16_t port) {
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			inet_pton(AF_INET, ip, &addr.sin_addr);
		}
		~Addr() = default;
	};

	class Socket : Util::noncopyable
	{
	public:
		explicit Socket(int fd) : m_fd(fd) {}
		~Socket() { Util::ERRIF(__func__, 0, ::close, m_fd); }

		void bind(const Addr& addr) {
			Util::ERRIF(__func__, 0, ::bind, m_fd, (struct sockaddr*)&addr.addr, addr.len);
		}

		void listen() {
			Util::ERRIF(__func__, 0, ::listen, m_fd, 1024);
		}

		int accept(Addr* addr) {
			return Util::ERRIF(__func__, 0, ::accept, m_fd, (struct sockaddr*)&addr->addr, &addr->len);
		}
		/**
		 * @brief 支持半关闭，服务器半关闭写
		*/
		void shutdownWrite(bool flag = true) {
			if (flag) {
				Util::ERRIF(__func__, 0, ::shutdown, m_fd, SHUT_WR);
			}
		}
		/**
		 * @brief 支持地址重用
		*/
		void setReuseAddr(bool flag = true) {
			if (flag) {
				int op = 1;
				Util::ERRIF(__func__, 0, ::setsockopt, m_fd, SOL_SOCKET, SO_REUSEADDR, &op, static_cast<socklen_t>(sizeof op));
			}
		}
		/**
		 * @brief 支持端口重用
		*/
		void setReusePort(bool flag = true) {
			if (flag) {
				int op = 1;
				Util::ERRIF(__func__, 0, ::setsockopt, m_fd, SOL_SOCKET, SO_REUSEPORT, &op, static_cast<socklen_t>(sizeof op));
			}
		}
		/**
		 * @brief 支持keep-alive
		*/
		void keepAlive(bool flag = true) {
			if (flag) {
				int op = 1;
				Util::ERRIF(__func__, 0, ::setsockopt, m_fd, SOL_SOCKET, SO_KEEPALIVE, &op, static_cast<socklen_t>(sizeof op));
			}
		}

		int getFd() const { return m_fd; }
	private:	
		const int m_fd;
	};

	namespace SocketUtil{
		int createNoblockSocket();

		void halfClose(int fd, int op);
	}
}

#endif


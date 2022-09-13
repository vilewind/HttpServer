/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-12 10:11
#
# Filename: httpConn.cpp
#
# Description: 
#
=============================================================================*/
#include "httpConn.h"

const char* ok_200_title="OK";
const char* error_400_title="Bad Request";
const char* error_400_form="Your request had bas syntax or is inherently impossible to safisfy.\n";
const char* error_403_title="Forbidden";
const char* error_403_form="You don not have permission to get file from this server.\n";
const char* error_404_title="Not Found";
const char* error_404_form="The requested file was not found on this server.\n";
const char* error_500_title="Internal Error";
const char* error_500_form="There was an unusual problem serving the requested filg.\n";

/* 网站根目录*/
const char* doc_root="/var/www/html";

int setnonblock(int fd) {
	int flag = fcntl(fd, F_GETFL);
	int new_flag = flag | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_flag);

	return flag;
}

void addfd(int epollfd, int fd, bool one_shot) {
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
	if (one_shot) {
		event.events |= EPOLLONESHOT;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnonblock(fd);
}

void removefd(int epollfd, int fd) {
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
	close(fd);
}

void modefd(int epollfd, int fd, int ev) {
	epoll_event event;
	event.data.fd = fd;
	event.events = ev | EPOLLIN | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

int HttpConn::m_user_count=0;
int HttpConn::m_epollfd=-1;

void HttpConn::close_conn(bool real_close) {
	if (real_close && (m_sockfd != -1)) {
		removefd(m_epollfd, m_sockfd);
		m_sockfd = -1;
		--m_user_count;
	}
}

void HttpConn::init(int fd, const sockaddr_in& addr) {
	m_sockfd = fd;
	m_addr = addr;
	int error = 0;
	socklen_t len = sizeof error;
	getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
	int reuse = 1;
	setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);

	addfd(m_epollfd, m_sockfd, true);
	++m_user_count;

	init();
}

void HttpConn::init() {
	m_check_state = CHECK_STATE_REQUESTLINE;
	m_linger = false;

	m_method = GET;
	m_url = nullptr;
	m_version = nullptr;
	m_content_length = 0;
	m_host = nullptr;
	m_start_line = 0;
	m_checked_idx = 0;
	m_read_idx = 0;
	m_write_idx = 0;
	memset(m_read_buf, '\0', READ_BUFFER_SIZE);
	memset(m_write_buf, '\0', WRITE_BUFFER_SIZE);
	memset(m_real_file, '\0', FILENAME_LEN);
}

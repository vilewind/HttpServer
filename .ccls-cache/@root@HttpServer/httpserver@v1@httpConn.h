/*=============================================================================
#
# Author: vilewind - luochengx2019@163.com
#
# Last modified: 2022-09-12 05:38
#
# Filename: httpConn.h
#
# Description: 
#
=============================================================================*/
#ifndef __HTTPCONN_H__
#define __HTTPCONN_H__

#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <cassert>
#include <cstring>
#include <sys/stat.h>
#include <thread>
#include <sys/mman.h>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <cstdarg>

class HttpConn
{
public:
	static const int FILENAME_LEN = 200;
	static const int READ_BUFFER_SIZE = 2048;
	static const int WRITE_BUFFER_SIZE = 1024;

	enum METHOD { GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PTACH};

/* 主状态机可能的状态*/
	enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };

/* HTTP请求结果状态*/
	enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };

/* 行分析状态*/
	enum LINE_STATUS { LINE_OK=0, LINE_BAD, LINE_OPEN };

public:
	HttpConn();
	~HttpConn();

public:
/* tcp连接*/
	void init(int, const sockaddr_in&);
	void close_conn(bool real_close=true);
	void process();
	bool read();
	bool write();
private:
	void init();
	HTTP_CODE process_read();
	bool process_write(HTTP_CODE ret);

	HTTP_CODE parse_request_line(char* text);
	HTTP_CODE parse_headers(char* text);
	HTTP_CODE parse_content(char* text);
	HTTP_CODE do_request();

	char* get_line() { return m_read_buf + m_start_line; }
	LINE_STATUS parse_line();

/* process_write调用函数集合，用于填充回复http应答*/	
	void unmap();
	bool add_response(const char* format, ...);
	bool add_content(const char* content);
	bool add_status_line(int status, const char* title);
	bool add_headers(int content_length);
	bool add_content_length(int content_length);
	bool add_linger();
	bool add_blank_line();

public:
	static int m_epollfd;
	static int m_user_count;
private:
	int m_sockfd;
	sockaddr_in m_addr;
/* 读缓冲区*/
	char m_read_buf[READ_BUFFER_SIZE];
	int m_read_idx;							//已读入客户数据的最后一个字节的下一个位置
	int m_checked_idx;						//正在分析的字符在缓冲区的位置
	int m_start_line;						//当前正在分析行的起始位置
/* 写缓冲区*/	
	char m_write_buf[WRITE_BUFFER_SIZE];
	int m_write_idx;						//写缓冲区中待发送的字节数

	CHECK_STATE m_check_state;
	METHOD m_method;

	char m_real_file[FILENAME_LEN];			//客户请求目标文件的完整路径=/root/+m_url
	char* m_url;							//客户请求文件名
	char* m_version;						//HTTP协议版本号
	char* m_host;							//主机名
	int m_content_length;					//HTTP请求的消息体长度
	bool m_linger;							//是否要求保持连接keep-alive

	char* m_file_address;					//客户请求的目标文件被mmap到内存中的起始位置
	struct stat m_file_stat;				//目标文件的状态，可通过其判断文件是否存在或读写权限等信息

/* 使用writev执行写操作*/	
	struct iovec m_iv[2];					
	int m_iv_count;

};

#endif

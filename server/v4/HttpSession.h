#ifndef __HTTPSESSION_H__
#define __HTTPSESSION_H__

#include <memory>

class TcpConnection;

struct HttpRequest
{
    enum METHOD { GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH };
    enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
    enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };
    enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };

    
};

struct HttpResponse
{

};

class HttpSession
{
public:
    using TcpConnSPtr = std::shared_ptr<TcpConnection>;

private:
    TcpConnSPtr m_tcpConn;
    HttpRequest m_request;
    HttpResponse m_response;
};

#endif


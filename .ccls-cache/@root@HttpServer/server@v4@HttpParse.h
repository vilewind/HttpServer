#ifndef __HTTPPARSE_H__
#define __HTTPPARSE_H__

#include <string>

class HttpParse
{
public:
    enum METHOD { GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH };
    enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
    enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };
    enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN, BUF_EMPTY };

    HttpParse() = default;
    ~HttpParse() = default;

    void append(std::string str) { m_buf += str; }
    
    HTTP_CODE parse_process();
private:

    HTTP_CODE parse_request_line();
    HTTP_CODE parse_headers();
    HTTP_CODE parse_content();

    LINE_STATUS parse_line();

    std::string m_buf;
    int m_lineStartIdx{0};                                            //按行解析，该行的起始位置
    int m_lineEndIdx{0};                                              //按行解析，改行的结束位置

    CHECK_STATE m_checkState{CHECK_STATE_REQUESTLINE};
    METHOD m_method{GET};
    std::string m_realFile{};
    std::string m_url{};
    std::string m_version{};
    std::string m_host{};
    int m_contentLen{0};
    bool m_linger{false};
};

#endif
#include "HttpParse.h"
#include <iostream>

const static std::string CRLF{"\r\n"};
const static std::string CR{"\r"};
const static std::string LF{"\n"};
const static std::string SP{"\t"};

HttpParse::LINE_STATUS HttpParse::parse_line()
{
    if (m_buf.empty())  return BUF_EMPTY;
    int idx = m_buf.find(CRLF, m_lineStartIdx);
    if (idx != m_buf.npos) 
    {
        m_lineEndIdx = idx;
        return LINE_OK;
    } 
    else 
    {
        /* 若“\r”在buffer的末尾，说明未读完，那么line不完整，继续读*/
        if ((idx = m_buf.find(CR, m_lineStartIdx)) == m_buf.npos - 1)
        {
            return LINE_OPEN;
        } 
        /* 未发现“\r”，有两种情况：1、未发现"\n"，那么继续读；2、发现“\n"，出错*/
        else if (idx == m_buf.npos) 
        {
            if (m_buf.find(LF, m_lineStartIdx) != m_buf.npos)
            {
                return LINE_BAD;
            }
            else 
            {
                return LINE_OPEN;
            }
        }
    }
}

/**
 * @brief 请求行：HTTP方法+"\t"+URL+"\t"+HTTP/1.1
*/
HttpParse::HTTP_CODE HttpParse::parse_request_line()
{
    int idx = m_buf.find(SP, m_lineStartIdx);
/* 请求行内没有空格，格式错误*/
    if (idx == m_buf.npos || idx >= m_lineEndIdx )
    {
        return BAD_REQUEST;
    }
/*仅支持GET请求*/
    std::string method = m_buf.substr(m_lineStartIdx, idx - m_lineStartIdx + 1);
    if (method == "GET")
    {
        m_method = GET;
    }
    else 
    {
        return BAD_REQUEST;
    }

    m_lineStartIdx = idx + 1;
    idx = m_buf.find(SP, m_lineStartIdx);
    if (idx == m_buf.npos || idx >= m_lineEndIdx )
    {
        return BAD_REQUEST;
    }
    m_url = m_buf.substr(m_lineStartIdx, idx - m_lineStartIdx + 1);
    m_version = m_buf.substr(idx+1, m_lineEndIdx - idx + 1);
/* 仅支持HTTP/1.1*/
    if (m_url.substr(0, 7) != "http://" || m_version != "HTTP/1.1")
    {
        return BAD_REQUEST;
    }
/* 更新状态*/
    m_checkState = CHECK_STATE_HEADER;
/* 更新下一行的起始点*/
    m_lineStartIdx = m_lineEndIdx + 2;
    m_lineEndIdx = m_lineStartIdx;
    return NO_REQUEST;
}

/**
 * @brief HTTP请求头= { key : value \r\n }
*/
HttpParse::HTTP_CODE HttpParse::parse_headers()
{
    int idx = m_lineStartIdx;
/* 空行，响应头的结束标志*/    
    if (m_buf[m_lineStartIdx] == '\0')
    {
        if ( m_method == HEAD)                                      //head方法与get类似，但服务器不会返回消息体，通常用来测试超链接的有效性、可用性和最近是否修改
        {
            return GET_REQUEST;
        }

        if (m_contentLen != 0)
        {
            m_checkState = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }

        return GET_REQUEST;
    }
    else if ((idx =  m_buf.find("Connection:", idx)) != m_buf.npos && idx < m_lineEndIdx)
    {
        if ((idx = m_buf.find("keep-alive", idx + 11)) != m_buf.npos && idx < m_lineEndIdx)
        {
            m_linger = true;
        }
    } 
    else if ((idx = m_buf.find("Content-Length:", m_lineStartIdx) != m_buf.npos) && idx < m_lineEndIdx)
    {
        if ((idx = m_buf.find(SP, idx+15)) != m_buf.npos && idx < m_lineEndIdx)
        {
            m_contentLen = stoi(m_buf.substr(idx + 1, m_lineEndIdx - idx - 1));
        }
    }
    else if ((idx = m_buf.find("Host:", m_lineStartIdx)) != m_buf.npos && idx < m_lineEndIdx)
    {
        if ((idx = m_buf.find(SP, idx+5)) != m_buf.npos && idx < m_lineEndIdx)
        {
            m_host = m_buf.substr(idx+1, m_lineEndIdx - idx - 1);
        }
    } 
    else 
    {
        std::cout << "unsupported header" << m_buf.substr(m_lineStartIdx, m_lineEndIdx-m_lineStartIdx + 1);
    }

    m_lineStartIdx = m_lineEndIdx + 2;
    m_lineEndIdx = m_lineStartIdx;

    return NO_REQUEST;
}

/**
 * @brief 请求体的长度为content-length
*/
HttpParse::HTTP_CODE HttpParse::parse_content()
{
    if (m_buf.size() - m_lineStartIdx >= m_contentLen)
    {
        m_buf.back() = '\0';
        return GET_REQUEST;
    }
    return NO_REQUEST;
}

HttpParse::HTTP_CODE HttpParse::process_process()
{
    LINE_STATUS line_status = LINE_OK;
}
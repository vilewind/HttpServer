#include "HttpResponse.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

HttpResponse::~HttpResponse()
{
    if ( !m_file_address.empty() )
    {
        munmap( const_cast<char*>(m_file_address.c_str()), m_file_stat.st_size);
        m_file_address.clear();
    }
}

void HttpResponse::addStatusLine(int status, const std::string& title)
{
    m_buf += " " + std::to_string(status) + " " + title + "\r\n";
}

void HttpResponse::addHeaders(int content_len, bool is_KeepAlive)
{
    addContentLen(content_len);
    addLinger(is_KeepAlive);
    addBlankLine();
}

void HttpResponse::addContentLen(int content_len)
{
    m_buf += "Content-Length: " + std::to_string(content_len) + "\r\n";
}

void HttpResponse::addLinger(bool is_KeepAlive)
{
    std::string str = is_KeepAlive ? "keep-alive" : "close";
    m_buf += "Connection: " + str + "\r\n";
}

void HttpResponse::addBlankLine()
{
    m_buf += "\r\n";
}

void HttpResponse::addContent(const std::string& content)
{
    // assert(content.size() == content_len);
    m_buf += content;
}

HttpResponse::HTTP_CODE HttpResponse::doRequest()
{
    // m_realFile = doc_root + m_url;
    // assert(m_realFile.size() > MAX_FILE_LEN);
///@brief stat(file_name, stat*buf),通过filename获取文件信息，并保存在stat结构体中    
    if ( stat( const_cast<char*>(m_realFile.c_str()), &m_file_stat) < 0)
    {
        return NO_RESOURCE;
    }
/// @param st_mode 文件存取权限，S_IROTH 所有用户均可读取 
    if (! (m_file_stat.st_mode & S_IROTH))                          
    {
        return FORBIDDEN_REQUEST;
    }
/// @a S_ISDIR(st_mode) 判断是否为目录
    if ( S_ISDIR( m_file_stat.st_mode ) )
    {
        return BAD_REQUEST;
    }
    
    return FILE_REQUEST;
}

void HttpResponse::requestProcess(HTTP_CODE ret)
{
    switch (ret)
    {
    case INTERNAL_ERROR :
    {
        addStatusLine(500, error_500_title);
        addHeaders(error_500_form.size());
        addContent(error_500_form);
        break;
    }
    case BAD_REQUEST :
    {
        addStatusLine(400, error_400_title);
        addHeaders(error_400_form.size());
        addContent(error_400_form);
        break;
    }
    case FORBIDDEN_REQUEST :
    {
        addStatusLine(403, error_403_title);
        addHeaders(error_403_form.size());
        addContent(error_403_form);
        break;
    }
    case FILE_REQUEST :
    {
        addStatusLine(200, ok_200_title);

        if ( m_file_stat.st_size != 0)
        {
            std::ifstream file;
            file.open(m_realFile, std::ios_base::in);
            if (!file.is_open())
            {
                /* */
            }
            std::stringstream ss;
            while(ss << file.rdbuf())
            {
                m_buf += ss.str();
            }
        } 
        else 
        {
            std::string ok_str = "<html><body></body></html>";
            addHeaders(ok_str.size());
            addContent(ok_str);
        }
        break;
    }
    default:
        break;
    }
}
#ifndef __HTTPRESPONSE_H__
#define __HTTPRESPONSE_H__

#include <string>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/uio.h>

static const std::string ok_200_title = "OK";
static const std::string error_400_title = "Bad Request";
static const std::string error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
static const std::string error_403_title = "Forbidden";
static const std::string error_403_form = "You do not have permission to get file from this server.\n";
static const std::string error_404_title = "Not Found";
static const std::string error_404_form = "The requested file was not found on this server.\n";
static const std::string error_500_title = "Internal Error";
static const std::string error_500_form = "There was an unusual problem serving the requested file.\n";
static const std::string doc_root = "/var/www/html";

class HttpResponse
{
public:
    enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };

    HttpResponse() = default;
    ~HttpResponse();

    HTTP_CODE doRequest();                                          //本地

    void requestProcess(HTTP_CODE);
private:
    void unmap();
    void addStatusLine( int status, const std::string& title );
    void addHeaders(int content_len, bool is_KeepAlive = false);
    void addContentLen(int content_len);
    void addLinger(bool is_KeepAlive = false);
    void addBlankLine();
    void addContent(const std::string& content);

    std::string m_buf{};
    std::string m_realFile;
    std::string m_file_address;
    struct stat m_file_stat;
};

#endif
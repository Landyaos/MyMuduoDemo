#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <map>
#include <string>

class HttpResponse
{
public:
    enum StatusCode
    {
        k100 = 100,
        k200 = 200,
        k301 = 301,
        k302 = 302,
        k400 = 400,
        k404 = 404,
        k500 = 500
    };

    HttpResponse() = default;
    ~HttpResponse() = default;

    void set(std::string str) { message_ = str; }

private:
    std::string version_;
    StatusCode status_code_;
    std::map<std::string, std::string> headers_;
    std::string file_path_;

    std::string message_;
};

#endif
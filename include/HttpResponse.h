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

    void set_version(std::string version) { version_ = version; }
    void set_status(std::string status) { status_ = status; }
    void set_header(std::string key, std::string value) { headers_[key] = value; }
    void set_data(std::string data) { data_ = data; }
    void Reset();

    std::string BuildHttpResponse()
    {
        std::string response;

        // 拼接状态行，包括HTTP版本和状态码
        response = version_ + " " + status_ +" OK\r\n";

        // 拼接响应头字段
        for (const auto &header : headers_)
        {
            response += header.first + ": " + header.second + "\r\n";
        }

        // 添加空行表示响应头结束
        response += "\r\n";

        // 添加响应体数据
        response += data_;

        return response;
    }

private:
    std::string version_;
    std::string status_;
    std::map<std::string, std::string> headers_;
    std::string data_;
};

#endif
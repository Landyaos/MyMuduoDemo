#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <iostream>
#include <map>
#include <string>

class HttpRequest
{
public:
    enum ParseRequestState
    {
        kMethod = 0,
        kPath,
        kVersion,
        kHeaderKey,
        kHeaderValue,
        kBody,
        kInvalid
    };

    HttpRequest();
    ~HttpRequest();

    ParseRequestState ParseRequest(std::string request);
    ParseRequestState ParseRequest1(std::string request);
    

    void toString()
    {
        std::cout << "Method:" << method_ << std::endl;
        std::cout << "Path:" << path_ << std::endl;
        std::cout << "HTTP Version:" << version_ << std::endl;
        std::cout << "Headers:\n";
        for (auto header : headers_)
        {
            std::cout << header.first << ":" << header.second << std::endl;
        }
        std::cout << "Request Body:\n"
                  << body_ << std::endl;
    }

private:
    std::string method_;
    std::string path_;
    std::string version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};
#endif

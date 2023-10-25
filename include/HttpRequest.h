#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <string>

#include "Buffer.h"

class HttpRequest
{
public:
    // 主状态机状态
    // CHECK_STATE_REQUESTLINE 解析请求行；CHECK_STATE_HEADER 解析头部字段
    enum PARSE_STATE
    {
        PARSE_REQUESTLINE = 0,
        PARSE_HEADER,
        PARSE_BODY,
        PARSE_COMPLETE
    };
    // 从状态机状态，LINE_OK 完整的一行；LINE_OPEN该行尚未读完；LINE_BAD 该行有错误
    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };
    // 处理http请求的结果 NO_REQUEST 表示读取的请求结果不完整；GET_REQUEST 读取了完整正确的http请求；BAD_REQUEST表示客户请求有错
    enum HTTP_CODE
    {
        NO_REQUEST,
        GOOD_REQUEST,
        BAD_REQUEST,
        FORBIDDEN_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };

    // 次状态机，每次读取一行，通过读取到/r/n来判断是不是完整的一行
    LINE_STATUS ParseLine(Buffer *buffer)
    {
        line_buf_ = buffer->ReadByLine();
        
        if (line_buf_ != "\r\n")
            return LINE_OK;
        else
            return LINE_OPEN;
    }

    // 解析报文主函数
    HTTP_CODE ParseRequest(Buffer *buffer)
    {
        LINE_STATUS linestatus = LINE_OK;
        HTTP_CODE retcode = NO_REQUEST;
        while ((linestatus = ParseLine(buffer)) == LINE_OK)
        {
            switch (parse_state_)
            {
            case PARSE_REQUESTLINE:
            {
                ParseRequestLine(line_buf_, parse_state_);
                break;
            }
            case PARSE_HEADER:
            {
                ParseHeaders(line_buf_, parse_state_);
                break;
            }
            case PARSE_BODY:
            {
                break;
            }
            case PARSE_COMPLETE:
            {
                return GOOD_REQUEST;
                break;
            }
            default:
            {
                return INTERNAL_ERROR;
            }
            }
        }
        if (linestatus == LINE_OPEN)
        {
            return NO_REQUEST;
        }
        else
        {
            return BAD_REQUEST;
        }
    }

    // 解析请求行 : GET url HTTP/1.1
    void ParseRequestLine(std::string &line, PARSE_STATE &checkstate)
    {
        std::istringstream iss(line);
        iss >> method_ >> path_ >> version_;
        checkstate = PARSE_HEADER;
    }
    // 解析头部字段，读取到空行说明头部正确
    void ParseHeaders(std::string &line, PARSE_STATE &checkstate)
    {
        if (line.size() == 0)
        {
            if (method_ == "GET")
                checkstate = PARSE_COMPLETE;
            else if (method_ == "POST")
                checkstate = PARSE_BODY;
        }
        else
        {
            int pos = static_cast<int>(line.find(':'));
            headers_[line.substr(0, pos)] = line.substr(pos + 2);
        }
    }
    void ParseBody(std::string &line, PARSE_STATE &checkstate)
    {
        checkstate = PARSE_COMPLETE;
    }

    void Reset()
    {
        parse_state_ = PARSE_REQUESTLINE;
        headers_.clear();
    }

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

    bool ParseState()
    {
        if (parse_state_ == PARSE_COMPLETE)
            return true;
        else
            return false;
    }

private:
    std::string line_buf_;
    PARSE_STATE parse_state_;

    std::string method_;
    std::string path_;
    std::string version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};

// class HttpRequest
// {
// public:
//     enum ParseRequestState
//     {
//         kMethod = 0,
//         kPath,
//         kVersion,
//         kHeaderKey,
//         kHeaderValue,
//         kBody,
//         kInvalid
//     };

//     HttpRequest();
//     ~HttpRequest();

//     ParseRequestState ParseRequest(std::string request);
//     ParseRequestState ParseRequest1(std::string request);

//     void toString()
//     {
//         std::cout << "Method:" << method_ << std::endl;
//         std::cout << "Path:" << path_ << std::endl;
//         std::cout << "HTTP Version:" << version_ << std::endl;
//         std::cout << "Headers:\n";
//         for (auto header : headers_)
//         {
//             std::cout << header.first << ":" << header.second << std::endl;
//         }
//         std::cout << "Request Body:\n"
//                   << body_ << std::endl;
//     }

// private:
//     std::string method_;
//     std::string path_;
//     std::string version_;
//     std::map<std::string, std::string> headers_;
//     std::string body_;
// };
#endif

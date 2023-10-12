#include "HttpRequest.h"

HttpRequest::HttpRequest()
{
}

HttpRequest::~HttpRequest()
{
}

HttpRequest::ParseRequestState HttpRequest::ParseRequest(std::string request)
{
    ParseRequestState state = ParseRequestState::kMethod;
    std::string key = "";
    std::string value = "";
    for (const auto& ch : request)
    {
        switch (state)
        {
        case ParseRequestState::kMethod:
            if (ch == ' ')
            {
                state = ParseRequestState::kPath;
            }
            else
            {
                method_.push_back(ch);
            }
            break;
        case ParseRequestState::kPath:
            if (ch == ' ')
            {
                state = ParseRequestState::kVersion;
            }
            else
            {
                path_.push_back(ch);
            }
            break;
        case ParseRequestState::kVersion:
            if (ch == '\r')
            {
                break;
            }
            else if (ch == '\n')
            {
                state = ParseRequestState::kHeaderKey;
            }
            else
            {
                version_.push_back(ch);
            }
            break;
        case ParseRequestState::kHeaderKey:
            if (ch == ':')
            {
                state = ParseRequestState::kHeaderValue;
            }
            else if (ch == '\r')
            {
                break;
            }
            else if (ch == '\n')
            {
                state = ParseRequestState::kBody;
            }
            else
            {
                key.push_back(ch);
            }
            break;
        case ParseRequestState::kHeaderValue:
            if (ch == '\r')
            {
                break;
            }
            else if (ch == '\n')
            {
                state = ParseRequestState::kHeaderKey;
                headers_[key] = value;
                key.clear();
                value.clear();
            }
            else
            {
                value.push_back(ch);
            }
            break;
        case ParseRequestState::kBody:
            body_.push_back(ch);
            break;
        }
    }
    return state;
}

HttpRequest::ParseRequestState HttpRequest::ParseRequest1(std::string request)
{
    





    




    return ParseRequestState();
}

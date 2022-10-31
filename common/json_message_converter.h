//
// Created by max on 29.10.2022.
//

#pragma once

#include <vector>

#include <boost/json.hpp>


#include "messages.h"


class JsonIncrementalMessageConverter
{
public:
    std::vector<char> encodeGreeting(const GreetingMessage &msg) const;
    std::vector<char> encodeReady(const ReadyMessage &msg) const;
    std::vector<char> encodeToken(const TokenMessage &msg) const;

    // Provides incremental interface to the converter.
    // Returns `true` when complete json was processed.
    bool processBytes(const char *data, size_t len);

    MessageVariant decodeMessage(const boost::json::value &value);
    MessageVariant getMessage();

protected:
    GreetingMessage decodeGreeting(const boost::json::object &json);
    ReadyMessage decodeReady(const boost::json::object &json);
    TokenMessage decodeToken(const boost::json::object &json);

    boost::json::stream_parser m_json_parser;
    std::vector<char> m_extra_buffer;

    static constexpr size_t MAX_EXTRA_BUFFER_SIZE = 10000;
    static constexpr const char* GREETING_MSG = "greeting";
    static constexpr const char* READY_MSG = "ready";
    static constexpr const char* TOKEN_MSG = "token";
};

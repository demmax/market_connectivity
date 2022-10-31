//
// Created by max on 29.10.2022.
//

#include <spdlog/spdlog.h>

#include "json_message_converter.h"


std::vector<char> JsonIncrementalMessageConverter::encodeGreeting(const GreetingMessage &msg) const
{
    boost::json::object obj;
    obj["message"] = GREETING_MSG;
    obj["client_id"] = msg.client_id;
    obj["tokens_count"] = msg.tokens_count;
    auto json_str = boost::json::serialize(obj);
    return std::vector(json_str.begin(), json_str.end());
}

std::vector<char> JsonIncrementalMessageConverter::encodeReady(const ReadyMessage &msg) const
{
    boost::json::object obj;
    obj["message"] = READY_MSG;
    auto json_str = boost::json::serialize(obj);
    return std::vector(json_str.begin(), json_str.end());
}

std::vector<char> JsonIncrementalMessageConverter::encodeToken(const TokenMessage &msg) const
{
    boost::json::object obj;
    obj["message"] = TOKEN_MSG;
    obj["token"] = msg.token;
    auto json_str = boost::json::serialize(obj);
    return std::vector(json_str.begin(), json_str.end());
}

MessageVariant JsonIncrementalMessageConverter::decodeMessage(const boost::json::value &value)
{
    if (!value.is_object() || !value.get_object().contains("message"))
        throw std::runtime_error("No 'message' field in the message");

    auto &json = value.get_object();
    std::string message{json.at("message").as_string().data()};

    if (message == GREETING_MSG)
        return decodeGreeting(json);
    else if (message == READY_MSG)
        return decodeReady(json);
    else if (message == TOKEN_MSG)
        return decodeToken(json);

    throw std::runtime_error("Unknown message: " + message);
}

GreetingMessage JsonIncrementalMessageConverter::decodeGreeting(const boost::json::object &json)
{
    int64_t count_received = json.at("tokens_count").as_int64();
    if (count_received < 0)
        throw std::runtime_error("`tokens_count` must be positive");

    return GreetingMessage{
        .client_id{json.at("client_id").as_string().data()},
        .tokens_count{static_cast<size_t>(count_received)}
    };
}

ReadyMessage JsonIncrementalMessageConverter::decodeReady(const boost::json::object &json)
{
    return ReadyMessage{};
}

TokenMessage JsonIncrementalMessageConverter::decodeToken(const boost::json::object &json)
{
    return TokenMessage{
        .token{json.at("token").as_string().data()}
    };
}

bool JsonIncrementalMessageConverter::processBytes(const char *data, size_t len)
{
    if (m_json_parser.done()) {
        // User has to read message; Bufferize for now.
        if (m_extra_buffer.size() + len > MAX_EXTRA_BUFFER_SIZE) {
            spdlog::warn("Buffer is full, new data is dropped.");
            return true;
        }

        m_extra_buffer.insert(m_extra_buffer.end(), data, data + len);
        return true;
    }

    size_t consumed = m_json_parser.write_some(data, len);
    if (consumed < len) {
        // Parser has got a complete json and any extra bytes will be dropped.
        // Save it and inform the caller that message is ready.
        m_extra_buffer.insert(m_extra_buffer.end(), data + consumed, data + len);
    }

    return m_json_parser.done();
}

MessageVariant JsonIncrementalMessageConverter::getMessage()
{
    if (!m_json_parser.done()) {
        return std::monostate();
    }

    auto result = decodeMessage(m_json_parser.release());
    m_json_parser.reset();

    if (!m_extra_buffer.empty()) {
        size_t consumed = m_json_parser.write_some(m_extra_buffer.data(), m_extra_buffer.size());
        if (consumed == m_extra_buffer.size()) {
            // Buffer was consumed completely, and we don't need it anymore.
            m_extra_buffer.clear();
        }
        else {
            m_extra_buffer.erase(m_extra_buffer.begin(), m_extra_buffer.begin() + consumed);
        }
    }

    return result;
}

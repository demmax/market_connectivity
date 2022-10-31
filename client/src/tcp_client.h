//
// Created by max on 29.10.2022.
//

#pragma once

#include <boost/asio.hpp>

#include "common/json_message_converter.h"


class TcpClient
{
public:
    TcpClient(boost::asio::io_context &io_context);

    void connect(const std::string &address, uint16_t port);
    void greetingHandshake(const std::string &client_id, size_t tokens_count);
    void sendToken(const std::string &token);

private:
    boost::asio::io_context &m_io_context;
    boost::asio::ip::tcp::socket m_socket;

    JsonIncrementalMessageConverter m_json_converter;
};

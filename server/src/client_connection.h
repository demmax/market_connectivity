//
// Created by max on 29.10.2022.
//

#pragma once

#include <boost/asio.hpp>

#include "common/json_message_converter.h"
#include "consumer_manager.h"

class ClientConnection;
using ClientConnectionPtr = std::shared_ptr<ClientConnection>;
using StatsMultiQueueProcessor = MultiQueueProcessor<std::string, Stats>;


class ClientConnection : public std::enable_shared_from_this<ClientConnection>
{
public:
    ClientConnection(boost::asio::io_context &io_context,
                     ConsumerManager *consumer_manager,
                     StatsMultiQueueProcessor *stats_processor);

    boost::asio::ip::tcp::socket& socket();

    void start();

private:
    void doRead();
    void handleRead(const boost::system::error_code& err,
                    std::size_t bytes_transferred);
    void sendReady();
    void handleSentReady(const boost::system::error_code& err,
                         std::size_t bytes_transferred);

    void processGreeting(const GreetingMessage &msg);
    void processTokenMessage(const TokenMessage &msg);

    boost::asio::ip::tcp::socket m_socket;
    std::array<char, 128> m_read_buffer;
    JsonIncrementalMessageConverter m_json_converter;

    std::string m_client_id;
    size_t m_expected_token_cnt{0};
    size_t m_received_token_cnt{0};

    ConsumerManager *p_consumer_manager;
    StatsMultiQueueProcessor *p_processor;
};

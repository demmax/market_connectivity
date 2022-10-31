//
// Created by max on 30.10.2022.
//

#pragma once

#include <boost/asio.hpp>

#include "client_connection.h"


class TcpServer
{
public:
    TcpServer(boost::asio::io_context &context,
              uint16_t port,
              size_t pool_size,
              ConsumerManager *consumer_manager,
              StatsMultiQueueProcessor *stats_processor);

    void run();

    void handleAccept(std::shared_ptr<ClientConnection> connection,
                      const boost::system::error_code& error);

private:
    void doAccept();

    boost::asio::io_context& m_io_context;
    boost::asio::ip::tcp::acceptor m_acceptor;
    size_t m_pool_size;
    ConsumerManager *p_consumer_manager;
    StatsMultiQueueProcessor *p_processor;
};

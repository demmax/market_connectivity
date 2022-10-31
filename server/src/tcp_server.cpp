//
// Created by max on 30.10.2022.
//

#include <boost/bind/bind.hpp>

#include <spdlog/spdlog.h>

#include "tcp_server.h"


TcpServer::TcpServer(boost::asio::io_context &context,
                     uint16_t port,
                     size_t pool_size,
                     ConsumerManager *consumer_manager,
                     StatsMultiQueueProcessor *stats_processor) :
    m_io_context{context},
    m_acceptor{context,
               boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)},
    m_pool_size{pool_size},
    p_consumer_manager{consumer_manager},
    p_processor{stats_processor}
{

}

void TcpServer::run()
{
    doAccept();
    spdlog::info("Start listening on port {}",
                 m_acceptor.local_endpoint().port());
}

void TcpServer::doAccept()
{
    auto connection = std::make_shared<ClientConnection>(m_io_context,
                                                         p_consumer_manager,
                                                         p_processor);

    m_acceptor.async_accept(connection->socket(),
                            boost::bind(&TcpServer::handleAccept,
                                        this,
                                        connection,
                                        boost::asio::placeholders::error
                            ));
}

void TcpServer::handleAccept(std::shared_ptr<ClientConnection> connection,
                             const boost::system::error_code& error)
{
    if (error) {
        spdlog::error(error.message());
    }
    else {
        spdlog::info("Connection accepted from {}:{}",
                     connection->socket().remote_endpoint().address().to_string(),
                     connection->socket().remote_endpoint().port());
        connection->start();
    }

    doAccept();
}

//
// Created by max on 29.10.2022.
//

#include "client_connection.h"
#include "MultiQueueProcessor.h"

#include <boost/bind/bind.hpp>

#include <spdlog/spdlog.h>


ClientConnection::ClientConnection(boost::asio::io_context &io_context,
                                   ConsumerManager *consumer_manager,
                                   StatsMultiQueueProcessor *stats_processor) :
    m_socket{io_context},
    p_consumer_manager{consumer_manager},
    p_processor{stats_processor}
{

}

void ClientConnection::start()
{
    doRead();
}

void ClientConnection::doRead()
{
    spdlog::trace("Reading from socket...");
    m_socket.async_read_some(boost::asio::buffer(m_read_buffer),
                             boost::bind(&ClientConnection::handleRead,
                                         shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred
                             ));
}

boost::asio::ip::tcp::socket& ClientConnection::socket()
{
    return m_socket;
}

void ClientConnection::handleRead(const boost::system::error_code& err,
                                  std::size_t bytes_transferred)
{
    if (err) {
        spdlog::error(err.message());
        return;
    }

    spdlog::trace("{} bytes received.", bytes_transferred);
    if (m_json_converter.processBytes(m_read_buffer.data(), bytes_transferred)) {
        MessageVariant message;
        do {
            message = m_json_converter.getMessage();
            if (std::holds_alternative<GreetingMessage>(message)) {
                processGreeting(std::get<GreetingMessage>(message));
            } else if (std::holds_alternative<TokenMessage>(message)) {
                processTokenMessage(std::get<TokenMessage>(message));
            }
        } while (!std::holds_alternative<std::monostate>(message));
    }

    doRead();
}

void ClientConnection::processGreeting(const GreetingMessage &msg)
{
    spdlog::trace("Process greeting message: {}", msg.client_id);

    if (!m_client_id.empty()) {
        spdlog::warn("Extra `Greeting` message. Ignore.");
        return;
    }

    if (msg.client_id.empty()) {
        spdlog::warn("Empty `client_id` in `Greeting` message. Ignore the message.");
        return;
    }

    if (!msg.tokens_count) {
        spdlog::warn("`tokens_count` is 0 in `Greeting` message. Ignore the message.");
        return;
    }

    m_client_id = msg.client_id;
    m_expected_token_cnt = msg.tokens_count;

    sendReady();
}

void ClientConnection::processTokenMessage(const TokenMessage &msg)
{
    spdlog::trace("Process token message: {}", msg.token);
    Consumer* consumer = p_consumer_manager->createConsumer(msg.token);
    if (consumer) {
        p_processor->Subscribe(msg.token, consumer);
    }

    p_processor->Enqueue(msg.token,
                         Stats{
        .client_id=m_client_id,
        .address=m_socket.remote_endpoint().address().to_string(),
        .port=m_socket.remote_endpoint().port(),
        .expected_client_tokens=m_expected_token_cnt,
        .received_client_tokens=++m_received_token_cnt
    });
}


void ClientConnection::sendReady()
{
    static std::vector<char> ready_msg_bytes = m_json_converter.encodeReady(ReadyMessage{});
    m_socket.async_write_some(boost::asio::buffer(ready_msg_bytes),
                              boost::bind(&ClientConnection::handleSentReady,
                                          shared_from_this(),
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred
                            ));
}

void ClientConnection::handleSentReady(const boost::system::error_code& err,
                                       std::size_t bytes_transferred)
{
    if (err) {
        spdlog::error("Error while sending `Ready`: {}", err.to_string());
        return;
    }
}

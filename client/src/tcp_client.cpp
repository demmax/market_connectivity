//
// Created by max on 29.10.2022.
//

#include <spdlog/spdlog.h>

#include "tcp_client.h"
#include "common/messages.h"


using boost::asio::ip::tcp;


TcpClient::TcpClient(boost::asio::io_context &io_context) :
    m_io_context{io_context},
    m_socket{m_io_context}
{

}

void TcpClient::connect(const std::string &address, uint16_t port)
{
    spdlog::trace("Connecting...");
    tcp::resolver resolver{m_io_context};
    tcp::resolver::results_type endpoints = resolver.resolve(address, std::to_string(port));
    boost::asio::connect(m_socket, endpoints);
}

void TcpClient::greetingHandshake(const std::string &client_id, size_t tokens_count)
{
    spdlog::trace("Sending greeting...");
    auto bytes = m_json_converter.encodeGreeting(
            GreetingMessage{.client_id=client_id,
                            .tokens_count=tokens_count});


    boost::asio::write(m_socket, boost::asio::buffer(bytes));

    spdlog::trace("Waiting for Ready...");

    std::array<char, 128> buf;
    do {
        size_t received = m_socket.read_some(boost::asio::buffer(buf));
        spdlog::trace("{} bytes received", received);
        if (m_json_converter.processBytes(buf.data(), received)) {
            break;
        }
    } while (true);

    MessageVariant message = m_json_converter.getMessage();
    if (!std::holds_alternative<ReadyMessage>(message)) {
        throw std::runtime_error("Unexpected message");
    }
}


void TcpClient::sendToken(const std::string &token)
{
    spdlog::trace("Sending token...");
    auto bytes = m_json_converter.encodeToken(TokenMessage{.token=token});
    boost::asio::write(m_socket, boost::asio::buffer(bytes));
}

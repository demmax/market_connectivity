//
// Created by max on 29.10.2022.
//

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include <spdlog/spdlog.h>
#include <fmt/core.h>

#include "tcp_client.h"


namespace po = boost::program_options;


int main(int argc, char **argv)
{
    std::string address;
    uint16_t port;
    std::string client_id;
    std::string tokens_file_path;
    size_t tokens_count;
    int log_level = spdlog::level::info;

    // read command-line options
    po::options_description desc{"Market Connectivity client options"};
    desc.add_options()
            ("help,h", "Print usage message")
            ("address,a", po::value(&address)->required(), "Address to connect to")
            ("port,p", po::value(&port)->required(), "Port to connect to")
            ("id,i", po::value(&client_id)->required(), "Client id")
            ("file,f", po::value(&tokens_file_path)->required(), "File with tokens")
            ("count,c", po::value(&tokens_count)->required(), "Count of tokens to send")
            ("log_level,l", po::value(&log_level), "Log level, 0 (trace) - 6 (no logging)");

    po::variables_map po_map;
    po::store(po::parse_command_line(argc, argv, desc), po_map);
    if (po_map.count("help")) {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }
    po::notify(po_map);

    spdlog::set_level((spdlog::level::level_enum)log_level);
    spdlog::set_pattern(fmt::format("[{}]%+", client_id));
    spdlog::info("Server endpoint: {}:{}", address, port);
    spdlog::info("Client ID: {}", client_id);
    spdlog::info("File with tokens: {}", tokens_file_path);
    spdlog::info("Tokens count to send: {}", tokens_count);
    spdlog::info("Log level: {}", log_level);

    // Check tokens file exists
    std::ifstream tokens_file(tokens_file_path);
    if (!tokens_file) {
        spdlog::error("Cannot open file {}", tokens_file_path);
        return 1;
    }

    // Initiate session
    boost::asio::io_context ctx;
    TcpClient client(ctx);
    client.connect(address, port);
    client.greetingHandshake(client_id, tokens_count);

    // Send tokens
    std::string token;
    while (tokens_count) {
        if (!std::getline(tokens_file, token)) {
            spdlog::error("End of file reached.");
            return 2;
        }

        boost::algorithm::trim(token);
        if (token.empty())
            continue;

        client.sendToken(token);
    }

    return 0;
}

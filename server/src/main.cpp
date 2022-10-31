//
// Created by max on 29.10.2022.
//
#include <cstdint>
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include <spdlog/spdlog.h>

#include "tcp_server.h"
#include "consumer_manager.h"


namespace po = boost::program_options;


int main(int argc, char **argv)
{
    uint16_t listen_port;
    size_t stat_store_interval;
    size_t client_pool_size;
    size_t handler_pool_size;
    std::string stats_file_path;
    int log_level = spdlog::level::info;

    // read command-line options
    po::options_description desc{"Market Connectivity server options"};
    desc.add_options()
        ("help,h", "Print usage message")
        ("port,p", po::value(&listen_port)->required(), "Port to listen on")
        ("stats_file,s", po::value(&stats_file_path)->required(), "Stats file path")
        ("interval,i", po::value(&stat_store_interval)->required(), "Interval to store stats")
        ("client_pool", po::value(&client_pool_size)->required(), "Client thread pool size")
        ("handler_pool", po::value(&handler_pool_size)->required(), "Handlers thread pool size")
        ("log_level,l", po::value(&log_level), "Log level, 0 (trace) - 6 (no logging)");

    po::variables_map po_map;
    po::store(po::parse_command_line(argc, argv, desc), po_map);
    if (po_map.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }
    po::notify(po_map);

    // configure logger
    spdlog::set_level((spdlog::level::level_enum)log_level);
    spdlog::set_pattern("[server]%+");

    spdlog::info("Listen port: {}", listen_port);
    spdlog::info("Stats file path: {}", stats_file_path);
    spdlog::info("Stats store interval: {}", stat_store_interval);
    spdlog::info("Client thread pool size: {}", client_pool_size);
    spdlog::info("Handlers thread pool size: {}", handler_pool_size);
    spdlog::info("Log level: {}", log_level);

    // Prepare objects
    std::fstream stat_file{stats_file_path, std::fstream::out};
    if (!stat_file) {
        spdlog::error("Cannot open stats file");
        return 1;
    }
    ConsumerManager consumer_manager(handler_pool_size,
                                     stat_file,
                                     stat_store_interval);

    // Start the server
    boost::asio::io_context io_context;
    StatsMultiQueueProcessor multi_queue_processor;

    TcpServer server(io_context,
                     listen_port,
                     client_pool_size,
                     &consumer_manager,
                     &multi_queue_processor);
    server.run();
    io_context.run();

    return 0;
}

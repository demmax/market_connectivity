//
// Created by max on 30.10.2022.
//

#pragma once

#include <boost/asio.hpp>

#include "MultiQueueProcessor.h"

struct Stats
{
    std::string client_id;
    std::string address;
    uint16_t port;
    size_t expected_client_tokens;
    size_t received_client_tokens;

    bool operator!=(const Stats& rhs) const {
        return client_id != rhs.client_id;
    }
};

using ClientsTable = std::map<std::string, Stats>;


class Consumer : public IConsumer<std::string, Stats>
{
public:
    Consumer(boost::asio::thread_pool &pool);
    virtual ~Consumer();

    void Consume(std::string key, const Stats &stats) override;

    void printTable(std::ostream &stream);

private:
    void updateStats(const Stats &stats);

    ClientsTable m_table;
    std::mutex m_mtx;
    boost::asio::thread_pool &m_thread_pool;
};

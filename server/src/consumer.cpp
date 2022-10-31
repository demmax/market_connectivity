//
// Created by max on 30.10.2022.
//

#include "consumer.h"

#include <fmt/core.h>


Consumer::Consumer(boost::asio::thread_pool &pool) :
    m_thread_pool(pool)
{

}

void Consumer::printTable(std::ostream &stream)
{
    std::lock_guard<std::mutex> lock{m_mtx};

    for (auto& [client_id, stats] : m_table) {
        stream << "Client ID: " << client_id << std::endl;
        stream << fmt::format("Address: {}:{}", stats.address, stats.port) << std::endl;
        stream << "Expected client tokens count: " << stats.expected_client_tokens << std::endl;
        stream << "Received client tokens count: " << stats.received_client_tokens << std::endl;
        stream << "--------------------------------" << std::endl;
    }
}


void Consumer::Consume(std::string key, const Stats &stats)
{
    boost::asio::post(m_thread_pool,
                      [this, stats]() {
                          updateStats(stats);
    });
}


void Consumer::updateStats(const Stats &stats)
{
    std::lock_guard<std::mutex> lock{m_mtx};

    auto it = m_table.find(stats.client_id);
    if (it == m_table.end()) {
        m_table.insert(std::make_pair(stats.client_id, stats));
    }
    else {
        it->second.received_client_tokens = stats.received_client_tokens;
    }
}

Consumer::~Consumer()
{
    m_thread_pool.join();
}

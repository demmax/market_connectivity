//
// Created by max on 30.10.2022.
//

#include "consumer_manager.h"

#include <spdlog/spdlog.h>


ConsumerManager::ConsumerManager(size_t consumers_pool_size,
                                 std::ostream &stats_file,
                                 size_t stats_interval) :
    m_pool{consumers_pool_size},
    m_stats_file{stats_file},
    m_stats_interval{stats_interval},
    m_dumping_thread{std::bind(&ConsumerManager::dumpingThread, this)}
{

}

Consumer *ConsumerManager::createConsumer(const std::string &token)
{
    std::lock_guard lock{m_mtx};
    if (m_consumers.find(token) != m_consumers.end()) {
        spdlog::trace("Consumer for token {} already exists", token);
        return nullptr;
    }

    spdlog::info("Creating a consumer for token {}", token);
    auto [iter, is_ok] = m_consumers.emplace(token, m_pool);
    if (!is_ok) {
        spdlog::error("Cannot create consumer for token {}!", token);
        return nullptr;
    }

    return &iter->second;
}

void ConsumerManager::dumpStats()
{
    std::lock_guard lock{m_mtx};

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);

    m_stats_file << std::ctime(&time_t_now) << std::endl;
    m_stats_file << "Total number of consumers: " << m_consumers.size() << std::endl;
    m_stats_file << "--------------------------------" << std::endl;
    for (auto& [token, consumer] : m_consumers) {
        m_stats_file << "Token: " << token << std::endl;
        consumer.printTable(m_stats_file);
    }
    m_stats_file << "================================" << std::endl << std::endl;
    m_stats_file.flush();
}

void ConsumerManager::dumpingThread()
{
    while (!m_stopped) {
        size_t interval = m_stats_interval;
        while (--interval) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (m_stopped) {
                return;
            }
        }

        dumpStats();
    }
}

ConsumerManager::~ConsumerManager()
{
    m_stopped = true;
    m_dumping_thread.join();
}


//
// Created by max on 30.10.2022.
//

#pragma once

#include <ostream>

#include "consumer.h"


class ConsumerManager
{
public:
    ConsumerManager(size_t consumers_pool_size,
                    std::ostream &stats_file,
                    size_t stats_interval);
    ~ConsumerManager();

    // Returns nullptr if consumer already exists
    Consumer* createConsumer(const std::string &token);

private:
    void dumpStats();
    void dumpingThread();

    std::mutex m_mtx;
    boost::asio::thread_pool m_pool;
    std::map<std::string, Consumer> m_consumers;
    std::ostream &m_stats_file;
    size_t m_stats_interval;

    std::thread m_dumping_thread;
    std::atomic<bool> m_stopped{false};
};

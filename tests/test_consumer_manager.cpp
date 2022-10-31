//
// Created by max on 30.10.2022.
//

#include <gtest/gtest.h>

#include <sstream>

#include "server/src/consumer_manager.h"


TEST(TestConsumerManager, test_creating)
{
    std::stringstream ss;
    ConsumerManager manager{1, ss, 10};

    Consumer* con1 = manager.createConsumer("token1");
    Consumer* con2 = manager.createConsumer("token2");
    Consumer* con3 = manager.createConsumer("token3");
    Consumer* dup_con1 = manager.createConsumer("token1");
    Consumer* dup_con2 = manager.createConsumer("token2");
    Consumer* dup_con3 = manager.createConsumer("token3");

    ASSERT_TRUE(con1);
    ASSERT_TRUE(con2);
    ASSERT_TRUE(con3);
    ASSERT_FALSE(dup_con1);
    ASSERT_FALSE(dup_con2);
    ASSERT_FALSE(dup_con3);
}

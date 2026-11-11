#include <gtest/gtest.h>

extern "C" {
#include "consumer.h"
}

TEST(ConsumerTest, AddConsumer) {
    Consumer consumers[MAX_CONSUMERS];
    int count = 0;

    StatusCode status = addConsumer(
        consumers, &count, 101, "Aryan", 500.0
    );

    EXPECT_EQ(status, STATUS_OK);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(consumers[0].id, 101);
    EXPECT_STREQ(consumers[0].name, "Aryan");
    EXPECT_DOUBLE_EQ(consumers[0].lastReading, 500.0);
}

TEST(ConsumerTest, DuplicateConsumerID) {
    Consumer consumers[MAX_CONSUMERS];
    int count = 0;

    ASSERT_EQ(addConsumer(consumers, &count, 101, "Aryan", 500.0), STATUS_OK);

    StatusCode status = addConsumer(
        consumers, &count, 101, "Rahul", 600.0
    );

    EXPECT_EQ(status, STATUS_DUPLICATE);
    EXPECT_EQ(count, 1);
}

TEST(ConsumerTest, FindConsumer) {
    Consumer consumers[MAX_CONSUMERS];
    Consumer *foundConsumer = nullptr;
    int count = 0;

    ASSERT_EQ(addConsumer(consumers, &count, 101, "Aryan", 500.0), STATUS_OK);

    StatusCode status = findConsumer(consumers, count, 101, &foundConsumer);

    EXPECT_EQ(status, STATUS_OK);
    ASSERT_NE(foundConsumer, nullptr);
    EXPECT_STREQ(foundConsumer->name, "Aryan");
}

TEST(ConsumerTest, ConsumerNotFound) {
    Consumer consumers[MAX_CONSUMERS];
    Consumer *foundConsumer = nullptr;
    int count = 0;

    StatusCode status = findConsumer(consumers, count, 999, &foundConsumer);

    EXPECT_EQ(status, STATUS_NOT_FOUND);
    EXPECT_EQ(foundConsumer, nullptr);
}

TEST(ConsumerTest, UpdateConsumerName) {
    Consumer consumers[MAX_CONSUMERS];
    int count = 0;

    ASSERT_EQ(addConsumer(consumers, &count, 101, "Aryan", 500.0), STATUS_OK);

    StatusCode status = updateConsumerName(
        consumers, count, 101, "Aarav"
    );

    EXPECT_EQ(status, STATUS_OK);
    EXPECT_STREQ(consumers[0].name, "Aarav");
}

TEST(ConsumerTest, EmptyConsumerNameIsRejected) {
    Consumer consumers[MAX_CONSUMERS];
    int count = 0;

    StatusCode status = addConsumer(
        consumers, &count, 101, "", 500.0
    );

    EXPECT_EQ(status, STATUS_INVALID_INPUT);
    EXPECT_EQ(count, 0);
}

TEST(ConsumerTest, InvalidInitialReadingIsRejected) {
    Consumer consumers[MAX_CONSUMERS];
    int count = 0;

    EXPECT_EQ(
        addConsumer(consumers, &count, 101, "Aryan", -10.0),
        STATUS_INVALID_INPUT
    );
    EXPECT_EQ(count, 0);
}

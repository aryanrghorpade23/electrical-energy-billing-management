CC = gcc
CXX = g++

CFLAGS = -Wall -Wextra -std=c11 -Iinclude
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

SOURCES = src/main.c src/consumer.c src/billing.c src/file_handler.c
TARGET = billing_system

TEST_SOURCES = tests/test_billing.cpp tests/test_consumer.cpp
TEST_OBJECTS = consumer_test.o billing_test.o
TEST_TARGET = billing_tests

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(SOURCES) include/types.h include/consumer.h include/billing.h include/file_handler.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) -lm

run: $(TARGET)
	./$(TARGET)

consumer_test.o: src/consumer.c include/consumer.h include/types.h
	$(CC) $(CFLAGS) -c src/consumer.c -o consumer_test.o

billing_test.o: src/billing.c include/billing.h include/types.h
	$(CC) $(CFLAGS) -c src/billing.c -o billing_test.o

$(TEST_TARGET): $(TEST_SOURCES) consumer_test.o billing_test.o
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) $(TEST_OBJECTS) -o $(TEST_TARGET) -lgtest -pthread

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o

/*
 * Copyright (c) 2019 Matt Gigli
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE
 * SOFTWARE.
 */

#include <gtest/gtest.h>
#include <chrono>
#include <thread>

#include "libaopp/event_queue.hpp"

class TestEvent : public aopp::Event {
 public:
  TestEvent() {}
  TestEvent(int i) : foo_(i) {}
  ~TestEvent() = default;

  int foo() const { return foo_; }
  void foo(int f) { foo_ = f; }

 private:
  int foo_ = 0;
};

class EventQueueTest : public ::testing::Test {
 protected:
  aopp::EventQueue queue_;
};

TEST(EventQueueConstructorTest, DefaultConstructor) {
  aopp::EventQueue queue;
  EXPECT_TRUE(queue.empty());
  EXPECT_FALSE(queue.full());
  EXPECT_EQ(queue.max_size(), -1);
  EXPECT_EQ(queue.size(), 0);
}

TEST(EventQueueConstructorTest, MaxSizeConstructor) {
  aopp::EventQueue queue(5);
  EXPECT_TRUE(queue.empty());
  EXPECT_FALSE(queue.full());
  EXPECT_EQ(queue.max_size(), 5);
  EXPECT_EQ(queue.size(), 0);

  std::shared_ptr<aopp::Event> evt = std::make_shared<aopp::Event>();
  for (std::size_t i = 0; i < queue.max_size(); i++) {
    EXPECT_FALSE(queue.full());
    bool success = queue.try_push(evt);
    EXPECT_TRUE(success);
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), i + 1);
  }

  bool success = queue.try_push(evt);
  EXPECT_FALSE(success);
  EXPECT_TRUE(queue.full());
}

TEST_F(EventQueueTest, TryPushBase) {
  std::shared_ptr<aopp::Event> evt = std::make_shared<aopp::Event>();
  bool success = queue_.try_push(evt);
  EXPECT_TRUE(success);
  EXPECT_FALSE(queue_.empty());
  EXPECT_EQ(queue_.size(), 1);
}

TEST_F(EventQueueTest, TryPushConstBase) {
  std::shared_ptr<const aopp::Event> evt = std::make_shared<aopp::Event>();
  bool success = queue_.try_push(evt);
  EXPECT_TRUE(success);
  EXPECT_FALSE(queue_.empty());
  EXPECT_EQ(queue_.size(), 1);
}

TEST_F(EventQueueTest, TryPushSubclass) {
  std::shared_ptr<TestEvent> evt = std::make_shared<TestEvent>();
  bool success = queue_.try_push(evt);
  EXPECT_TRUE(success);
  EXPECT_FALSE(queue_.empty());
  EXPECT_EQ(queue_.size(), 1);
}

TEST_F(EventQueueTest, TryPushConstSubclass) {
  std::shared_ptr<const TestEvent> evt = std::make_shared<const TestEvent>();
  bool success = queue_.try_push(evt);
  EXPECT_TRUE(success);
  EXPECT_FALSE(queue_.empty());
  EXPECT_EQ(queue_.size(), 1);
}

TEST_F(EventQueueTest, PopBase) {
  std::shared_ptr<const TestEvent> evt = std::make_shared<const TestEvent>();
  bool success = queue_.try_push(evt);
  EXPECT_TRUE(success);

  std::shared_ptr<const aopp::Event> e = queue_.pop();
  EXPECT_TRUE(queue_.empty());
  EXPECT_EQ(queue_.size(), 0);
}

TEST_F(EventQueueTest, PopSubclass) {
  std::shared_ptr<TestEvent> evt1 = std::make_shared<TestEvent>(10);
  bool success = queue_.try_push(evt1);
  EXPECT_TRUE(success);
  std::shared_ptr<TestEvent> evt2 = std::make_shared<TestEvent>(20);
  success = queue_.try_push(evt2);
  EXPECT_TRUE(success);
  EXPECT_EQ(queue_.size(), 2);

  const TestEvent& test_evt1 =
      reinterpret_cast<const TestEvent&>(*queue_.pop());
  EXPECT_EQ(test_evt1.foo(), 10);
  const TestEvent& test_evt2 =
      reinterpret_cast<const TestEvent&>(*queue_.pop());
  EXPECT_EQ(test_evt2.foo(), 20);
  EXPECT_TRUE(queue_.empty());
  EXPECT_EQ(queue_.size(), 0);
}

TEST_F(EventQueueTest, NoMaxSize) {
  // We can't actually test no max size, but 1,000 seems reasonable
  for (std::size_t i = 0; i < 1000; i++) {
    std::shared_ptr<aopp::Event> evt = std::make_shared<aopp::Event>();
    bool success = queue_.try_push(evt);
    EXPECT_FALSE(queue_.full());
    EXPECT_TRUE(success);
    EXPECT_FALSE(queue_.empty());
    EXPECT_EQ(queue_.size(), i + 1);
  }
}

TEST_F(EventQueueTest, ProducerConsumer) {
  std::thread producer([&]() {
    for (int i = 0; i < 1000; i++) {
      auto evt = std::make_shared<const TestEvent>(i);
      queue_.try_push(evt);
    }
  });

  std::thread consumer([&]() {
    for (int i = 0; i < 1000; i++) {
      const TestEvent& evt = reinterpret_cast<const TestEvent&>(*queue_.pop());
      EXPECT_EQ(evt.foo(), i);
    }
  });

  producer.join();
  consumer.join();
}

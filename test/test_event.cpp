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
#include <memory>

#include "libaopp/event.hpp"

class TestEvent : public aopp::Event {
 public:
  TestEvent() {}
  ~TestEvent() = default;

  int foo() const { return foo_; }
  void foo(int f) { foo_ = f; }

 private:
  int foo_ = 0;
};

TEST(EventConstructorTest, DefaultCOnstructor) { TestEvent evt; }

TEST(EventPtrConstructionTest, NonConst) {
  std::shared_ptr<TestEvent> evt = aopp::make_event<TestEvent>();
  EXPECT_EQ(evt->foo(), 0);
  evt->foo(5);
  EXPECT_EQ(evt->foo(), 5);
}

TEST(EventPtrConstructionTest, Const) {
  std::shared_ptr<const TestEvent> evt = aopp::make_event<const TestEvent>();
  EXPECT_EQ(evt->foo(), 0);
  // evt->foo(5); // Wouldn't compile, because `evt` is const pointer
}

TEST(EventPtrCastTest, TestEvent) {
  std::shared_ptr<aopp::Event> evt = aopp::make_event<TestEvent>();
  TestEvent& test_evt = reinterpret_cast<TestEvent&>(*evt);
  test_evt.foo(5);
  EXPECT_EQ(test_evt.foo(), 5);
}

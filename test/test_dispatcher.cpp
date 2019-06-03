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

#include "libaopp/util/dispatcher.hpp"

class A {
 public:
  A() : some_data(0){};
  A(int d) : some_data(d){};
  virtual ~A() = default;

  int some_data;
};

class B : public A {};
class C : public A {};
class D : public A {};
class E : public A {};

int handle_event(const A& a) {
  static auto dispatch = aopp::util::make_dispatcher<A, int>(
      [&](const A& a) {
        (void)a;
        std::cout << "Event not handled" << std::endl;
        return 0;
      },
      [&](const B& b) {
        (void)b;
        std::cout << "B event handled" << std::endl;
        return 1;
      },
      [&](const C& c) {
        (void)c;
        std::cout << "C event handled" << std::endl;
        return 2;
      },
      [&](const D& d) {
        (void)d;
        std::cout << "D event handled" << std::endl;
        return 3;
      });

  return dispatch(a);
}

TEST(Dispatcher, DispatchDerivedEvents) {
  int r;
  A* evt;

  B b;
  evt = &b;
  r = handle_event(*evt);
  EXPECT_EQ(r, 1);

  C c;
  evt = &c;
  r = handle_event(*evt);
  EXPECT_EQ(r, 2);

  D d;
  evt = &d;
  r = handle_event(*evt);
  EXPECT_EQ(r, 3);

  E e;
  evt = &e;
  r = handle_event(*evt);
  EXPECT_EQ(r, 0);
}

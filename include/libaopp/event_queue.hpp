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

#ifndef LIBAOPP_EVENT_QUEUE_HPP
#define LIBAOPP_EVENT_QUEUE_HPP

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

#include "libaopp/event.hpp"

namespace aopp {

class EventQueue {
 public:
  EventQueue(std::size_t max_size);
  EventQueue();
  ~EventQueue();

  // Pushes the given element value to the end of the queue.
  // The content of value is copied to the new element. Increases container
  // size by one.
  // Does not block, returns true if push was successful, false if
  // queue is full.
  bool try_push(std::shared_ptr<const Event> value);

  // Removes the first element in the queue, reducing its size by one.
  // Blocks if queue is empty.
  std::shared_ptr<const Event> pop();

  // Returns true if queue is empty, false if it is not.
  bool empty() const noexcept;

  // Returns the number of elements in the queue.
  std::size_t size() const noexcept;

  // Returns true if queue is full, false if it is not.
  bool full() const noexcept;

  // Returns the max size of this queue.
  std::size_t max_size() const noexcept;

  // EventQueue class does not support copying or moving.
  EventQueue(const EventQueue&) = delete;
  EventQueue& operator=(const EventQueue&) = delete;
  EventQueue(EventQueue&&) = delete;
  EventQueue& operator=(EventQueue&&) = delete;

 private:
  std::queue<std::shared_ptr<const Event>> q_;
  mutable std::mutex mutex_;
  std::condition_variable pop_cond_;
  std::size_t max_size_ = -1;
};

}  // namespace aopp

#endif  // LIBAOPP_EVENT_QUEUE_HPP

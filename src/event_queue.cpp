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

#include "libaopp/event_queue.hpp"

namespace aopp {

EventQueue::EventQueue(std::size_t max_size) : max_size_(max_size) {}

EventQueue::EventQueue() = default;

EventQueue::~EventQueue() = default;

bool EventQueue::try_push(std::shared_ptr<const Event> value) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (q_.size() == max_size_) {
    return false;
  }

  q_.push(value);
  lock.unlock();
  pop_cond_.notify_one();
  return true;
}

std::shared_ptr<const Event> EventQueue::pop() {
  std::unique_lock<std::mutex> lock(mutex_);
  while (q_.empty()) {
    // std::condition_variable.wait() may return spuriously.
    // Use while loop to check for condition we're actually waiting for.
    pop_cond_.wait(lock);
  }

  auto value = q_.front();
  q_.pop();
  return value;
}

bool EventQueue::empty() const noexcept {
  std::unique_lock<std::mutex> lock(mutex_);
  return q_.empty();
}

std::size_t EventQueue::size() const noexcept {
  std::unique_lock<std::mutex> lock(mutex_);
  return q_.size();
}

bool EventQueue::full() const noexcept {
  std::unique_lock<std::mutex> lock(mutex_);
  return q_.size() == max_size_;
}

std::size_t EventQueue::max_size() const noexcept {
  std::unique_lock<std::mutex> lock(mutex_);
  return max_size_;
}

}  // namespace aopp

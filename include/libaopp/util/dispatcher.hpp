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

#ifndef LIBAOPP_UTIL_DISPATCHER_HPP
#define LIBAOPP_UTIL_DISPATCHER_HPP

#include <functional>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include "libaopp/util/function_traits.hpp"

namespace aopp {
namespace util {

template <typename Base, typename ResultType = void,
          typename CallbackType = std::function<ResultType(const Base&)> >
class Dispatcher {
 public:
  // Add the given callback function to the dispatcher map.
  // Uses the typeid of the callback's parameter, which is inferred via
  // template deduction, as the key to the dispatcher map.
  template <typename T>
  void Add(std::function<ResultType(const T&)> cb) {
    // Create a wrapper around the callback, allowing the dispatcher to be
    // called with a base class const reference to the actual parameter
    // object that is dispatched.
    auto cb_wrapper = [cb](const Base& b) {
      return cb(dynamic_cast<const T&>(b));
    };

    // Save off the wrapper, not the actual callback.
    map_[std::type_index(typeid(T))] = cb_wrapper;
  }

  // Dispatch the given base class const reference.
  // Uses the typeid of the base class object to call the function that takes
  // the derived class const reference as the argument.
  // Falls back on the base class callback function, if one exists.
  ResultType operator()(const Base& b) {
    // TODO: Use if-else to check if typeid(b) in map
    try {
      std::type_index t = std::type_index(typeid(b));
      return map_.at(t)(b);
    } catch (const std::out_of_range& e) {
      std::type_index t = std::type_index(typeid(Base));
      return map_.at(t)(b);
    }
  }

 private:
  std::unordered_map<std::type_index, CallbackType> map_;
};

template <typename Base, typename ReturnType, typename CB>
void AddCallback(Dispatcher<Base, ReturnType>& dispatcher, CB cb) {
  using arg_type = typename function_traits<CB>::arg_type;
  dispatcher.template Add<arg_type>(cb);
}

template <typename Base, typename ReturnType, typename CB, typename... CBs>
void AddCallback(Dispatcher<Base, ReturnType>& dispatcher, CB cb, CBs... cbs) {
  AddCallback<Base>(dispatcher, cb);
  AddCallback<Base>(dispatcher, cbs...);
}

template <typename Base, typename ReturnType, typename... CBs>
Dispatcher<Base, ReturnType> make_dispatcher(CBs... cbs) {
  Dispatcher<Base, ReturnType> dispatcher;
  AddCallback<Base, ReturnType>(dispatcher, cbs...);
  return dispatcher;
}

}  // namespace util
}  // namespace aopp

#endif  // LIBAOPP_UTIL_DISPATCHER_HPP

#ifndef ECMASCRIPT_MATCHER_H
#define ECMASCRIPT_MATCHER_H

#include <iostream>
#include <string>
#include <type_traits>

template <typename T, typename It> class Matcher {

  It m_begin, m_cur, m_end;

public:
  Matcher(It begin, It end) : m_begin(begin), m_cur(begin), m_end(end){};

  bool match(const T &value)
  {
    if (m_cur != m_end && *m_cur == value)
      return ++m_cur, true;
    return false;
  }

  template <typename Pred> auto match(Pred &&pred) -> decltype(pred(), bool())
  {
    auto m = m_cur;
    if (pred())
      return true;
    return m_cur = m, false;
  }

  template <typename Pred>
  auto match(Pred &&pred) -> decltype(pred(*m_cur), bool())
  {
    auto m = m_cur;
    if (m_cur != m_end && pred(*m_cur++))
      return true;
    return m_cur = m, false;
  }

  bool match(const char *value)
  {
    return match(value, std::is_constructible<T, const char *>());
  }

  bool match(const T &value, std::true_type)
  // This method creates a T from value and matches that
  {
    return match(value);
  }

  bool match(const char *value, std::false_type)
  // This overload treats value as a container and matches its chars one by one
  {
    auto m = m_cur;
    while (*value) {
      if (!match(*value++))
        return m_cur = m, false;
    }
    return true;
  }

  template <typename Pred> auto rmatch(Pred &&pred) -> decltype(pred(), bool())
  {
    auto m = m_cur;
    if (pred())
      return true;
    return m_cur = m, false;
  }

  template <typename Pred>
  auto rmatch(Pred &&pred) -> decltype(pred(*m_cur), bool())
  {
    auto m = m_cur;
    std::cout << "rmatch: " << (m_cur == m_begin) << (m_cur == m_end)
              << std::endl;
    if (m_cur != m_begin && pred(*(--m_cur)))
      return true;
    return m_cur = m, false;
  }

  bool peek(const T &value) const { return m_cur != m_end && *m_cur == value; }

  template <typename Pred>
  auto peek(Pred &&pred) const -> decltype(pred(*m_cur), bool())
  {
    return m_cur != m_end && pred(*m_cur);
  }

  template <typename Arg, typename... Args>
  bool any_of(Arg &&arg, Args &&... args)
  {
    return match(arg) || any_of(std::forward<Args>(args)...);
  }

  bool any_of() { return false; }

  It mark() const { return m_cur; }

  void reset(It mark) { m_cur = mark; }

  void reset() { m_cur = m_begin; }

  template <typename... Args> bool operator()(Args &&... args)
  {
    return match(std::forward<Args>(args)...);
  }

  const T* operator->() const
  {
    return matched();
  }

  const T *matched() const
  {
    return m_cur != m_begin ? &*(m_cur - 1) : nullptr;
  }

  const It matching() const { return m_cur; }

  operator const T *() const { return matched(); }
  operator const T &() const
  // Dereference without a positive match is undefined
  {
    return *(m_cur - 1);
  }

  template <typename U> operator U() const { return *matched(); }
  operator std::string() const { return *matched(); }
};

#endif
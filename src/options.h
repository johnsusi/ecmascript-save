#ifndef ECMASCRIPT_OPTIONS_H
#define ECMASCRIPT_OPTIONS_H

#include "variant.hpp"

#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Options {
  using Key = std::string;
  using Option =
      mpark::variant<bool, int, double, std::string, std::vector<std::string>>;
  using Data = std::unordered_map<std::string, Option>;

  std::shared_ptr<Data> data = std::make_shared<Data>();

  struct ReadOrWrite {
    std::weak_ptr<Data> dataRef;
    Key                 key;

    auto data() -> decltype(dataRef.lock());
    auto data() const -> decltype(dataRef.lock());

    ReadOrWrite& operator=(const char* value);

    template <typename T>
    ReadOrWrite& operator=(T&& value)
    {
      (*data())[key] = std::forward<T>(value);
      return *this;
    }

    template <typename T>
    operator T() const
    {
      using namespace std;
      return get<T>(data()->at(key));
    }

    const Option& value() const;

    auto begin() const
    {
      using namespace std;
      return get<vector<string>>(data()->at(key)).begin();
    }
    auto end() const
    {
      using namespace std;
      return get<vector<string>>(data()->at(key)).end();
    }

    template <typename T>
    auto push_back(T&& value)
    {
      using namespace std;
      return get<vector<string>>(data()->at(key))
          .push_back(std::forward<T>(value));
    }
    bool empty() const
    {
      using namespace std;
      return get<vector<string>>(data()->at(key)).empty();
    }

    template <typename T>
    bool operator==(const T& other) const
    {
      using namespace std;
      return get<T>(data()->at(key)) == other;
    }
  };

  template <typename T>
  Option make_option(T&& v)
  {
    return Option{v};
  }

  template <typename... Ts>
  Option make_option(const mpark::variant<Ts...>& v)
  {
    using namespace std;
    return visit([](auto value) { return value; }, v);
  }

public:
  Options();
  Options(std::initializer_list<std::pair<Key, Option>> il);
  Options(const Options&) = default;
  Options(Options&&)      = default;

  template <typename T>
  Options(T&& c)
  {
    for (auto&& kv : c) {
      Key    k = kv.first;
      Option v = make_option(kv.second);
      data->emplace(k, v);
    }
  }

  ReadOrWrite operator[](const Key& key);
  const ReadOrWrite operator[](const Key& key) const;

  std::size_t count(const Key&) const;

  friend std::ostream& operator<<(std::ostream&, const Options::ReadOrWrite&);

  auto begin() const
  {
    return data->begin();
  }
  auto end() const
  {
    return data->end();
  }
};

#endif
#pragma once
#ifndef LIBECMASCRIPT_JSON_HPP
#define LIBECMASCRIPT_JSON_HPP

#include <iosfwd>
#include <map>
#include <string_view>
#include <variant>
#include <vector>

struct JsonValue
{
    bool is_null() const;
    bool is_true() const;
    bool is_false() const;
    bool is_number() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;

    std::string to_string() const;

    JsonValue operator[](std::size_t index) const;
    JsonValue operator[](const char* key) const;

    struct null_t;
    struct true_t;
    struct false_t;
    struct number_t;
    struct string_t;
    struct array_t;
    struct object_t;

    using value_t = std::variant<std::monostate, null_t, false_t, true_t, number_t, string_t, array_t, object_t>;

    JsonValue(value_t value);
    ~JsonValue();

    struct null_t
    {
    };

    struct true_t
    {
    };

    struct false_t
    {
    };

    struct number_t
    {
        std::string_view::iterator start, end;
    };

    struct string_t
    {
        std::string_view::iterator start, end;
        bool operator<(const string_t &other) const;
    };

    struct array_t
    {
        std::vector<value_t> elements;
    };

    struct object_t
    {
        std::map<string_t, value_t> members;
    };
    friend std::ostream& operator<<(std::ostream&, const JsonValue&);
  private:
    value_t _value;
};

JsonValue parse_json(std::string_view source);

bool operator==(const JsonValue& lhs, const std::string_view& rhs);
std::ostream& operator<<(std::ostream& out, const JsonValue& value);

#endif
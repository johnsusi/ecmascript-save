#include "json.hpp"

#include <algorithm>
#include <sstream>

#include <iostream>

bool JsonValue::string_t::operator<(const JsonValue::string_t &other) const
{
    return std::string_view(start, end) <
           std::string_view(other.start, other.end);
}

JsonValue::JsonValue(value_t value) : _value(value)
{
}

JsonValue::~JsonValue()
{
}

bool JsonValue::is_null() const
{
    return std::get_if<null_t>(&_value) != nullptr;
}

bool JsonValue::is_false() const
{
    return std::get_if<false_t>(&_value) != nullptr;
}

bool JsonValue::is_true() const
{
    return std::get_if<true_t>(&_value) != nullptr;
}

bool JsonValue::is_number() const
{
    return std::get_if<number_t>(&_value) != nullptr;
}

bool JsonValue::is_string() const
{
    return std::get_if<string_t>(&_value) != nullptr;
}

bool JsonValue::is_array() const
{
    return std::get_if<array_t>(&_value) != nullptr;
}

bool JsonValue::is_object() const
{
    return std::get_if<object_t>(&_value) != nullptr;
}

std::ostream &operator<<(std::ostream &out, const JsonValue::value_t &value);

std::ostream &operator<<(std::ostream &out, const std::monostate &value)
{
    return out;
}

std::ostream &operator<<(std::ostream &out, const JsonValue::null_t &value)
{
    return out << "null";
}

std::ostream &operator<<(std::ostream &out, const JsonValue::false_t &value)
{
    return out << "false";
}

std::ostream &operator<<(std::ostream &out, const JsonValue::true_t &value)
{
    return out << "true";
}

std::ostream &operator<<(std::ostream &out, const JsonValue::number_t &value)
{
    return out << std::string_view(value.start, value.end);
}

std::ostream &operator<<(std::ostream &out, const JsonValue::string_t &value)
{
    return out << "\"" << std::string_view(value.start, value.end) << "\"";
}

std::ostream &operator<<(std::ostream &out, const JsonValue::array_t &value)
{
    out << "[";
    bool first = true;
    for (const auto &element : value.elements)
    {
        if (first)
            first = false;
        else
            out << ",";
        out << element;
    }
    out << "]";
    return out;
}

std::ostream &operator<<(std::ostream &out, const JsonValue::object_t &value)
{
    auto insertion_index = std::vector<JsonValue::string_t>();
    
    for (const auto &[key, val] : value.members)
        insertion_index.push_back(key);
    
    std::sort(insertion_index.begin(), insertion_index.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.start < rhs.start;
    });

    out << "{";
    bool first = true;

    for (auto key : insertion_index)
    {
        if (first)
            first = false;
        else
            out << ",";
        out << key << ":" << value.members.at(key);
    }   
    out << "}";
    return out;
}

std::ostream &operator<<(std::ostream &out, const JsonValue::value_t &value)
{
    std::visit([&out](const auto &v) { out << v; }, value);
    return out;
}

std::ostream& operator<<(std::ostream& out, const JsonValue& value)
{
    return out << value._value;
}

bool operator==(const JsonValue& lhs, const std::string_view& rhs)
{
    return lhs.to_string() == rhs;
}

std::string JsonValue::to_string() const
{
    std::stringstream buffer;
    buffer << _value;
    return buffer.str();
}

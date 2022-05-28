#include "json.hpp"

#include <stdexcept>

bool match(auto &&it, auto end, auto &&pred)
{
    auto jt = it;
    if (pred(jt, end))
    {
        it = jt;
        return true;
    }
    return false;
}

bool is_match(auto &it, auto end, const char c)
{
    if (it != end && *it == c)
    {
        ++it;
        return true;
    }
    return false;
}

bool is_match(auto &it, auto end, const char *c)
{

    auto jt = it;
    for (; *c; jt++, c++)
        if (*jt != *c)
            return false;
    it = jt;
    return true;
}

bool is_match(auto &it, auto end, auto &&pred)
{
    if (it != end && pred(*it))
    {
        ++it;
        return true;
    }
    return false;
}

bool is_ws(auto &&it, auto end)
{
    while (it != end)
    {
        switch (*it)
        {
        case 0x0020:
            ++it;
            break;
        case 0x000A:
            ++it;
            break;
        case 0x000D:
            ++it;
            break;
        case 0x0009:
            ++it;
            break;
        default:
            return true;
        }
    }
    return true;
}

bool is_null(auto &&it, auto end, JsonValue::value_t &value)
{
    if (is_match(it, end, "null"))
    {
        value = JsonValue::null_t{};
        return true;
    }
    return false;
}

bool is_false(auto &&it, auto end, JsonValue::value_t &value)
{
    if (is_match(it, end, "false"))
    {
        value = JsonValue::false_t{};
        return true;
    }
    return false;
}

bool is_true(auto &&it, auto end, JsonValue::value_t &value)
{
    if (is_match(it, end, "true"))
    {
        value = JsonValue::true_t{};
        return true;
    }
    return false;
}

bool is_sign(auto &&it, auto end)
{
    if (it != end)
    {
        switch (*it)
        {
        case '-':
            ++it;
            break;
        case '+':
            ++it;
            break;
        default:
            break;
        }
    }
    return true;
}

bool is_digits(auto &&it, auto end);

bool is_exponent(auto &&it, auto end)
{
    auto jt = it;
    if (jt != end && (*jt == 'e' || *jt == 'E'))
    {
        if (!is_sign(++jt, end) || !is_digits(jt, end))
            ; // TODO parse error
        it = jt;
    }
    return true;
}

bool is_fraction(auto &&it, auto end)
{
    auto jt = it;
    if (jt != end && *jt++ == '.')
    {
        if (!is_digits(jt, end))
            ; // TODO parse error
        it = jt;
    }
    return true;
}

bool is_onenine(auto &&it, auto end)
{
    return is_match(it, end, [](auto value) { return value >= '1' && value <= '9'; });
}

bool is_digit(auto &&it, auto end)
{
    return is_match(it, end, '0') || is_onenine(it, end);
}

bool is_digits(auto &&it, auto end)
{
    if (is_digit(it, end))
    {
        while (is_digit(it, end))
            ;
        return true;
    }
    return false;
}

bool is_integer(auto &&it, auto end)
{
    return is_match(it, end, '-') && ((is_onenine(it, end) && (is_digits(it, end) || true)) || is_digit(it, end)) ||
           ((is_onenine(it, end) && (is_digits(it, end) || true)) || is_digit(it, end));
}

bool is_number(auto &&it, auto end, JsonValue::value_t &value)
{
    auto jt = it;
    if (is_integer(jt, end) && is_fraction(jt, end) && is_exponent(jt, end))
    {
        value = JsonValue::number_t{it, jt};
        it = jt;
        return true;
    }
    return false;
}

bool is_hex(auto &&it, auto end)
{
    if (it != end && ((*it >= '0' && *it <= '9') || (*it >= 'A' && *it <= 'F') || (*it >= 'a' && *it <= 'f')))
    {
        ++it;
        return true;
    }
    return false;
}

bool is_escape(auto &&it, auto end)
{
    if (it == end)
        return false;
    switch (*it)
    {
    case '"':
    case '\\':
    case '/':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
        return true;
    case 'u':
        return true;
    default:
        return false;
    }
}

bool is_character(auto &&it, auto end)
{
    if (it != end && *it >= 0x0020 && *it <= 0x10FFFF)
    {
        if (*it == '"')
            return false;
        if (*it == '\\' && !is_escape(++it, end))
            throw std::runtime_error("expected escape sequence");
        else
            ++it;
        return true;
    }
    return false;
}

bool is_characters(auto &&it, auto end)
{
    while (is_character(it, end))
        ;
    return true;
}

bool is_string(auto &&it, auto end, JsonValue::value_t &value)
{

    if (is_match(it, end, '"'))
    {
        auto jt = it;
        if (!is_characters(jt, end))
            throw std::runtime_error("expected characters");
        value = JsonValue::string_t{it, jt};

        if (!is_match(jt, end, '"'))
            throw std::runtime_error("unclosed string");
        it = jt;
        return true;
    }
    return false;
}

bool is_element(auto &&it, auto end, JsonValue::value_t &value)
{
    return is_ws(it, end) && is_value(it, end, value) && is_ws(it, end);
}

bool is_elements(auto &&it, auto end, std::vector<JsonValue::value_t> &elements)
{
    for (JsonValue::value_t element; is_element(it, end, element);)
    {
        elements.push_back(std::move(element));
        if (!is_match(it, end, ','))
            break;
    }
    return true;
}

bool is_array(auto &&it, auto end, JsonValue::value_t &value)
{
    if (is_match(it, end, '['))
    {
        auto elements = std::vector<JsonValue::value_t>{};
        if (!is_elements(it, end, elements))
            throw std::runtime_error("Expected array elements");
        if (!is_match(it, end, ']'))
            throw std::runtime_error("Expected ']'");
        value = JsonValue::array_t{std::move(elements)};
        return true;
    }
    return false;
}

bool is_member(auto &&it, auto end, auto &members)
{
    auto key = JsonValue::value_t{}, value = JsonValue::value_t{};
    if (is_ws(it, end) && is_string(it, end, key) && is_ws(it, end))
    {
        if (!is_match(it, end, ':'))
            throw std::runtime_error("Expected ':'");
        if (!is_element(it, end, value))
            throw std::runtime_error("Expected <element>");
        members[std::get<JsonValue::string_t>(key)] = value;
        return true;
    }
    return false;
}

bool is_members(auto &&it, auto end, auto &members)
{
    while (is_member(it, end, members))
        if (!is_match(it, end, ','))
            break;
    return true;
}

bool is_object(auto &it, auto end, JsonValue::value_t &value)
{
    if (is_match(it, end, '{'))
    {
        JsonValue::object_t object;
        // std::map<JsonValue::string_t, JsonValue::value_t> members;
        if (!is_members(it, end, object.members))
            throw std::runtime_error("Expected object members");
        if (!is_match(it, end, '}'))
            throw std::runtime_error("Expected object");
        value = object;
        return true;
    }
    return false;
}

bool is_value(auto &&it, auto end, JsonValue::value_t &value)
{
    return is_object(it, end, value) || is_array(it, end, value) || is_string(it, end, value) ||
           is_number(it, end, value) || is_true(it, end, value) || is_false(it, end, value) || is_null(it, end, value);
}

JsonValue parse_json(std::string_view source)
{
    auto value = JsonValue::value_t{};
    if (is_element(source.begin(), source.end(), value))
        ;
    return JsonValue{value};
}

#include "json.hpp"

#include <stdexcept>

struct JsonParser
{

    bool is_match(auto &&it, auto end, auto expected)
    {
        if (it != end && *it == expected)
        {
            std::advance(it, 1);
            return true;
        }
        return false;
    }

    bool is_ws(auto &&it, auto end)
    {
        while (it != end && (*it == 0x0020 || *it == 0x000A || *it == 0x000D || *it == 0x0009))
            ++it;
        return true;
    }

    bool is_member(auto &&it, auto end, JsonValue::String &key, JsonValue &value)
    {
        auto jt = it;
        JsonValue string;
        if (!is_ws(jt, end) || is_string(jt, end, string))
            return false;
        key = string.as_string();
        if (!is_ws(jt, end) || !is_match(jt, end, ':') || !is_element(jt, end, value))
            throw SyntaxError(it, end, "Expected <member>");
        it = jt;
        return true;
    }

    bool is_characters(auto &&it, auto end)
    {
        return false;
    }

    bool is_integer(auto &&it, auto end)
    {
        return false;
    }

    bool is_fraction(auto &&it, auto end)
    {
        return false;
    }

    bool is_exponent(auto &&it, auto end)
    {
        return false;
    }

    bool is_object(auto &&it, auto end, JsonValue &result)
    {
        auto jt = it;
        if (!is_match(jt, end, '{'))
            return false;

        auto members = std::map<JsonValue::String, JsonValue>{};

        JsonValue::String key;
        JsonValue value;
        while (is_member(jt, end, key, value))
        {
            // members[std::move(key)] = std::move(value);
            if (!is_match(jt, end, ','))
                break;
        }

        if (!is_match(jt, end, '}'))
            throw SyntaxError(it, end, "Expected <object>");

        result = {JsonValue::Object{std::move(members)}, it, jt};
        return true;
    }

    bool is_array(auto &&it, auto end, JsonValue &result)
    {
        auto jt = it;
        if (!is_match(jt, end, '['))
            return false;

        auto elements = std::vector<JsonValue>{};

        for (JsonValue element; is_element(jt, end, element) && is_match(jt, end, ',');)
            elements.emplace_back(std::move(element));

        if (!is_match(jt, end, ']'))
            throw SyntaxError(it, end, "Expected <array>");

        result = {JsonValue::Array{std::move(elements)}, it, jt};
        return true;
    }

    bool is_string(auto &&it, auto end, JsonValue &result)
    {
        auto jt = it;
        if (!is_match(jt, end, '"'))
            return false;
        if (!is_characters(jt, end))
            throw SyntaxError(it, end, "Expected <string>");
        if (!is_match(jt, end, '"'))
            throw SyntaxError(it, end, "Expected <string>");
        result = {JsonValue::String{}, it, jt};
        return true;
    }

    bool is_number(auto &&it, auto end, JsonValue &result)
    {
        auto jt = it;
        if (!is_integer(jt, end))
            return false;
        if (!is_fraction(jt, end))
            throw SyntaxError(it, end, "Expected <fraction>");
        if (!is_exponent(jt, end))
            throw SyntaxError(it, end, "Expected <exponent>");
        result = {JsonValue::Number{}, it, jt};
        return true;
    }

    bool is_true(auto &&it, auto end, JsonValue &result)
    {
        auto jt = it;
        if (!is_match(jt, end, 't'))
            return false;
        if (!is_match(jt, end, 'r'))
            throw SyntaxError(it, end, "Expected 'true'");
        if (!is_match(jt, end, 'u'))
            throw SyntaxError(it, end, "Expected 'true'");
        if (!is_match(jt, end, 'e'))
            throw SyntaxError(it, end, "Expected 'true'");
        result = {JsonValue::True{}, it, jt};
        return true;
    }

    bool is_false(auto &&it, auto end, JsonValue &result)
    {
        auto jt = it;
        if (!is_match(jt, end, 'f'))
            return false;
        if (!is_match(jt, end, 'a'))
            throw SyntaxError(it, end, "Expected 'false'");
        if (!is_match(jt, end, 'l'))
            throw SyntaxError(it, end, "Expected 'false'");
        if (!is_match(jt, end, 's'))
            throw SyntaxError(it, end, "Expected 'false'");
        if (!is_match(jt, end, 'e'))
            throw SyntaxError(it, end, "Expected 'false'");
        result = {JsonValue::False{}, it, jt};
        return true;
    }

    bool is_null(auto &&it, auto end, JsonValue &result)
    {
        auto jt = it;
        if (!is_match(jt, end, 'n'))
            return false;
        if (!is_match(jt, end, 'u'))
            throw SyntaxError(it, end, "Expected 'null'");
        if (!is_match(jt, end, 'l'))
            throw SyntaxError(it, end, "Expected 'null'");
        if (!is_match(jt, end, 'l'))
            throw SyntaxError(it, end, "Expected 'null'");
        result = {JsonValue::Null{}, it, jt};
        return true;
    }

    bool is_value(auto &&it, auto end, JsonValue &result)
    {
        return is_null(it, end, result) || is_false(it, end, result) || is_true(it, end, result) ||
               is_number(it, end, result) || is_string(it, end, result) || is_array(it, end, result) ||
               is_object(it, end, result);
    }

    bool is_element(auto &&it, auto end, JsonValue &result)
    {
        return is_ws(it, end) && is_value(it, end, result) && is_ws(it, end);
    }
};

JsonValue parse_json(const std::string_view &json)
{
    JsonParser parser;
    JsonValue result;
    if (!parser.is_element(json.cbegin(), json.cend(), result))
        throw std::runtime_error("Unable to parse json");
    return result;
}

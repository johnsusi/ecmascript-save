#ifndef __ECMASCRIPT_JSON_JSON_HPP
#define __ECMASCRIPT_JSON_JSON_HPP

#include <charconv>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

class SyntaxError : std::runtime_error
{
  public:
    SyntaxError(auto it, auto end, const char *message) : runtime_error(message)
    {
    }
};

struct JsonValue
{

    using Iterator = std::string_view::const_iterator;
    Iterator start, end;

    struct Null
    {
    };

    struct False
    {
    };

    struct True
    {
    };

    struct Number
    {
        std::optional<double> value;
    };

    struct String
    {
        std::optional<std::string> value;
    };

    struct Array
    {
        std::vector<JsonValue> value;
    };

    struct Member
    {
        String key;
        String value;
    };

    struct Object
    {
        std::map<String, JsonValue> value;
    };

    using Type = std::variant<std::monostate, Null, True, False, Number, String, Array, Object>;

    Type type;

    JsonValue()
    {
    }

    JsonValue(Type type, Iterator start, Iterator end) : start(start), end(end), type(type)
    {
    }

    operator double() const
    {
        return 0.0;
    }

    bool is_number() const
    {
        return std::holds_alternative<Number>(type);
    }

    bool is_true() const
    {
        return std::holds_alternative<True>(type);
    }

    bool is_false() const
    {
        return std::holds_alternative<False>(type);
    }

    bool is_null() const
    {
        return std::holds_alternative<Null>(type);
    }

    String as_string() const
    {
        return std::get<String>(type);
    }
};

//! parse_json tries to parse json
/*!
    \param json a string of json. Note that the lifetime of the string_view must
        be longer or equal to the returned value.

    \return A JsonValue representing the parsed json. This structure have
        pointers to the input string_view!
*/
JsonValue parse_json(const std::string_view &json);

#endif
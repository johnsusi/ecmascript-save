#ifndef __ECMASCRIPT_PARSER_EXPRESSION_HPP
#define __ECMASCRIPT_PARSER_EXPRESSION_HPP

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ecmascript
{

struct VariantEqualsOperator
{
    template <typename T> bool operator()(const T &lhs, const T &rhs) const
    {
        return lhs == rhs;
    }

    template <typename T1, typename T2> bool operator()(const T1 &, const T2 &) const
    {
        return false;
    }
};

template <typename T> struct RecursiveWrapper
{
    std::unique_ptr<T> data;
    RecursiveWrapper() = default;
    RecursiveWrapper(auto &&...args) : data(std::make_unique<T>(std::forward<decltype(args)>(args)...))
    {
    }
    RecursiveWrapper(RecursiveWrapper &&) = default;
    RecursiveWrapper(const RecursiveWrapper &other) : data(std::make_unique<T>(*other.data.get()))
    {
    }
    RecursiveWrapper &operator=(RecursiveWrapper &&) = default;
    RecursiveWrapper &operator=(const RecursiveWrapper &other)
    {
        data.reset(new T{*other.data.get()});
        return *this;
    }

    operator T &()
    {
        return *data.get();
    }
    operator const T &() const
    {
        return *data.get();
    }

    bool operator==(const RecursiveWrapper &other) const
    {
        if (!data)
            return !other.data;
        if (!other.data)
            return false;
        return std::visit(VariantEqualsOperator{}, *data.get(), *other.data.get());
    }
};

struct ThisExpression;
struct IdentifierExpression;
struct LiteralExpression;
struct ArrayExpression;
struct ObjectExpression;
struct GroupingExpression;

struct PrimaryExpression;

struct MemberExpression;
struct CallExpression;
struct NewExpression;

struct PostfixExpression;
struct UnaryExpression;
struct MultiplicativeExpression;
struct AdditiveExpression;
struct ShiftExpression;
struct RelationalExpression;
struct EqualityExpression;
struct BitwiseANDExpression;
struct BitwiseXORExpression;
struct BitwiseORExpression;
struct LogicalANDExpression;
struct LogicalORExpression;
struct ConditionalExpression;
struct AssignmentExpression;

using Empty = std::monostate;

using Expression =
    std::variant<Empty, ThisExpression, IdentifierExpression, LiteralExpression, ArrayExpression, ObjectExpression,
                 GroupingExpression, MemberExpression, NewExpression, CallExpression, PostfixExpression,
                 UnaryExpression, MultiplicativeExpression, AdditiveExpression, ShiftExpression, RelationalExpression,
                 EqualityExpression, BitwiseANDExpression, BitwiseXORExpression, BitwiseORExpression,
                 LogicalANDExpression, LogicalORExpression, ConditionalExpression, AssignmentExpression>;

using ExpressionRef = RecursiveWrapper<Expression>;
struct ThisExpression
{
    bool operator==(const ThisExpression &other) const;
};

struct IdentifierExpression
{
    std::string identifier;
    bool operator==(const IdentifierExpression &other) const;
};
struct NullLiteral
{
    bool operator==(const NullLiteral &other) const
    {
        return true;
    }
};
struct BooleanLiteral
{
    bool value;
    bool operator==(const BooleanLiteral &other) const
    {
        return value == other.value;
    }
};
struct NumericLiteral
{
    double value;
    bool operator==(const NumericLiteral &other) const
    {
        return value == other.value;
    }
};
struct StringLiteral
{
    std::string value;
    bool operator==(const StringLiteral &other) const
    {
        return value == other.value;
    }
};
struct RegularExpressionLiteral
{
};

struct LiteralExpression
{
    using Literal =
        std::variant<NullLiteral, BooleanLiteral, NumericLiteral, StringLiteral /*, RegularExpressionLiteral*/>;

    Literal literal;

    bool operator==(const LiteralExpression &other) const;
};

struct ArrayExpression
{
    std::vector<Expression> elements;
    bool operator==(const ArrayExpression &other) const;
};

struct PropertyAssignment
{
    bool operator==(const PropertyAssignment &other) const;
};

struct ObjectExpression
{
    std::vector<PropertyAssignment> propertyAssignments;
    bool operator==(const ObjectExpression &other) const;
};

struct GroupingExpression
{
    std::vector<Expression> expressions;
    bool operator==(const GroupingExpression &other) const;
};

using Arguments = std::vector<Expression>;

struct MemberExpression
{
    ExpressionRef object, property;
    bool operator==(const MemberExpression &other) const;
};

struct NewExpression
{
    ExpressionRef expression;
    Arguments arguments;
    bool operator==(const NewExpression &other) const;
};

struct CallExpression
{
    ExpressionRef expression;
    Arguments arguments;
    bool operator==(const CallExpression &other) const;
};

struct PostfixExpression
{
    std::string op;
    ExpressionRef lhs;
    bool operator==(const PostfixExpression &other) const;
};

struct UnaryExpression
{
    std::string op;
    ExpressionRef rhs;
    bool operator==(const UnaryExpression &other) const;
};

struct BinaryExpression
{
    std::string op;
    ExpressionRef lhs, rhs;
    bool operator==(const BinaryExpression &other) const;
};

struct MultiplicativeExpression : BinaryExpression
{
    bool operator==(const MultiplicativeExpression &other) const;
};

struct AdditiveExpression : BinaryExpression
{
    bool operator==(const AdditiveExpression &other) const;
};

struct ShiftExpression : BinaryExpression
{
    bool operator==(const ShiftExpression &other) const;
};

struct RelationalExpression : BinaryExpression
{
    bool operator==(const RelationalExpression &other) const;
};

struct EqualityExpression : BinaryExpression
{
    bool operator==(const EqualityExpression &other) const;
};

struct BitwiseANDExpression : BinaryExpression
{
    bool operator==(const BitwiseANDExpression &other) const;
};

struct BitwiseXORExpression : BinaryExpression
{
    bool operator==(const BitwiseXORExpression &other) const;
};

struct BitwiseORExpression : BinaryExpression
{
    bool operator==(const BitwiseORExpression &other) const;
};

struct LogicalANDExpression : BinaryExpression
{
    bool operator==(const LogicalANDExpression &other) const;
};

struct LogicalORExpression : BinaryExpression
{
    bool operator==(const LogicalORExpression &other) const;
};

struct ConditionalExpression : BinaryExpression
{
    bool operator==(const ConditionalExpression &other) const;
};

struct AssignmentExpression : BinaryExpression
{
    bool operator==(const AssignmentExpression &other) const;
};

inline bool ThisExpression::operator==(const ThisExpression &other) const = default;
inline bool IdentifierExpression::operator==(const IdentifierExpression &other) const = default;
inline bool LiteralExpression::operator==(const LiteralExpression &other) const = default;
inline bool ArrayExpression::operator==(const ArrayExpression &other) const = default;
inline bool PropertyAssignment::operator==(const PropertyAssignment &other) const = default;
inline bool ObjectExpression::operator==(const ObjectExpression &other) const = default;
inline bool GroupingExpression::operator==(const GroupingExpression &other) const = default;
inline bool MemberExpression::operator==(const MemberExpression &other) const = default;
inline bool NewExpression::operator==(const NewExpression &other) const = default;
inline bool CallExpression::operator==(const CallExpression &other) const = default;
inline bool PostfixExpression::operator==(const PostfixExpression &other) const = default;
inline bool UnaryExpression::operator==(const UnaryExpression &other) const = default;
inline bool BinaryExpression::operator==(const BinaryExpression &other) const = default;
inline bool MultiplicativeExpression::operator==(const MultiplicativeExpression &other) const = default;
inline bool AdditiveExpression::operator==(const AdditiveExpression &other) const = default;
inline bool ShiftExpression::operator==(const ShiftExpression &other) const = default;
inline bool RelationalExpression::operator==(const RelationalExpression &other) const = default;
inline bool EqualityExpression::operator==(const EqualityExpression &other) const = default;
inline bool BitwiseANDExpression::operator==(const BitwiseANDExpression &other) const = default;
inline bool BitwiseXORExpression::operator==(const BitwiseXORExpression &other) const = default;
inline bool BitwiseORExpression::operator==(const BitwiseORExpression &other) const = default;
inline bool LogicalANDExpression::operator==(const LogicalANDExpression &other) const = default;
inline bool LogicalORExpression::operator==(const LogicalORExpression &other) const = default;
inline bool ConditionalExpression::operator==(const ConditionalExpression &other) const = default;
inline bool AssignmentExpression::operator==(const AssignmentExpression &other) const = default;

} // namespace ecmascript

template <typename T, typename... Types> inline bool operator==(const T &t, const std::variant<Types...> &v)
{
    const T *c = std::get_if<T>(&v);
    if (c)
        return *c == t;
    else
        return false;
}

template <typename T, typename... Types> inline bool operator==(const std::variant<Types...> &v, const T &t)
{
    return t == v;
}

#endif
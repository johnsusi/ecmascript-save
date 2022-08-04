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

using Expression = std::variant<std::monostate, ThisExpression, IdentifierExpression, LiteralExpression,
                                ArrayExpression, ObjectExpression, GroupingExpression, MemberExpression, NewExpression,
                                CallExpression, PostfixExpression, UnaryExpression, MultiplicativeExpression,
                                AdditiveExpression, ShiftExpression, RelationalExpression, EqualityExpression,
                                BitwiseANDExpression, BitwiseXORExpression, BitwiseORExpression, LogicalANDExpression,
                                LogicalORExpression, ConditionalExpression, AssignmentExpression>;

using ExpressionRef = RecursiveWrapper<Expression>;
struct ThisExpression
{
    bool operator==(const ThisExpression &other) const = default;
};

struct IdentifierExpression
{
    bool operator==(const IdentifierExpression &other) const = default;
};

struct LiteralExpression
{
    bool operator==(const LiteralExpression &other) const = default;
};

struct ArrayExpression
{
    std::vector<Expression> elements;
    bool operator==(const ArrayExpression &other) const = default;
};

struct PropertyAssignment
{
    bool operator==(const PropertyAssignment &other) const = default;
};

struct ObjectExpression
{
    std::vector<PropertyAssignment> propertyAssignments;
    bool operator==(const ObjectExpression &other) const = default;
};

struct GroupingExpression
{
    std::vector<Expression> expressions;
    bool operator==(const GroupingExpression &other) const = default;
};

using Arguments = std::vector<ExpressionRef>;

struct MemberExpression
{
    ExpressionRef object, property;
    bool operator==(const MemberExpression &other) const = default;
};

struct NewExpression
{
    ExpressionRef expression;
    Arguments arguments;
    bool operator==(const NewExpression &other) const = default;
};

struct CallExpression
{
    ExpressionRef expression;
    Arguments arguments;
    bool operator==(const CallExpression &other) const = default;
};

struct PostfixExpression
{
    std::string op;
    ExpressionRef lhs;
    bool operator==(const PostfixExpression &other) const = default;
};

struct UnaryExpression
{
    std::string op;
    ExpressionRef rhs;
    bool operator==(const UnaryExpression &other) const = default;
};

struct BinaryExpression
{
    std::string op;
    ExpressionRef lhs, rhs;
    bool operator==(const BinaryExpression &other) const = default;
};

struct MultiplicativeExpression : BinaryExpression
{
    bool operator==(const MultiplicativeExpression &other) const = default;
};

struct AdditiveExpression : BinaryExpression
{
    bool operator==(const AdditiveExpression &other) const = default;
};

struct ShiftExpression : BinaryExpression
{
    bool operator==(const ShiftExpression &other) const = default;
};

struct RelationalExpression : BinaryExpression
{
    bool operator==(const RelationalExpression &other) const = default;
};

struct EqualityExpression : BinaryExpression
{
    bool operator==(const EqualityExpression &other) const = default;
};

struct BitwiseANDExpression : BinaryExpression
{
    bool operator==(const BitwiseANDExpression &other) const = default;
};

struct BitwiseXORExpression : BinaryExpression
{
    bool operator==(const BitwiseXORExpression &other) const = default;
};

struct BitwiseORExpression : BinaryExpression
{
    bool operator==(const BitwiseORExpression &other) const = default;
};

struct LogicalANDExpression : BinaryExpression
{
    bool operator==(const LogicalANDExpression &other) const = default;
};

struct LogicalORExpression : BinaryExpression
{
    bool operator==(const LogicalORExpression &other) const = default;
};

struct ConditionalExpression : BinaryExpression
{
    bool operator==(const ConditionalExpression &other) const = default;
};

struct AssignmentExpression : BinaryExpression
{
    bool operator==(const AssignmentExpression &other) const = default;
};

} // namespace ecmascript

#endif
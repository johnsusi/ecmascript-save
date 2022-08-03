#ifndef __ECMASCRIPT_PARSER_EXPRESSION_HPP
#define __ECMASCRIPT_PARSER_EXPRESSION_HPP

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ecmascript
{

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

struct ExpressionRef
{
    std::unique_ptr<Expression> data;
    ExpressionRef &operator=(Expression &&other);
    operator Expression &();
    bool operator==(const ExpressionRef &other) const;
};

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
    bool operator==(const ArrayExpression &other) const;
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
    bool operator==(const GroupingExpression &other) const;
};

using Arguments = std::vector<Expression>;

struct MemberExpression
{
    ExpressionRef object, property;
    bool operator==(const MemberExpression &other) const = default;
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

bool operator==(const ecmascript::Expression &lhs, const ecmascript::Expression &rhs);

#endif
#include "expression.hpp"

using namespace ecmascript;

ExpressionRef::ExpressionRef() = default;
ExpressionRef::ExpressionRef(ExpressionRef &&other) = default;

ExpressionRef::ExpressionRef(Expression &&other) : data(new Expression{std::move(other)})
{
}

ExpressionRef &ExpressionRef::operator=(ExpressionRef &&other)
{
    std::swap(data, other.data);
    return *this;
}

ExpressionRef &ExpressionRef::operator=(Expression &&other)
{
    data.reset(new Expression{std::move(other)});
    return *this;
}

ExpressionRef::operator Expression &()
{
    return *data.get();
}

bool ExpressionRef::operator==(const ExpressionRef &other) const
{
    if (!data)
        return !other.data;
    if (!other.data)
        return false;
    return *data.get() == *other.data.get();
}

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

bool operator==(const Expression &lhs, const Expression &rhs)
{
    return std::visit(VariantEqualsOperator{}, lhs, rhs);
}

bool ArrayExpression::operator==(const ArrayExpression &other) const = default;
bool GroupingExpression::operator==(const GroupingExpression &other) const = default;
bool NewExpression::operator==(const NewExpression &other) const = default;
bool CallExpression::operator==(const CallExpression &other) const = default;

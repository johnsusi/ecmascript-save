#include "statement.hpp"

using namespace ecmascript;

StatementRef::StatementRef(Statement &&replacement) : data(new Statement(std::move(replacement)))
{
}

StatementRef &StatementRef::operator=(Statement &&replacement)
{
    data.reset(new Statement(std::move(replacement)));
    return *this;
}

StatementRef::operator Statement &()
{
    return *data.get();
}

bool StatementRef::operator==(const StatementRef &other) const
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

bool operator==(const Statement &lhs, const Statement &rhs)
{
    return std::visit(VariantEqualsOperator{}, lhs, rhs);
}

bool BlockStatement::operator==(const BlockStatement &other) const = default;
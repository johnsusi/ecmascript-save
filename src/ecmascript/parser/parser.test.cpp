// #include "parser.cpp"

// #include <catch2/catch_test_macros.hpp>

// #include <iostream>

// #include <variant>
// #include <vector>

// struct EmptyStatement
// {
// };

// struct IfStatement
// {
// };
// using Statement = std::variant<EmptyStatement, IfStatement>;

// struct FunctionDeclaration
// {
// };
// using SourceElement = std::variant<Statement, FunctionDeclaration>;

// struct Program
// {
//     std::vector<SourceElement> sourceElements;
// };

// struct EqualsVisitor
// {

//     bool operator()(const Statement &lhs, const Statement &rhs)
//     {
//         std::cout << "statement\n";
//         return std::visit(*this, lhs, rhs);
//     }

//     template <typename T> bool operator()(T &lhs, T &rhs)
//     {
//         std::cout << typeid(T).name() << std::endl;
//         return true;
//     }

//     template <typename T1, typename T2> bool operator()(T1 &lhs, T2 &rhs)
//     {
//         std::cout << typeid(T1).name() << ", " << typeid(T2).name() << std::endl;
//         return false;
//     }
// };

// bool operator==(const Program &lhs, const Program &rhs)
// {
//     if (lhs.sourceElements.size() != rhs.sourceElements.size())
//         return false;
//     for (int i = 0; i < lhs.sourceElements.size(); ++i)
//     {
//         if (!std::visit(EqualsVisitor{}, lhs.sourceElements[i], rhs.sourceElements[i]))
//             return false;
//     }
//     return true;
// }

// class Parser
// {
//   public:
//     Parser(Lexer lexer)
//     {
//     }

//     Program parse()
//     {
//         return {{SourceElement{EmptyStatement{}}}};
//     }
// };

// TEST_CASE("Empty statement", "[parser]")
// {
//     auto lexer = Lexer{Token{";"}};
//     auto parser = Parser{lexer};
//     auto actual = parser.parse();

//     auto expected = Program{{SourceElement{EmptyStatement{}}}};
//     REQUIRE(actual == expected);
// }

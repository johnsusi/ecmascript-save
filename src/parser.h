#ifndef ECMASCRIPT_PARSER_H
#define ECMASCRIPT_PARSER_H

#include "ast.h"
#include "matcher.h"
#include "token.h"
// #include "builder.h"

#include <iostream>
#include <memory>
#include <vector>

// 11.3
bool is_assignment_operator(const std::string&);

class BasicParser
{
protected:
  Matcher<Token, std::vector<Token>::iterator> match;

public:

  template <typename It>
  BasicParser(It begin, It end) : match(begin, end) {}

  bool no_line_terminator_here()
  {
    return match.peek([](const Token& token) { return !token.preceded_by_line_terminator; });
  }

  // 7.9
  bool automatic_semicolon_insertion()
  {
    if (match(";")) return true;
    if (no_line_terminator_here() && !match.peek("}")) return false; // 7.9.1
    return true;
  }


  bool syntax_error(std::string what = {})
  {
    std::cout << *match.matched() << std::endl;
    throw std::runtime_error("SyntaxError: " + what);
  }
};

class ExpressionBuilder
{
  std::vector<Expression> stack;
public:
  void push(Expression&& value)
  {
    stack.push_back(value);
  }

  template <typename T, typename... Args>
  void push(Args&&... args)
  {
    stack.push_back(T { std::forward<Args>(args)... });
  }

  Expression pop()
  {
    auto expr = stack.back();
    stack.pop_back();
    return expr;
  }

};

class ExpressionParser : public BasicParser, private ExpressionBuilder
{
public:

  template <typename... Args>
  ExpressionParser(Args&&... args) : BasicParser(std::forward<Args>(args)...) {}

  boost::optional<Identifier> identifier()
  {
    if (!match([](const auto& token) { return token.is_identifier(); })) return {};
    return Identifier { match };
  }

  // A.1
  bool identifier_name()
  {
    return match([](const auto& token) { return token.is_identifier(); });
  }

  bool literal()
  {
         if (auto literal = null_literal())    push<NullLiteral>();
    else if (auto literal = boolean_literal()) push<BooleanLiteral>(match);
    else if (auto literal = numeric_literal()) push<NumericLiteral>(match);
    else if (auto literal = string_literal())  push<StringLiteral>(match);
    // else if (regular_expression_literal())
    else return false;
    return true;
  }

  bool null_literal()
  {
    return match([](const auto& token) { return token.is_null_literal(); });
  }

  bool boolean_literal()
  {
    return match([](const auto& token) { return token.is_boolean_literal(); });
  }

  bool numeric_literal()
  {
    return match([](const auto& token) { return token.is_numeric_literal(); });
  }

  bool string_literal()
  {
    return match([](const auto& token) { return token.is_string_literal(); });
  }

  bool regular_expression_literal()
  {
    return match([](const auto& token) { return token.is_regular_expression_literal(); });
  }

  // A.3
  bool primary_expression()
  {
    if (match("(")) {
      if (!expression()) syntax_error();
      if (!match(")")) syntax_error();
      return true;
    }

    if (match("this")) {
      push<This>();
      return true;
    }
    if (auto id = identifier()) {
      push(*id);
      return true;
    }
    return literal() || array_literal() || object_literal();
  }

  bool array_literal()
  {
    if (match("[")) {
      if (element_list()) {
        if (match(",")) elision_opt();
      }
      else elision_opt();
      if (!match("]")) syntax_error();
      push<ArrayLiteral>();
      return true;
    }
    return false;
  }

  bool element_list()
  {
    if (elision_opt() && assignment_expression()) {
      if (match(",")) element_list();
      return true;
    }
    return false;
  }

  bool elision_opt()
  {
    return elision(), true;
  }

  bool elision()
  {
    if (match(",")) {
      push<Identifier>(u"undefined");
      while (match(",")) push<Identifier>(u"undefined");
      return true;
    }
    return false;
  }

  bool object_literal()
  {
    if (match("{")) {
      property_name_and_value_list();
      match(",");
      if (!match("}")) syntax_error();
      push<ObjectLiteral>();
      return true;
    }
    return false;
  }

  bool property_name_and_value_list()
  {
    if (property_assignment()) {
      while (match(",") && (property_assignment() || syntax_error()));
      return true;
    }
    return false;
  }

  bool function_body()
  {
    return false;
  }

  bool property_assignment()
  {
    if (match("get")) {
      return (property_name() && match("(") && match(")") && match("{") &&
        function_body() && match("}")) || syntax_error();

    }
    else if (match("set")) {
      return (property_name() && match("(") && property_set_parameter_list() &&
        match(")") && match("{") && function_body() && match("}")) ||
        syntax_error();
    }
    else if (property_name()) {
      return (match(":") && assignment_expression()) || syntax_error();
    }
    else return false;
  }

  bool property_name()
  {
    return identifier_name() || string_literal() || numeric_literal();
  }

  bool property_set_parameter_list()
  {
    if (identifier()) return true;
    return false;
  }

  bool function_expression()
  {
    return false;
  }
  bool member_expression()
  {
    if (primary_expression() || function_expression() || match([this]{
      return match("new") && member_expression() && arguments();
    })) {
      while (true) {
        if (match("[")) {
          if (expression() && match("]")) continue;
          else syntax_error();
        }
        if (match(".")) {
          if (identifier_name()) continue;
          else syntax_error();
        }
        return true;
      }
    }
    return false;
  }

  bool new_expression()
  {
    return member_expression() || (match("new") && new_expression());
  }

  bool call_expression()
  {
    if (member_expression()) {
      arguments() || syntax_error();
      while (true) {
        if (match("[")) {
          (expression() && match("]")) || syntax_error();
          continue;
        }
        if (match(".")) {
          identifier_name() || syntax_error();
          continue;
        }
        if (arguments()) continue;
        return true;
      }
    }
    return false;
  }

  bool arguments()
  {
    if (match("(")) {
      return argument_list(), match(")") || syntax_error();
    }
    return false;
  }

  bool argument_list()
  {
    if (assignment_expression()) {
      while (match(",") && (assignment_expression() || syntax_error()));
      return true;
    }
    return false;
  }

  bool left_hand_side_expression()
  {
    return new_expression() || call_expression();
  }

  bool postfix_expression()
  {
    if (left_hand_side_expression()) {
      if (no_line_terminator_here()) {
        if (match("++") || match("--")) push<PostfixExpression>(match, pop());
      }
      return true;
    }
    return false;
  }

  bool unary_expression()
  {
    if (match("delete") || match("void") || match("typeof") || match("++") ||
        match("--") || match("+") || match("-") || match("~") || match("!"))
    {
      std::string op = match;
      if (!unary_expression()) syntax_error();
      push<UnaryExpression>(op, pop());
      return true;
    }
    return postfix_expression();
  }

  bool multiplicative_expression()
  {
    if (unary_expression()) {
      while (match("*") || match("/") || match("%"))
      {
        auto lhs = pop();
        std::string op = match;
        if (!unary_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool additive_expression()
  {
    if (multiplicative_expression()) {
      while (match("+") || match("-"))
      {
        auto lhs = pop();
        std::string op = match;
        if (!multiplicative_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool shift_expression()
  {
    if (additive_expression()) {
      while (match("<<") || match(">>") || match(">>>"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!additive_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool relational_expression()
  {
    if (shift_expression()) {
      while (match("<") || match(">") || match("<=") || match(">=") ||
        match("instanceof") || match("in"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!shift_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool relational_expression_no_in()
  {
    if (shift_expression()) {
      while (match("<") || match(">") || match("<=") || match(">=") ||
        match("instanceof"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!shift_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool equality_expression()
  {
    if (relational_expression()) {
      while (match("==") || match("!=") || match("===") || match("!=="))
      {
        auto lhs = pop();
        std::string op = match;
        if (!relational_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool equality_expression_no_in()
  {
    if (relational_expression()) {
      while (match("==") || match("!=") || match("===") || match("!=="))
      {
        std::string op = match;
        auto lhs = pop();
        if (!relational_expression_no_in()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool bitwise_and_expression()
  {
    if (equality_expression()) {
      while (match("&"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!equality_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool bitwise_and_expression_no_in()
  {
    if (equality_expression_no_in()) {
      while (match("&"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!equality_expression_no_in()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool bitwise_xor_expression()
  {
    if (bitwise_and_expression()) {
      while (match("^"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!bitwise_and_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool bitwise_xor_expression_no_in()
  {
    if (bitwise_and_expression_no_in()) {
      while (match("^"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!bitwise_and_expression_no_in()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool bitwise_or_expression()
  {
    if (bitwise_xor_expression()) {
      while (match("|"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!bitwise_xor_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool bitwise_or_expression_no_in()
  {
    if (bitwise_xor_expression_no_in()) {
      while (match("|"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!bitwise_xor_expression_no_in()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool logical_and_expression()
  {
    if (bitwise_or_expression()) {
      while (match("&&"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!bitwise_or_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool logical_and_expression_no_in()
  {
    if (bitwise_or_expression_no_in()) {
      while (match("&&"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!bitwise_or_expression_no_in()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool logical_or_expression()
  {
    if (logical_and_expression()) {
      while (match("||"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!logical_and_expression()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool logical_or_expression_no_in()
  {
    if (logical_and_expression_no_in()) {
      while (match("||"))
      {
        std::string op = match;
        auto lhs = pop();
        if (!logical_and_expression_no_in()) syntax_error();
        push<BinaryExpression>(op, lhs, pop());
      }
      return true;
    }
    return false;
  }

  bool conditional_expression()
  {
    if (logical_or_expression()) {
      if (match("?")) {
        auto test = pop();
        if (!assignment_expression()) syntax_error();
        auto consequent = pop();
        if (!match(":") && assignment_expression()) syntax_error();
        auto alternate = pop();
        push<ConditionalExpression>(test, consequent, alternate);
      }
      return true;
    }
    return false;
  }

  bool conditional_expression_no_in()
  {
    if (logical_or_expression_no_in()) {
      if (match("?")) {
        auto test = pop();
        if (!assignment_expression()) syntax_error();
        auto consequent = pop();
        if (!match(":") && assignment_expression_no_in()) syntax_error();
        auto alternate = pop();
        push<ConditionalExpression>(test, consequent, alternate);
      }
      return true;
    }
    return false;
  }

  bool assignment_expression()
  {
    if (conditional_expression()) {
      if (match(is_assignment_operator)) {
        BinaryExpression expr;
        expr.lhs = pop();
        expr.op = *match.matched()->to_punctuator();
        if (!assignment_expression()) syntax_error();
        expr.rhs = pop();
        push(expr);
      }
      return true;
    }
    return false;
  }

  bool assignment_expression_no_in()
  {
    if (left_hand_side_expression()) {
      if (match(is_assignment_operator)) return assignment_expression_no_in() || syntax_error();
      return true;
    }
    return conditional_expression_no_in();
  }

  bool expression_opt()
  {
    return expression(), true;
  }

  bool expression()
  {
    if (assignment_expression()) {
      if (match(",")) {
        if (!assignment_expression()) syntax_error();
      }
      return true;
    }
    return false;
  }

  bool expression_no_in_opt()
  {
    return expression_no_in(), true;
  }

  bool expression_no_in()
  {
    if (assignment_expression_no_in()) {
      while (match(",") && (assignment_expression_no_in() || syntax_error()));
      return true;
    }
    return false;
  }

  Expression pop_expression()
  {
    return pop();
  }

};

class StatementBuilder
{

  std::vector<Statement> stack;

public:

  void push(Statement&& stmt)
  {
    stack.push_back(stmt);
  }

  template <typename T, typename... Args>
  void push(Args&&... args)
  {
    stack.push_back(T { std::forward<Args>(args)... });
  }

  Statement pop()
  {
    Statement stmt = stack.back();
    stack.pop_back();
    return stmt;
  }
};

class StatementParser : public ExpressionParser, private StatementBuilder
{
  using StatementBuilder::push;
  using StatementBuilder::pop;

  template <typename T> using optional = boost::optional<T>;

public:

  template <typename... Args>
  StatementParser(Args&&... args) : ExpressionParser(std::forward<Args>(args)...) {}

  // A.4
  bool statement()
  {
    return block() || variable_statement() || empty_statement() ||
      expression_statement() || if_statement() || iteration_statement() ||
      continue_statement() || break_statement() || return_statement() ||
      with_statement() || labelled_statement() || switch_statement() ||
      throw_statement() || try_statement() || debugger_statement();
  }

  bool block()
  {
    if (!match("{")) return false;
    auto list = statement_list();
    if (!match("}")) syntax_error();
    push<Block>(list);
    return true;
  }

  StatementList statement_list()
  {
    StatementList list;
    while (statement()) list.push_back(pop());
    return list;
  }

  bool variable_statement()
  {
    if (!match("var")) return false;
    if (!variable_declaration_list()) syntax_error();
    push<VariableStatement>();
    return true;
  }

  optional<VariableDeclarationList> variable_declaration_list()
  {
    if (auto decl = variable_declaration()) {
      VariableDeclarationList list { *decl };
      while (match(",")) {
        if (auto decl = variable_declaration()) list.push_back(*decl);
        else syntax_error();
      }
      return list;
    }
    return {};
  }

  optional<VariableDeclarationList> variable_declaration_list_no_in()
  {
    if (auto decl = variable_declaration_no_in()) {
      VariableDeclarationList list { *decl };
      while (match(",")) {
        if (auto decl = variable_declaration_no_in()) list.push_back(*decl);
        else syntax_error();
      }
      return list;
    }
    return {};
  }

  optional<VariableDeclaration> variable_declaration()
  {
    auto id = identifier();
    if (!id) return {};
    if (initializer())
      return VariableDeclaration { *id, pop_expression()};
    else
      return VariableDeclaration { *id };
  }

  optional<VariableDeclaration> variable_declaration_no_in()
  {
    auto id = identifier();
    if (!id) return {};
    if (initializer_no_in())
      return VariableDeclaration { *id, pop_expression()};
    else
      return VariableDeclaration { *id };
  }

  bool initializer()
  {
    return match("=") && assignment_expression();
  }

  bool initializer_no_in()
  {
    return match("=") && assignment_expression_no_in();
  }

  bool empty_statement()
  {
    if (!match(";")) return false;
    push<EmptyStatement>();
    return true;
  }

  bool expression_statement()
  {
    if (match.peek("{") || match.peek("function") || !expression()) return false;
    if (!automatic_semicolon_insertion()) syntax_error();
    push<ExpressionStatement>(pop_expression());
    return true;
  }

  bool if_statement()
  {
    if (!match("if")) return false;
    if (!match("(") && expression() && match(")") && statement()) syntax_error();
    auto expr = pop_expression();
    auto stmt = pop();
    if (match("else")) {
      if (!statement()) syntax_error();
      push<IfStatement>(expr, stmt, pop());
    }
    else push<IfStatement>(expr, stmt);
    return true;
  }

  bool iteration_statement()
  {
    if (match("do")) {
      if (
        statement() &&
        match("while") &&
        match("(") &&
        expression() &&
        match(")") &&
        automatic_semicolon_insertion()
      ) {
        Statement stmt = pop();
        Expression expr = pop_expression();
        push<DoWhileStatement>(stmt, expr);
        return true;
      }
      else syntax_error();
    }
    else if (match("while")) {
      if (
        match("(") &&
        expression() &&
        match(")") &&
        statement()
      ) {
        push<WhileStatement>(pop_expression(), pop());
        return true;
      }
      else syntax_error();
    }
    else if (match("for")) {
      // if (expression_no_in_op
      //   match("(") &&
      //   (
      //     match([this]{
      //       return expression_no_in_opt() && match(";") && expression_opt() && match(";") && expression_opt();
      //     }) || match([this]{
      //       return match("var") && variable_declaration_list_no_in() && match(";") && expression_opt() && match(";") && expression_opt();
      //     }) || match([this]{
      //       return left_hand_side_expression() && match("in") && expression();
      //     }) || match([this]{
      //       return match("var") && variable_declaration_no_in() && match("in") && expression();
      //     })
      //   ) &&
      //   match(")") &&
      //   statement()
      // ) || syntax_error();
      syntax_error();
    }
    return false;
  }

  bool continue_statement()
  {
    if (!match("continue")) return false;
    if (no_line_terminator_here() && identifier()) {
      if(!automatic_semicolon_insertion()) syntax_error();
      auto label = boost::get<Identifier>(pop_expression());
      push<ContinueStatement>(label);
    }
    if(!automatic_semicolon_insertion()) syntax_error();
    push<ContinueStatement>();
    return true;
  }

  bool break_statement()
  {
    if (match("break")) {
      if (no_line_terminator_here())
      {
        auto id = identifier();
        if (!id || !automatic_semicolon_insertion()) syntax_error();
        push<BreakStatement>(*id);
        return true;
      }
      if (!automatic_semicolon_insertion()) syntax_error();
      push<BreakStatement>();
      return true;
    }
    return false;
  }

  bool return_statement()
  {
    if (match("return")) {
      if (no_line_terminator_here() && expression())
      {
        if (!automatic_semicolon_insertion()) syntax_error();
        auto expr = pop_expression();
        push<ReturnStatement>(expr);
        return true;
      }
      if (!automatic_semicolon_insertion()) syntax_error();
      push<ReturnStatement>();
      return true;
    }
    return false;
  }

  bool with_statement()
  {
    if (!match("with")) return false;
    if (!match("(") || !expression() || !match(")") || !statement()) syntax_error();
    push<WithStatement>(pop_expression(), pop());
    return true;
  }

  bool switch_statement()
  {
    if (!match("switch")) return false;
    if (!match("(") || !expression() || !match(")") || !case_block()) syntax_error();
    push<SwitchStatement>(pop_expression());
    return true;
  }

  bool case_block()
  {
    if (!match("{")) return false;
    case_clauses();
    if (default_clause()) case_clauses();
    return match("}") || syntax_error("missing }");

  }

  bool case_clauses()
  {
    if (case_clause()) {
      while (case_clause());
      return true;
    }
    return false;
  }

  bool case_clause()
  {
    if (match("case")) {
      expression() || syntax_error();
      match(":") || syntax_error();
      auto list = statement_list();
      return true;
    }
    return false;
  }

  bool default_clause()
  {
    if (match("default")) {
      match(":") || syntax_error();
      auto list = statement_list();
      return true;
    }
    return false;
  }

  bool labelled_statement()
  {
    auto label = identifier();
    if (!label) return false;
    if (!match(":") || !statement()) syntax_error();
    push<LabelledStatement>(*label, pop());
    return true;
  }

  bool throw_statement()
  {
    if (!match("throw")) return false;
    if (!no_line_terminator_here() || !expression() ||
        !automatic_semicolon_insertion()) syntax_error();
    push<ThrowStatement>(pop_expression());
    return true;
  }

  bool try_statement()
  {
    if (!match("try")) return false;
    if (!block()) syntax_error();
    auto stmt = TryStatement { boost::get<Block>(pop()) };
    if (match("catch")) stmt.handler = boost::get<Block>(pop());
    if (match("finally")) stmt.finalizer = boost::get<Block>(pop());
    if (!stmt.handler && !stmt.finalizer) syntax_error();
    push(stmt);
    return true;
  }

  bool debugger_statement()
  {
    if (!match("debugger")) return false;
    if (!automatic_semicolon_insertion()) syntax_error();
    push<DebuggerStatement>();
    return true;
  }

  Statement pop_statement()
  {
    return pop();
  }
};

class ProgramBuilder
{
  using Declaration = boost::variant<
    SourceElement,
    SourceElements,
    Program
  >;
  std::vector<Declaration> stack;

public:

  void push(Declaration&& decl)
  {
    stack.push_back(decl);
  }

  template <typename T, typename... Args>
  void push(Args&&... args)
  {
    stack.push_back(T { std::forward<Args>(args)... });
  }

  Declaration pop()
  {
    Declaration decl = stack.back();
    stack.pop_back();
    return decl;
  }

};

class Parser : StatementParser, private ProgramBuilder
{

  using ProgramBuilder::push;
  using ProgramBuilder::pop;

  // A.5
  bool function_declaration()
  {
    if (!match("function")) return false;
    FunctionDeclaration decl;
    auto id = identifier();
    if (!identifier()) syntax_error();
    decl.id = *id;
    if (!match("(")) syntax_error();
    decl.params = formal_parameter_list();
    if (!match(")") || !match("{")) syntax_error();
    decl.body = function_body();
    if (!match("}")) syntax_error();
    push(decl);
    return true;
  }

  bool function_expression()
  {
    if (!match("function")) return false;
    FunctionExpression expr;
    expr.id = identifier();
    if (!match("(")) syntax_error();
    expr.params = formal_parameter_list();
    if (!match(")") || !match("{")) syntax_error();
    expr.body = function_body();
    if (!match("}")) syntax_error();
    // push(expr);
    return true;
  }

  FormalParameterList formal_parameter_list()
  {
    FormalParameterList list;
    while (auto id = identifier()) {
      list.push_back(*id);
      if (!match(",")) break;
    }
    return list;
  }

  FunctionBody function_body()
  {
    return { source_elements() };
  }

  Program program()
  {
    return {source_elements()};
  }

  SourceElements source_elements()
  {
    SourceElements list;
    while (source_element()) list.push_back(pop_statement());
    return list;
  }

  bool source_element()
  {
    return statement() || function_declaration();
  }

public:

  template <typename... Args>
  Parser(Args&&... args) : StatementParser(std::forward<Args>(args)...) {}

  Program parse()
  {
    return program();
  }

};

template <typename It>
Parser make_parser(It f, It l)
{
  return {f, l};
}

template <typename Cont>
auto make_parser(Cont&& cont)
{
  using std::begin;
  using std::end;
  return make_parser(begin(cont), end(cont));
}

#endif
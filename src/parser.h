#ifndef ECMASCRIPT_PARSER_H
#define ECMASCRIPT_PARSER_H

#include "ast.h"
#include "logger.h"
#include "matcher.h"
#include "token.h"
#include "trace.h"
#include "util.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include <boost/type_index.hpp>

class SyntaxError : public std::runtime_error {
public:
  SyntaxError(std::string what) : std::runtime_error(what) {}
};

class Parser {

  Matcher<Token, std::vector<Token>::iterator> match;

  std::shared_ptr<std::vector<std::unique_ptr<Node>>> storage;
  std::vector<Node *> stack;
  Logger *m_logger = nullptr;

  [[noreturn]] bool logic_error(std::string what = {}) {
    auto debug_info = match.done() ? nullptr : match.matching()->debug_info;
    auto loc = debug_info ? debug_info->loc() : "";
    throw SyntaxError("InternalCompilerError: " + loc + what + "\n" +
                      (debug_info ? debug_info->syntax_error_at() : "") +
                      stack_trace());
  }

  [[noreturn]] bool syntax_error(std::string what = {}) {
    auto debug_info = match.done() ? nullptr : match.matching()->debug_info;

    throw SyntaxError("SyntaxError: " + what + "\n" +
                      (debug_info ? debug_info->syntax_error_at() : "") +
                      stack_trace());
  }

  template <typename F> void log(F &&callback) {
    if (m_logger)
      m_logger->log(callback);
  }

  template <typename T> void push(T *value) {
    log([&](auto &log) { log << "push " << demangle(value) << "\n"; });
    stack.push_back(value);
  }

  template <typename T, typename... Args> T *emplace(Args &&... args) {
    T *value = new T{std::forward<Args>(args)...};
    log([&](auto &log) { log << "emplace " << demangle(value) << "\n"; });
    storage->emplace_back(value);
    stack.push_back(value);
    return value;
  }

  template <typename T> T *pop() {
    if (auto value = dynamic_cast<T *>(stack.back())) {
      log([&](auto &log) { log << "pop " << demangle(value) << "\n"; });
      stack.pop_back();
      return value;
    } else
      logic_error("Expected " + demangle<T>() + ", got " +
                  stack.back()->type());
  }

  template <typename T, std::size_t I> T *peek_at() {
    if (auto value = dynamic_cast<T *>(*(stack.end() - I))) {
      log([&](auto &log) { log << "peek " << demangle(value) << "\n"; });
      return value;
    } else
      logic_error("Expected " + demangle<T>() + ", got " +
                  stack.back()->type());
  }

  template <typename T, typename... Ts, typename... Args>
  T *replace(Args &&... args) {

    return index_apply<sizeof...(Ts)>([&](auto... Is) {
      T *value = new T{std::forward<Args>(args)...,
                       peek_at<Ts, sizeof...(Ts) - Is>()...};

      log([&](auto &log) {
        log << "replace [";
        for_each_arg(
            [&](auto arg) { log << " " << demangle(arg) << ","; },
            dynamic_cast<Ts *>(*(stack.end() - (sizeof...(Ts) - Is)))...);
        log << "] with " << demangle(value) << "\n";
      });

      stack.resize(stack.size() - sizeof...(Ts));
      storage->emplace_back(value);
      stack.push_back(value);
      return value;
    });
  }

  template <typename T> bool last_match_is_a() {
    return !(stack.empty() || !dynamic_cast<T *>(stack.back()));
  }

  bool no_line_terminator_here() {
    return match.peek([](const Token &token) {
      return !token.preceded_by_line_terminator();
    });
  }

  // 7.9
  bool automatic_semicolon_insertion() {
    if (match(";"))
      return true;
    if (no_line_terminator_here() && !match.peek("}"))
      return false; // 7.9.1
    return true;
  }

  // A.1
  bool identifier() {
    trace("identifier");
    if (!match([](const auto &token) { return token.is_identifier(); }))
      return false;
    emplace<Identifier>(match->string_value());
    return true;
  }

  bool identifier_name() {
    trace("identifier_name");
    if (!match([](const auto &token) { return token.is_identifier_name(); }))
      return false;
    emplace<Identifier>(match->string_value());
    return true;
  }

  bool literal() {
    trace("literal");
    return null_literal() || boolean_literal() || numeric_literal() ||
           string_literal() || regular_expression_literal();
  }

  bool null_literal() {
    trace("null_literal");
    if (!match([](const auto &token) { return token.is_null_literal(); }))
      return false;
    emplace<NullLiteral>();
    return true;
  }

  bool boolean_literal() {
    trace("boolean_literal");
    if (!match([](const auto &token) { return token.is_boolean_literal(); }))
      return false;
    emplace<BooleanLiteral>(match->boolean_value());
    return true;
  }

  bool numeric_literal() {
    trace("numeric_literal");
    if (!match([](const auto &token) { return token.is_numeric_literal(); }))
      return false;
    emplace<NumericLiteral>(match->numeric_value());
    return true;
  }

  bool string_literal() {
    trace("string_literal");
    if (!match([](const auto &token) { return token.is_string_literal(); }))
      return false;
    emplace<StringLiteral>(match->string_value());
    return true;
  }

  bool regular_expression_literal() {
    trace("regular_expression_literal");
    if (!match([](const auto &token) {
          return token.is_regular_expression_literal();
        }))
      return false;
    emplace<RegularExpressionLiteral>(match->string_value());
    return true;
  }

  // A.3
  bool primary_expression() {
    trace("primary_expression");
    if (match("(")) {
      if (!expression() || !match(")"))
        syntax_error();
    } else if (match("this")) {
      emplace<This>();
      replace<ThisExpression, This>();
    } else if (identifier()) {
      replace<IdentifierExpression, Identifier>();
    } else if (literal()) {
      replace<LiteralExpression, Literal>();
      return true;
    } else if (array_literal()) {
      replace<ArrayExpression, ArrayLiteral>();
    } else if (object_literal()) {
      replace<ObjectExpression, ObjectLiteral>();
    } else
      return false;
    return true;
  }

  bool array_literal() {
    trace("array_literal");
    if (!match("["))
      return false;
    if (!element_list())
      emplace<ElementList>();

    auto expr = replace<ArrayLiteral, ElementList>();

    if (elision())
      expr->elision = pop<Elision>();
    if (!match("]"))
      syntax_error("Expected ]");
    return true;
  }

  bool element_list() {
    trace("element_list");
    ElementList *list;
    if (elision()) {
      list = replace<ElementList, Node>();
      if (assignment_expression())
        list->push_back(pop<Expression>());
    } else if (assignment_expression()) {
      list = replace<ElementList, Node>();
    } else
      return false;
    while (match([&] {
      if (!match(","))
        return false;
      auto el = elision() ? pop<Elision>() : nullptr;
      if (!assignment_expression())
        return false;
      if (el)
        list->push_back(el);
      list->push_back(pop<Expression>());
      return true;
    }))
      continue;
    return true;
  }

  bool elision() {
    trace("elision");
    if (!match(","))
      return false;
    auto expr = emplace<Elision>();
    while (match(","))
      ++expr->count;
    return true;
  }

  bool object_literal() {
    trace("object_literal");
    if (!match("{"))
      return false;
    if (property_name_and_value_list())
      replace<ObjectLiteral, PropertyNameAndValueList>();
    else
      emplace<ObjectLiteral>();
    match(",");
    if (!match("}"))
      syntax_error("Expected }");
    return true;
  }

  bool property_name_and_value_list() {
    trace("property_name_and_value_list");
    if (!property_assignment())
      return false;
    auto list = replace<PropertyNameAndValueList, PropertyAssignment>();
    while (!match.lookahead("}") && match(",")) {
      if (!property_assignment())
        syntax_error();
      list->push_back(pop<PropertyAssignment>());
    }
    return true;
  }

  bool property_assignment() {
    trace("property_assignment");
    if (match.lookahead(":")) {
      if (!property_name())
        return false;
      if (!match(":") || !assignment_expression())
        syntax_error();
      replace<PropertyAssignment, PropertyName, Expression>();
      return true;
    } else if (match("get")) {
      if (!property_name() || !match("(") || !match(")") || !match("{") ||
          !function_body() || !match("}"))
        syntax_error("get");
      replace<PropertyAssignment, PropertyName, FunctionBody>();
      return true;
    } else if (match("set")) {
      if (!property_name() || !match("(") || !property_set_parameter_list() ||
          !match(")") || !match("{") || !function_body() || !match("}"))
        syntax_error("set");
      replace<PropertyAssignment, PropertyName, Identifier, FunctionBody>();
      return true;
    } else
      return false;
  }

  bool property_name() {
    trace("property_name");
    if (identifier_name()) {
      replace<PropertyName, Identifier>();
      return true;
    } else if (string_literal()) {
      replace<PropertyName, StringLiteral>();
      return true;
    } else if (numeric_literal()) {
      replace<PropertyName, NumericLiteral>();
      return true;
    } else
      return false;
  }

  bool property_set_parameter_list() {
    trace("property_set_parameter_list");
    return identifier();
  }

  bool member_expression() {
    trace("member_expression");
    if (match("new")) {
      if (!member_expression())
        return false;
      if (!arguments()) {
        // new NewExpression
        replace<NewExpression, Expression>();
        return true;
      }
      replace<NewExpression, Expression, Arguments>();
    } else if (!primary_expression() && !function_expression())
      return false;
    while (true) {
      if (match("[")) {
        if (!expression() || !match("]"))
          syntax_error();
        replace<MemberExpression, Expression, Expression>();
      } else if (match(".")) {
        if (!identifier_name())
          syntax_error();
        replace<MemberExpression, Expression, Identifier>();
      } else
        break;
    }
    return true;
  }

  bool new_expression() {
    trace("new_expression");
    return member_expression();
  }

  bool call_expression() {
    trace("call_expression");
    // if (!last_match_is_a<MemberExpression>()
    //     && !last_match_is_a<NewExpression>())
    //   return false;
    if (!arguments())
      return false;
    replace<CallExpression, Expression, Arguments>();
    while (true) {
      if (arguments()) {
        replace<CallExpression, Expression, Arguments>();
      } else if (match("[")) {
        if (!expression() || !match("]"))
          syntax_error();
        replace<MemberExpression, Expression, Expression>();
      } else if (match(".")) {
        if (!identifier_name())
          syntax_error();
        replace<MemberExpression, Expression, Identifier>();
      } else
        break;
    }
    return true;
  }

  bool arguments() {
    trace("arguments");
    if (!match("("))
      return false;
    if (argument_list())
      replace<Arguments, ArgumentList>();
    else
      emplace<Arguments>();
    if (!match(")"))
      syntax_error();
    return true;
  }

  bool argument_list() {
    trace("argument_list");
    if (!assignment_expression())
      return false;
    auto list = replace<ArgumentList, Expression>();
    while (match(",")) {
      if (!assignment_expression())
        syntax_error();
      list->push_back(pop<Expression>());
    }
    return true;
  }

  bool left_hand_side_expression() {
    trace("left_hand_side_expression");
    if (!new_expression())
      return false;
    call_expression();
    return true;
  }

  bool postfix_expression() {
    trace("postfix_expression");
    if (!left_hand_side_expression())
      return false;
    if (no_line_terminator_here()) {
      if (match("++") || match("--"))
        replace<PostfixExpression, Expression>(match);
    }
    return true;
  }

  bool unary_expression() {
    trace("unary_expression");
    if (match.any_of("delete", "void", "typeof", "++", "--", "+", "-", "~",
                     "!")) {
      auto expr = emplace<UnaryExpression>(*match);
      if (!unary_expression())
        syntax_error("Expected UnaryExpression");
      expr->rhs = pop<Expression>();
      return true;
    }
    return postfix_expression();
  }

  bool multiplicative_expression() {
    trace("multiplicative_expression");
    if (!unary_expression())
      return false;
    while (match.any_of("*", "/", "%")) {
      auto expr = replace<BinaryExpression, Expression>(*match);
      if (!unary_expression())
        syntax_error("Expected UnaryExpression");
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool additive_expression() {
    trace("additive_expression");
    if (!multiplicative_expression())
      return false;
    while (match.any_of("+", "-")) {
      auto expr = emplace<BinaryExpression>(*match, pop<Expression>());
      if (!multiplicative_expression())
        syntax_error("Expected MultiplicativeExpression");
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool shift_expression() {
    trace("shift_expression");
    if (!additive_expression())
      return false;
    while (match.any_of("<<", ">>", ">>>")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!additive_expression())
        syntax_error("Expected AdditiveExpression");
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool relational_expression() {
    trace("relational_expression");
    if (!shift_expression())
      return false;
    while (match.any_of("<", ">", "<=", ">=", "instanceof", "in")) {
      auto expr = replace<BinaryExpression, Expression>(match);
      if (!shift_expression())
        syntax_error("Expected ShiftExpression");
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool relational_expression_no_in() {
    trace("relational_expression_no_in");
    if (!shift_expression())
      return false;
    while (match.any_of("<", ">", "<=", ">=", "instanceof")) {
      auto expr = replace<BinaryExpression, Expression>(match);
      if (!shift_expression())
        syntax_error("Expected ShiftExpression");
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool equality_expression() {
    trace("equality_expression");
    if (!relational_expression())
      return false;
    while (match.any_of("==", "!=", "===", "!==")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!relational_expression())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool equality_expression_no_in() {
    trace("equality_expression_no_in");
    if (!relational_expression_no_in())
      return false;
    while (match.any_of("==", "!=", "===", "!==")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!relational_expression_no_in())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool bitwise_and_expression() {
    trace("bitwise_and_expression");
    if (!equality_expression())
      return false;
    while (match("&")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!equality_expression())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool bitwise_and_expression_no_in() {
    trace("bitwise_and_expression_no_in");
    if (!equality_expression_no_in())
      return false;
    while (match("&")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!equality_expression_no_in())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool bitwise_xor_expression() {
    trace("bitwise_xor_expression");
    if (!bitwise_and_expression())
      return false;
    while (match("^")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!bitwise_and_expression())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool bitwise_xor_expression_no_in() {
    trace("bitwise_xor_expression_no_in");
    if (!bitwise_and_expression_no_in())
      return false;
    while (match("^")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!bitwise_and_expression_no_in())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool bitwise_or_expression() {
    trace("bitwise_or_expression");
    if (!bitwise_xor_expression())
      return false;
    while (match("|")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!bitwise_xor_expression())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool bitwise_or_expression_no_in() {
    trace("bitwise_or_expression_no_in");
    if (!bitwise_xor_expression_no_in())
      return false;
    while (match("|")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!bitwise_xor_expression_no_in())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool logical_and_expression() {
    trace("logical_and_expression");
    if (!bitwise_or_expression())
      return false;
    while (match("&&")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!bitwise_or_expression())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool logical_and_expression_no_in() {
    trace("logical_and_expression_no_in");
    if (!bitwise_or_expression_no_in())
      return false;
    while (match("&&")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!bitwise_or_expression_no_in())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool logical_or_expression() {
    trace("logical_or_expression");
    if (!logical_and_expression())
      return false;
    while (match("||")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!logical_and_expression())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool logical_or_expression_no_in() {
    trace("logical_or_expression_no_in");
    if (!logical_and_expression_no_in())
      return false;
    while (match("||")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!logical_and_expression_no_in())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool conditional_expression() {
    trace("conditional_expression");
    if (!logical_or_expression())
      return false;
    if (match("?")) {
      if (!assignment_expression() || !match(":") || !assignment_expression())
        syntax_error();
      replace<ConditionalExpression, Expression, Expression, Expression>();
    }
    return true;
  }

  bool conditional_expression_no_in() {
    trace("conditional_expression_no_in");
    if (!logical_or_expression_no_in())
      return false;
    if (match("?")) {
      if (!assignment_expression() || !match(":") ||
          !assignment_expression_no_in())
        syntax_error();
      replace<ConditionalExpression, Expression, Expression, Expression>();
    }
    return true;
  }

  bool assignment_operator() {
    trace("assignment_operator");
    return match.any_of("=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", ">>>=",
                        "&=", "^=", "|=");
  }

  bool assignment_expression() {
    trace("assignment_expression");
    if (!conditional_expression())
      return false;
    if (assignment_operator()) {
      auto expr = emplace<AssignmentExpression>(match, pop<Expression>());
      if (!assignment_expression())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool assignment_expression_no_in() {
    trace("assignment_expression_no_in");
    if (!conditional_expression_no_in())
      return false;
    if (assignment_operator()) {
      auto expr = emplace<AssignmentExpression>(match, pop<Expression>());
      if (!assignment_expression_no_in())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool expression() {
    trace("expression");
    if (!assignment_expression())
      return false;
    while (match(",")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!assignment_expression())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool expression_no_in() {
    trace("expression_no_in");
    if (!assignment_expression_no_in())
      return false;
    while (match(",")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!assignment_expression_no_in())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  // A.4
  bool statement() {
    trace("statement");
    return block() || variable_statement() || empty_statement() ||
           expression_statement() || if_statement() || iteration_statement() ||
           continue_statement() || break_statement() || return_statement() ||
           with_statement() || labelled_statement() || switch_statement() ||
           throw_statement() || try_statement() || debugger_statement();
  }

  bool block() {
    trace("block");
    if (!match("{"))
      return false;
    if (statement_list())
      replace<Block, StatementList>();
    else
      emplace<Block>();
    if (!match("}"))
      syntax_error();
    return true;
  }

  bool statement_list() {
    trace("statement_list");
    auto list = emplace<StatementList>();
    while (statement())
      list->push_back(pop<Statement>());
    return true;
  }

  bool variable_statement() {
    trace("variable_statement");
    if (!match("var"))
      return false;
    if (!variable_declaration_list())
      syntax_error();
    if (!automatic_semicolon_insertion())
      syntax_error("Expected ;");
    emplace<VariableStatement>(pop<VariableDeclarationList>());
    return true;
  }

  bool variable_declaration_list() {
    trace("variable_declaration_list");
    auto list = emplace<VariableDeclarationList>();
    if (variable_declaration()) {
      list->push_back(pop<VariableDeclaration>());
      while (match(",")) {
        if (!variable_declaration())
          syntax_error();
        list->push_back(pop<VariableDeclaration>());
      }
    }
    return true;
  }

  bool variable_declaration_list_no_in() {
    trace("variable_declaration_list_no_in");
    auto list = emplace<VariableDeclarationList>();
    if (variable_declaration_no_in()) {
      list->push_back(pop<VariableDeclaration>());
      while (match(",")) {
        if (!variable_declaration_no_in())
          syntax_error();
        list->push_back(pop<VariableDeclaration>());
      }
    }
    return true;
  }

  bool variable_declaration() {
    trace("variable_declaration");
    if (!identifier())
      return false;
    auto id = pop<Identifier>();
    if (initializer())
      emplace<VariableDeclaration>(id, pop<Expression>());
    else
      emplace<VariableDeclaration>(id);
    return true;
  }

  bool variable_declaration_no_in() {
    trace("variable_declaration_no_in");
    if (!identifier())
      return false;
    auto id = pop<Identifier>();
    if (initializer_no_in())
      emplace<VariableDeclaration>(id, pop<Expression>());
    else
      emplace<VariableDeclaration>(id);
    return true;
  }

  bool initializer() {
    trace("initializer");
    return match("=") && (assignment_expression() || syntax_error());
  }

  bool initializer_no_in() {
    trace("initializer_no_in");
    return match("=") && assignment_expression_no_in();
  }

  bool empty_statement() {
    trace("empty_statement");
    if (!match(";"))
      return false;
    emplace<EmptyStatement>();
    return true;
  }

  bool expression_statement() {
    trace("expression_statement");
    if (match.peek("{") || match.peek("function") || match.lookahead(":") ||
        !expression())
      return false;
    if (!automatic_semicolon_insertion())
      syntax_error();
    replace<ExpressionStatement, Expression>();
    return true;
  }

  bool if_statement() {
    trace("if_statement");
    if (!match("if"))
      return false;
    if (!match("(") || !expression() || !match(")") || !statement())
      syntax_error();
    auto stmt = pop<Statement>();
    auto expr = pop<Expression>();
    if (match("else")) {
      if (!statement())
        syntax_error();
      emplace<IfStatement>(expr, stmt, pop<Statement>());
    } else
      emplace<IfStatement>(expr, stmt);
    return true;
  }

  bool iteration_statement() {
    trace("iteration_statement");
    return do_while_statement() || while_statement() || for_statement() ||
           for_in_statement();
  }

  bool do_while_statement() {
    trace("do_while_statement");
    if (!match("do"))
      return false;
    if (!statement() || !match("while") || !match("(") || !expression() ||
        !match(")") || !automatic_semicolon_insertion())
      syntax_error();
    replace<DoWhileStatement, Statement, Expression>();
    return true;
  }

  bool while_statement() {
    trace("while_statement");
    if (!match("while"))
      return false;
    if (!match("(") || !expression() || !match(")") || !statement())
      syntax_error();
    replace<WhileStatement, Expression, Statement>();
    return true;
  }

  bool for_statement() {
    trace("for_statement");
    return match([this] {
      if (!match("for"))
        return false;
      if (!match("("))
        syntax_error("Expected (");
      ForStatement *stmt;
      if (match("var") && variable_declaration_list_no_in()) {
        if (!match(";")) {
          pop<VariableDeclarationList>();
          return false;
        }
        stmt = replace<ForStatement, VariableDeclarationList>();
      } else {
        if (expression())
          stmt = replace<ForStatement, Expression>();
        else
          stmt = emplace<ForStatement>();
        if (!match(";")) {
          pop<ForStatement>();
          return false;
        }
      }
      stmt->test = expression() ? pop<Expression>() : nullptr;
      if (!match(";"))
        syntax_error("Expected ;");
      stmt->update = expression() ? pop<Expression>() : nullptr;
      if (!match(")"))
        syntax_error("Expected )");
      if (!statement())
        syntax_error("Expected statement");
      stmt->body = pop<Statement>();
      return true;
    });
  }

  bool for_in_statement() {
    trace("for_in_statement");
    return match([this] {
      if (!match("for"))
        return false;
      if (!match("("))
        syntax_error("Expected (");
      ForInStatement *stmt;
      if (match("var") && variable_declaration_no_in() && match("in"))
        stmt = replace<ForInStatement, VariableDeclaration>();
      else if (left_hand_side_expression() && match("in"))
        stmt = replace<ForInStatement, LeftHandSideExpression>();
      else
        return false;
      if (!expression())
        syntax_error("Expected expression");
      stmt->right = pop<Expression>();
      if (!match(")"))
        syntax_error("Expected )");
      if (!statement())
        syntax_error("Expected statement");
      stmt->body = pop<Statement>();
      return true;
    });
  }

  bool continue_statement() {
    trace("continue_statement");
    if (!match("continue"))
      return false;
    auto label = (no_line_terminator_here() && identifier()) ? pop<Identifier>()
                                                             : nullptr;
    if (!automatic_semicolon_insertion())
      syntax_error();
    emplace<ContinueStatement>(label);
    return true;
  }

  bool break_statement() {
    trace("break_statement");
    if (!match("break"))
      return false;
    auto label = (no_line_terminator_here() && identifier()) ? pop<Identifier>()
                                                             : nullptr;

    if (!automatic_semicolon_insertion())
      syntax_error("missing ;");
    emplace<BreakStatement>(label);
    return true;
  }

  bool return_statement() {
    trace("return_statement");
    if (!match("return"))
      return false;
    auto expr = (no_line_terminator_here() && expression()) ? pop<Expression>()
                                                            : nullptr;
    if (!automatic_semicolon_insertion())
      syntax_error("missing ;");
    emplace<ReturnStatement>(expr);
    return true;
  }

  bool with_statement() {
    trace("with_statement");
    if (!match("with"))
      return false;
    if (!match("(") || !expression() || !match(")") || !statement())
      syntax_error();
    replace<WithStatement, Expression, Statement>();
    return true;
  }

  bool switch_statement() {
    trace("switch_statement");
    if (!match("switch"))
      return false;
    if (!match("(") || !expression() || !match(")") || !case_block())
      syntax_error();
    replace<SwitchStatement, Expression, CaseBlock>();
    return true;
  }

  bool case_block() {
    trace("case_block");
    if (!match("{"))
      return false;
    auto list = emplace<CaseBlock>();
    while (case_clause())
      list->push_back(pop<CaseClause>());
    if (default_clause()) {
      list->push_back(pop<CaseClause>());
      while (case_clause())
        list->push_back(pop<CaseClause>());
    }
    if (!match("}"))
      syntax_error("missing }");
    return true;
  }

  bool case_clause() {
    trace("case_clause");
    if (!match("case"))
      return false;
    if (!expression())
      syntax_error();
    if (!match(":"))
      syntax_error("missing : after case label");
    if (!statement_list())
      syntax_error();
    auto list = pop<StatementList>();
    auto test = pop<Expression>();
    emplace<CaseClause>(test, list);
    return true;
  }

  bool default_clause() {
    trace("default_clause");
    if (!match("default"))
      return false;
    if (!match(":"))
      syntax_error("missing : after case label");
    if (!statement_list())
      syntax_error();
    emplace<DefaultClause>(pop<StatementList>());
    return true;
  }

  bool labelled_statement() {
    trace("labelled_statement");
    return match([this] {
      if (!identifier() || !match(":"))
        return false;
      if (!statement())
        syntax_error();
      auto stmt = pop<Statement>();
      auto label = pop<Identifier>();
      emplace<LabelledStatement>(label, stmt);
      return true;
    });
  }

  bool throw_statement() {
    trace("throw_statement");
    if (!match("throw"))
      return false;
    if (!no_line_terminator_here() || !expression() ||
        !automatic_semicolon_insertion())
      syntax_error();
    emplace<ThrowStatement>(pop<Expression>());
    return true;
  }

  bool try_statement() {
    trace("try_statement");
    if (!match("try"))
      return false;
    if (!block())
      syntax_error();
    auto stmt = emplace<TryStatement>(pop<Block>());
    if (match("catch")) {
      if (!match("(") || !identifier() || !match(")") || !block())
        syntax_error();
      stmt->handler = pop<Block>();
      stmt->binding = pop<Identifier>();
    }
    if (match("finally")) {
      if (!block())
        syntax_error();
      stmt->finalizer = pop<Block>();
    }
    if (!stmt->handler && !stmt->finalizer)
      syntax_error();
    return true;
  }

  bool debugger_statement() {
    trace("debugger_statement");
    if (!match("debugger"))
      return false;
    if (!automatic_semicolon_insertion())
      syntax_error();
    emplace<DebuggerStatement>();
    return true;
  }

  // A.5
  bool function_declaration() {
    trace("function_declaration");
    if (!match("function"))
      return false;

    if (!identifier() || !match("(") || !formal_parameter_list() ||
        !match(")") || !match("{") || !function_body() || !match("}"))
      syntax_error();

    replace<FunctionDeclaration, Identifier, FormalParameterList,
            FunctionBody>();
    return true;
  }

  bool function_expression() {
    trace("function_expression");
    if (!match("function"))
      return false;

    auto id = identifier() ? pop<Identifier>() : nullptr;

    if (!match("(") || !formal_parameter_list() || !match(")") || !match("{") ||
        !function_body() || !match("}"))
      syntax_error();

    auto body = pop<FunctionBody>();
    auto params = pop<FormalParameterList>();
    emplace<FunctionExpression>(id, params, body);
    return true;
  }

  bool formal_parameter_list() {
    trace("formal_parameter_list");
    auto list = emplace<FormalParameterList>();
    while (identifier()) {
      list->push_back(pop<Identifier>());
      if (!match(","))
        break;
    }
    return true;
  }

  bool function_body() {
    trace("function_body");
    if (source_elements())
      emplace<FunctionBody>(pop<SourceElements>());
    else
      emplace<FunctionBody>();
    return true;
  }

  bool program() {
    trace("program");
    if (source_elements())
      replace<ProgramDeclaration, SourceElements>();
    else
      emplace<ProgramDeclaration>();
    return true;
  }

  bool source_elements() {
    trace("source_elements");
    auto list = emplace<SourceElements>();
    while (source_element())
      list->push_back(pop<SourceElement>());
    return true;
  }

  bool source_element() {
    trace("source_element");
    return statement() || function_declaration();
  }

public:
  template <typename... Args>
  Parser(Args &&... args)
      : match(std::forward<Args>(args)...),
        storage(std::make_shared<std::vector<std::unique_ptr<Node>>>()) {}

  Program parse() {
    trace("parse");
    if (!program())
      syntax_error();
    return {storage, pop<ProgramDeclaration>()};
  }

  void logger(Logger *logger = make_silent_logger()) { m_logger = logger; }
};

template <typename It> Parser make_parser(It f, It l) { return {f, l}; }

template <typename Cont> auto make_parser(Cont &&cont) {
  using std::begin;
  using std::end;
  return make_parser(begin(cont), end(cont));
}

#endif
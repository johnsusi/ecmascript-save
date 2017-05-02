#ifndef ECMASCRIPT_PARSER_H
#define ECMASCRIPT_PARSER_H

#include "ast.h"
#include "matcher.h"
#include "token.h"
#include "util.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

class Parser {

  Matcher<Token, std::vector<Token>::iterator> match;

  std::shared_ptr<std::vector<std::unique_ptr<Node>>> storage;
  std::vector<Node *>                                 stack;

  template <typename T>
  void push(T *value)
  {
    stack.push_back(value);
  }

  template <typename T, typename... Args>
  T *emplace(Args &&... args)
  {
    T *value = new T{std::forward<Args>(args)...};
    storage->emplace_back(value);
    stack.push_back(value);
    return value;
  }

  template <typename T>
  T *pop()
  {
    if (auto value = dynamic_cast<T *>(stack.back())) {
      stack.pop_back();
      return value;
    }
    else
      syntax_error("Bad pop");
    return nullptr;
  }

  // template <typename T, std::size_t N> void replace()
  // {
  //   index_apply<N>([&](auto... Is) {
  //     auto value = new T{get<T>(*(stack.end() - (N - Is))).value...};
  //     stack.resize(stack.size() - N);
  //     s
  //   });
  // }

  // template <typename T, typename... Ts, typename... Args>
  // void replace(Args &&... args)
  // {
  //   index_apply<sizeof...(Ts)>([&](auto... Is) {
  //     T* value = new T{get<Ts>::invoke(*(stack.end() - (sizeof...(Ts) -
  //     Is)))...,
  //             std::forward<Args>(args)...};
  //     stack.resize(stack.size() - sizeof...(Ts));
  //     stack.push_back(std::move(value));
  //   });
  // }

  template <typename T, typename... Ts, typename... Args>
  void replace(Args &&... args)
  {

    index_apply<sizeof...(Ts)>([&](auto... Is) {
      T *value =
          new T{dynamic_cast<Ts>(*(stack.end() - (sizeof...(Ts) - Is)))...};
      storage->emplace_back(value);
      stack.push_back(value);
    });
  }

  bool no_line_terminator_here()
  {
    return match.peek(
        [](const Token &token) { return !token.preceded_by_line_terminator; });
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
    throw std::runtime_error("SyntaxError: " + what + "\n" +
                             match.matching()->debug_info->syntax_error_at());
  }

  // A.1
  bool identifier()
  {
    if (!match([](const auto &token) { return token.is_identifier(); }))
      return false;
    emplace<Identifier>(*match->to_identifier());
    return true;
  }

  bool identifier_name()
  {
    return match([](const auto &token) { return token.is_identifier_name(); });
  }

  bool literal()
  {
    return null_literal() || boolean_literal() || numeric_literal() ||
           string_literal() || regular_expression_literal();
  }

  bool null_literal()
  {
    if (!match([](const auto &token) { return token.is_null_literal(); }))
      return false;
    emplace<NullLiteral>();
    return true;
  }

  bool boolean_literal()
  {
    if (!match([](const auto &token) { return token.is_boolean_literal(); }))
      return false;
    emplace<BooleanLiteral>(*match->to_boolean_literal());
    return true;
  }

  bool numeric_literal()
  {
    if (!match([](const auto &token) { return token.is_numeric_literal(); }))
      return false;
    emplace<NumericLiteral>(*match->to_numeric_literal());
    return true;
  }

  bool string_literal()
  {
    if (!match([](const auto &token) { return token.is_string_literal(); }))
      return false;
    emplace<StringLiteral>(*match->to_string_literal());
    return true;
  }

  bool regular_expression_literal()
  {
    if (!match([](const auto &token) {
          return token.is_regular_expression_literal();
        }))
      return false;
    // emplace<RegularExpressionLiteral>(match);
    return true;
  }

  // A.3
  bool primary_expression()
  {
    if (match("(")) {
      if (!expression() || !match(")")) syntax_error();
      return true;
    }

    if (match("this")) {
      emplace<This>();
      return true;
    }

    return identifier() || literal(); // || array_literal() || object_literal();
  }

  bool array_literal()
  {
    if (match("[")) {
      auto expr                          = emplace<ArrayLiteral>();
      if (element_list()) expr->elements = pop<ElementList>();
      if (elision()) expr->elision       = pop<Elision>();
      if (!match("]")) syntax_error();
      return true;
    }
    return false;
  }

  bool element_list()
  {
    if (!elision() || !assignment_expression()) return false;
    auto list = emplace<ElementList>();
    list->push_back(pop<Expression>());
    while (match([&] {
      if (!match(",")) return false;
      auto el = elision() ? pop<Elision>() : nullptr;
      if (!assignment_expression()) return false;
      if (el) list->push_back(el);
      list->push_back(pop<Expression>());
      return true;
    }))
      continue;
    return true;
  }

  bool elision()
  {
    if (!match(",")) return false;
    auto expr = emplace<Elision>();
    while (match(",")) ++expr->count;
    return true;
  }

  bool object_literal()
  {
    if (match("{")) {
      property_name_and_value_list();
      match(",");
      if (!match("}")) syntax_error();
      emplace<ObjectLiteral>();
      return true;
    }
    return false;
  }

  bool property_name_and_value_list()
  {
    if (property_assignment()) {
      while (match(",") && (property_assignment() || syntax_error()))
        ;
      return true;
    }
    return false;
  }

  bool property_assignment()
  {
    if (match("get")) {
      if (!property_name() || !match("(") || !match(")") || !match("{") ||
          !function_body() || !match("}"))
        syntax_error();
      return true;
    }
    else if (match("set")) {
      if (!property_name() || !match("(") || !property_set_parameter_list() ||
          !match(")") || !match("{") || !function_body() || !match("}"))
        syntax_error();
      return true;
    }
    else if (property_name()) {
      if (!match(":") || !assignment_expression()) syntax_error();
      return true;
    }
    else
      return false;
  }

  bool property_name()
  {
    return identifier_name() || string_literal() || numeric_literal();
  }

  bool property_set_parameter_list() { return identifier(); }

  bool member_expression()
  {
    if (!primary_expression() && !function_expression()) return false;
    while (true) {
      if (match("[")) {
        if (!expression() || !match("]")) syntax_error();
      }
      if (match(".")) {
        if (!identifier_name()) syntax_error();
        emplace<Identifier>(*match);
      }
      else
        break;
      auto rhs = pop<Expression>();
      auto lhs = pop<Expression>();
      emplace<MemberExpression>(lhs, rhs);
    }
    return true;
  }

  bool new_expression()
  {
    if (!match("new")) return member_expression();
    if (!new_expression()) syntax_error();
    auto expr = emplace<NewExpression>(pop<Expression>());
    if (arguments()) expr->arguments = pop<Arguments>();
    return true;
  }

  bool arguments()
  {
    if (!match("(")) return false;
    if (argument_list())
      emplace<Arguments>(pop<ArgumentList>());
    else
      emplace<Arguments>();
    if (!match(")")) syntax_error();
    return true;
  }

  bool argument_list()
  {
    if (!assignment_expression()) return false;
    auto list = emplace<ArgumentList>();
    list->push_back(pop<Expression>());
    while (match(",")) {
      if (!assignment_expression()) syntax_error();
      list->push_back(pop<Expression>());
    }
    return true;
  }

  bool left_hand_side_expression()
  {
    if (!new_expression()) return false;
    // if (arguments()) {
    //   replace<CallExpression, Expression, Arguments>();
    //   while (true) {

    //     if (arguments()) {
    //       replace<CallExpression, Expression, Arguments>();
    //       continue;
    //     }
    //     else if (match("[")) {
    //       if (!expression() || !match("]"))
    //         syntax_error();
    //       replace<MemberExpression, Expression, Expression>();
    //       continue;
    //     }
    //     else if (match(".")) {
    //       if (!identifier_name())
    //         syntax_error();
    //       emplace<Identifier>(match);
    //       replace<MemberExpression, Expression, Expression>();
    //       continue;
    //     }
    //     else
    //       break;
    //   }
    // }
    return true;
  }

  bool postfix_expression()
  {
    if (!left_hand_side_expression()) return false;
    if (no_line_terminator_here()) {
      if (match("++") || match("--"))
        emplace<PostfixExpression>(match, pop<Expression>());
    }
    return true;
  }

  bool unary_expression()
  {
    if (match.any_of("delete", "void", "typeof", "++", "--", "+", "-", "~",
                     "!")) {
      auto expr = emplace<UnaryExpression>(*match);
      if (!unary_expression()) syntax_error();
      expr->rhs = pop<Expression>();
      return true;
    }
    return postfix_expression();
  }

  bool multiplicative_expression()
  {
    if (!unary_expression()) return false;
    while (match.any_of("*", "/", "%")) {
      auto expr = emplace<BinaryExpression>(*match, pop<Expression>());
      if (!unary_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool additive_expression()
  {
    if (!multiplicative_expression()) return false;
    while (match.any_of("+", "-")) {
      auto expr = emplace<BinaryExpression>(*match, pop<Expression>());
      if (!multiplicative_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool shift_expression()
  {
    if (!additive_expression()) return false;
    while (match.any_of("<<", ">>", ">>>")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!additive_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool relational_expression()
  {
    if (!shift_expression()) return false;
    while (match.any_of("<", ">", "<=", ">=", "instanceof", "in")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!shift_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  //   bool relational_expression_no_in()
  //   {
  //     if (shift_expression()) {
  //       while (match("<") || match(">") || match("<=") || match(">=") ||
  //              match("instanceof")) {
  //         std::string op = match;
  //         auto lhs = pop();
  //         if (!shift_expression())
  //           syntax_error();
  //         emplace<BinaryExpression>(op, lhs, pop());
  //       }
  //       return true;
  //     }
  //     return false;
  //   }

  bool equality_expression()
  {
    if (!relational_expression()) return false;
    while (match.any_of("==", "!=", "===", "!==")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!relational_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  //   bool equality_expression_no_in()
  //   {
  //     if (relational_expression()) {
  //       while (match("==") || match("!=") || match("===") || match("!==")) {
  //         std::string op = match;
  //         auto lhs = pop();
  //         if (!relational_expression_no_in())
  //           syntax_error();
  //         emplace<BinaryExpression>(op, lhs, pop());
  //       }
  //       return true;
  //     }
  //     return false;
  //   }

  bool bitwise_and_expression()
  {
    if (!equality_expression()) return false;
    while (match("&")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!equality_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  //   bool bitwise_and_expression_no_in()
  //   {
  //     if (equality_expression_no_in()) {
  //       while (match("&")) {
  //         std::string op = match;
  //         auto lhs = pop();
  //         if (!equality_expression_no_in())
  //           syntax_error();
  //         emplace<BinaryExpression>(op, lhs, pop());
  //       }
  //       return true;
  //     }
  //     return false;
  //   }

  bool bitwise_xor_expression()
  {
    if (!bitwise_and_expression()) return false;
    while (match("^")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!bitwise_and_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  //   bool bitwise_xor_expression_no_in()
  //   {
  //     if (bitwise_and_expression_no_in()) {
  //       while (match("^")) {
  //         std::string op = match;
  //         auto lhs = pop();
  //         if (!bitwise_and_expression_no_in())
  //           syntax_error();
  //         emplace<BinaryExpression>(op, lhs, pop());
  //       }
  //       return true;
  //     }
  //     return false;
  //   }

  bool bitwise_or_expression()
  {
    if (!bitwise_xor_expression()) return false;
    while (match("|")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!bitwise_xor_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  //   bool bitwise_or_expression_no_in()
  //   {
  //     if (bitwise_xor_expression_no_in()) {
  //       while (match("|")) {
  //         std::string op = match;
  //         auto lhs = pop();
  //         if (!bitwise_xor_expression_no_in())
  //           syntax_error();
  //         emplace<BinaryExpression>(op, lhs, pop());
  //       }
  //       return true;
  //     }
  //     return false;
  //   }

  bool logical_and_expression()
  {
    if (!bitwise_or_expression()) return false;
    while (match("&&")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!bitwise_or_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  //   bool logical_and_expression_no_in()
  //   {
  //     if (bitwise_or_expression_no_in()) {
  //       while (match("&&")) {
  //         std::string op = match;
  //         auto lhs = pop();
  //         if (!bitwise_or_expression_no_in())
  //           syntax_error();
  //         emplace<BinaryExpression>(op, lhs, pop());
  //       }
  //       return true;
  //     }
  //     return false;
  //   }

  bool logical_or_expression()
  {
    if (!logical_and_expression()) return false;
    while (match("||")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!logical_and_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  //   bool logical_or_expression_no_in()
  //   {
  //     if (logical_and_expression_no_in()) {
  //       while (match("||")) {
  //         std::string op = match;
  //         auto lhs = pop();
  //         if (!logical_and_expression_no_in())
  //           syntax_error();
  //         emplace<BinaryExpression>(op, lhs, pop());
  //       }
  //       return true;
  //     }
  //     return false;
  //   }

  bool conditional_expression()
  {
    if (!logical_or_expression()) return false;
    if (match("?")) {
      if (!assignment_expression() || !match(":") || !assignment_expression())
        syntax_error();
      auto alternate  = pop<Expression>();
      auto consequent = pop<Expression>();
      auto test       = pop<Expression>();
      emplace<ConditionalExpression>(test, consequent, alternate);
    }
    return true;
  }

  //   bool conditional_expression_no_in()
  //   {
  //     if (logical_or_expression_no_in()) {
  //       if (match("?")) {
  //         auto test = pop();
  //         if (!assignment_expression())
  //           syntax_error();
  //         auto consequent = pop();
  //         if (!match(":") && assignment_expression_no_in())
  //           syntax_error();
  //         auto alternate = pop();
  //         emplace<ConditionalExpression>(test, consequent, alternate);
  //       }
  //       return true;
  //     }
  //     return false;
  //   }

  // bool assignment_operator()
  // {
  //   return match([](const auto &token) {
  //     return token.one_of("=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=",
  //                         ">>>=", "&=", "^=", "|=");
  //   });
  // }

  bool assignment_expression()
  {
    if (!conditional_expression()) return false;
    if (match.any_of("=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", ">>>=",
                     "&=", "^=", "|=")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!assignment_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  //   bool assignment_expression_no_in()
  //   {
  //     if (left_hand_side_expression()) {
  //       if (match(is_assignment_operator))
  //         return assignment_expression_no_in() || syntax_error();
  //       return true;
  //     }
  //     return conditional_expression_no_in();
  //   }

  //   bool expression_opt() { return expression(), true; }

  bool expression()
  {
    if (!assignment_expression()) return false;
    while (match(",")) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!assignment_expression()) syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  //   bool expression_no_in_opt() { return expression_no_in(), true; }

  //   bool expression_no_in()
  //   {
  //     if (assignment_expression_no_in()) {
  //       while (match(",") && (assignment_expression_no_in() ||
  //       syntax_error()))
  //         ;
  //       return true;
  //     }
  //     return false;
  //   }

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
    if (statement_list())
      emplace<Block>(pop<StatementList>());
    else
      emplace<Block>();
    if (!match("}")) syntax_error();
    return true;
  }

  bool statement_list()
  {
    auto list = emplace<StatementList>();
    while (statement()) list->push_back(pop<Statement>());
    return true;
  }

  bool variable_statement()
  {
    if (!match("var")) return false;
    if (!variable_declaration_list()) syntax_error();
    emplace<VariableStatement>(pop<VariableDeclarationList>());
    return true;
  }

  bool variable_declaration_list()
  {
    auto list = emplace<VariableDeclarationList>();

    if (variable_declaration()) {
      list->push_back(pop<VariableDeclaration>());
      while (match(",")) {
        if (!variable_declaration()) syntax_error();
        list->push_back(pop<VariableDeclaration>());
      }
    }
    return true;
  }

  //   optional<VariableDeclarationList> variable_declaration_list_no_in()
  //   {
  //     if (auto decl = variable_declaration_no_in()) {
  //       VariableDeclarationList list{*decl};
  //       while (match(",")) {
  //         if (auto decl = variable_declaration_no_in())
  //           list.push_back(*decl);
  //         else
  //           syntax_error();
  //       }
  //       return list;
  //     }
  //     return {};
  //   }

  bool variable_declaration()
  {
    if (!identifier()) return false;
    auto id = pop<Identifier>();
    if (initializer())
      emplace<VariableDeclaration>(id, pop<Expression>());
    else
      emplace<VariableDeclaration>(id);
    return true;
  }

  //   optional<VariableDeclaration> variable_declaration_no_in()
  //   {
  //     auto id = identifier();
  //     if (!id)
  //       return {};
  //     if (initializer_no_in())
  //       return VariableDeclaration{*id, pop_expression()};
  //     else
  //       return VariableDeclaration{*id};
  //   }

  bool initializer()
  {
    return match("=") && (assignment_expression() || syntax_error());
  }

  //   bool initializer_no_in()
  //   {
  //     return match("=") && assignment_expression_no_in();
  //   }

  bool empty_statement()
  {
    if (!match(";")) return false;
    emplace<EmptyStatement>();
    return true;
  }

  bool expression_statement()
  {
    if (match.peek("{") || match.peek("function") || !expression())
      return false;
    if (!automatic_semicolon_insertion()) syntax_error();
    emplace<ExpressionStatement>(pop<Expression>());
    return true;
  }

  bool if_statement()
  {
    if (!match("if")) return false;
    if (!match("(") || !expression() || !match(")") || !statement())
      syntax_error();
    auto stmt = pop<Statement>();
    auto expr = pop<Expression>();
    if (match("else")) {
      if (!statement()) syntax_error();
      emplace<IfStatement>(expr, stmt, pop<Statement>());
    }
    else
      emplace<IfStatement>(expr, stmt);
    return true;
  }

  bool iteration_statement() { return false; }
  //   {
  //     if (match("do")) {
  //       if (statement() && match("while") && match("(") && expression() &&
  //           match(")") && automatic_semicolon_insertion()) {
  //         Statement stmt = pop();
  //         Expression expr = pop_expression();
  //         emplace<DoWhileStatement>(stmt, expr);
  //         return true;
  //       }
  //       else
  //         syntax_error();
  //     }
  //     else if (match("while")) {
  //       if (match("(") && expression() && match(")") && statement()) {
  //         emplace<WhileStatement>(pop_expression(), pop());
  //         return true;
  //       }
  //       else
  //         syntax_error();
  //     }
  //     else if (match("for")) {
  //       // if (expression_no_in_op
  //       //   match("(") &&
  //       //   (
  //       //     match([this]{
  //       //       return expression_no_in_opt() && match(";") &&
  //       expression_opt()
  //       //       && match(";") && expression_opt();
  //       //     }) || match([this]{
  //       //       return match("var") && variable_declaration_list_no_in()
  //       &&
  //       //       match(";") && expression_opt() && match(";") &&
  //       expression_opt();
  //       //     }) || match([this]{
  //       //       return left_hand_side_expression() && match("in") &&
  //       //       expression();
  //       //     }) || match([this]{
  //       //       return match("var") && variable_declaration_no_in() &&
  //       //       match("in") && expression();
  //       //     })
  //       //   ) &&
  //       //   match(")") &&
  //       //   statement()
  //       // ) || syntax_error();
  //       syntax_error();
  //     }
  //     return false;
  //   }

  bool continue_statement()
  {
    if (!match("continue")) return false;
    auto id = (no_line_terminator_here() && identifier()) ? pop<Identifier>()
                                                          : nullptr;
    if (!automatic_semicolon_insertion()) syntax_error();
    emplace<ContinueStatement>(id);
    return true;
  }

  bool break_statement()
  {
    if (!match("break")) return false;
    auto id = (no_line_terminator_here() && identifier()) ? pop<Identifier>()
                                                          : nullptr;

    if (!automatic_semicolon_insertion()) syntax_error("missing ;");
    emplace<BreakStatement>();
    return true;
  }

  bool return_statement()
  {
    if (!match("return")) return false;
    auto expr = (no_line_terminator_here() && expression()) ? pop<Expression>()
                                                            : nullptr;
    if (!automatic_semicolon_insertion()) syntax_error("missing ;");
    emplace<ReturnStatement>(expr);
    return true;
  }

  bool with_statement()
  {
    if (!match("with")) return false;
    if (!match("(") || !expression() || !match(")") || !statement())
      syntax_error();
    auto stmt = pop<Statement>();
    auto expr = pop<Expression>();
    emplace<WithStatement>(expr, stmt);
    return true;
  }

  bool switch_statement()
  {
    if (!match("switch")) return false;
    if (!match("(") || !expression() || !match(")") || !case_block())
      syntax_error();
    auto block = pop<CaseBlock>();
    auto expr  = pop<Expression>();
    emplace<SwitchStatement>(expr, block);
    return true;
  }

  bool case_block()
  {
    if (!match("{")) return false;
    auto list = emplace<CaseBlock>();
    while (case_clause()) list->push_back(pop<CaseClause>());
    if (default_clause()) {
      list->push_back(pop<CaseClause>());
      while (case_clause()) list->push_back(pop<CaseClause>());
    }
    if (!match("}")) syntax_error("missing }");
    return true;
  }

  bool case_clause()
  {
    if (!match("case")) return false;
    if (!expression()) syntax_error();
    if (!match(":")) syntax_error("missing : after case label");
    if (!statement_list()) syntax_error();
    auto list = pop<StatementList>();
    auto test = pop<Expression>();
    emplace<CaseClause>(test, list);
    return true;
  }

  bool default_clause()
  {
    if (!match("default")) return false;
    if (!match(":")) syntax_error("missing : after case label");
    if (!statement_list()) syntax_error();
    emplace<DefaultClause>(pop<StatementList>());
    return true;
  }

  bool labelled_statement()
  {
    if (!identifier()) return false;
    if (!match(":") || !statement()) syntax_error();
    auto stmt  = pop<Statement>();
    auto label = pop<Identifier>();
    emplace<LabelledStatement>(label, stmt);
    return true;
  }

  bool throw_statement()
  {
    if (!match("throw")) return false;
    if (!no_line_terminator_here() || !expression() ||
        !automatic_semicolon_insertion())
      syntax_error();
    emplace<ThrowStatement>(pop<Expression>());
    return true;
  }

  bool try_statement()
  {
    if (!match("try")) return false;
    if (!block()) syntax_error();
    auto stmt                             = emplace<TryStatement>(pop<Block>());
    if (match("catch")) stmt->handler     = pop<Block>();
    if (match("finally")) stmt->finalizer = pop<Block>();
    if (!stmt->handler && !stmt->finalizer) syntax_error();
    return true;
  }

  bool debugger_statement()
  {
    if (!match("debugger")) return false;
    if (!automatic_semicolon_insertion()) syntax_error();
    emplace<DebuggerStatement>();
    return true;
  }

  // A.5
  bool function_declaration()
  {
    if (!match("function")) return false;

    if (!identifier() || !match("(") || !formal_parameter_list() ||
        !match(")") || !match("{") || !function_body() || !match("}"))
      syntax_error();

    auto body   = pop<FunctionBody>();
    auto params = pop<FormalParameterList>();
    auto id     = pop<Identifier>();
    emplace<FunctionDeclaration>(id, params, body);
    return true;
  }

  bool function_expression()
  {
    if (!match("function")) return false;

    auto id = identifier() ? pop<Identifier>() : nullptr;

    if (!match("(") || !formal_parameter_list() || !match(")") || !match("{") ||
        !function_body() || !match("}"))
      syntax_error();

    auto body   = pop<FunctionBody>();
    auto params = pop<FormalParameterList>();
    emplace<FunctionExpression>(id, params, body);
    return true;
  }

  bool formal_parameter_list()
  {
    auto list = emplace<FormalParameterList>();
    while (identifier()) {
      list->push_back(pop<Identifier>());
      if (!match(",")) break;
    }
    return true;
  }

  bool function_body()
  {
    if (source_elements())
      emplace<FunctionBody>(pop<SourceElements>());
    else
      emplace<FunctionBody>();
    return true;
  }

  bool program() { return source_elements(); }

  bool source_elements()
  {
    auto list = emplace<SourceElements>();
    while (source_element()) list->push_back(pop<SourceElement>());
    return true;
  }

  bool source_element() { return statement() || function_declaration(); }

public:
  template <typename... Args>
  Parser(Args &&... args)
      : match(std::forward<Args>(args)...),
        storage(std::make_shared<std::vector<std::unique_ptr<Node>>>())
  {
  }

  Program parse()
  {
    if (!program()) syntax_error();
    return {storage, pop<SourceElements>()};
  }
};

template <typename It>
Parser make_parser(It f, It l)
{
  return {f, l};
}

template <typename Cont>
auto make_parser(Cont &&cont)
{
  using std::begin;
  using std::end;
  return make_parser(begin(cont), end(cont));
}

#endif
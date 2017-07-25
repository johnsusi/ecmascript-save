#include "parser.h"

struct Parser::SyntacticGrammar {
  virtual ~SyntacticGrammar()
  {
  }
  virtual Program parse() = 0;
};

template <class log>
class SyntacticGrammarImpl : public Parser::SyntacticGrammar {
  Lexer* lexer;
  Matcher<Token, std::vector<Token>::const_iterator> match;

  std::shared_ptr<std::vector<std::unique_ptr<Node>>> storage;
  std::vector<Node*>                                  stack;

  [[noreturn]] bool logic_error(std::string what = {})
  {
    // auto debug_info =
    //     match.done() ? nullptr : nullptr; // match.matching()->debug_info;
    // auto loc = debug_info ? debug_info->loc() : "";
    throw SyntaxError("InternalCompilerError: " + what);
    // "InternalCompilerError: " + loc + what
    // + (debug_info ? "\n" + debug_info->syntax_error_at() : ""));
  }

  [[noreturn]] bool syntax_error(std::string what = {})
  {
    // auto debug_info = match.done() ? nullptr : match.matching()->debug_info;
    auto debug_info = match.done() ? nullptr : lexer->lookup(match.matching());
    throw SyntaxError(
        "SyntaxError: " + what //
        + (debug_info
               ? "\n" + debug_info->loc() + "\n" + debug_info->syntax_error_at()
               : "") //
        );
  }

  template <typename T>
  void push(T* value)
  {
    stack.push_back(value);
  }

  template <typename T, typename... Args>
  T* emplace(Args&&... args)
  {
    // log([f = __PRETTY_FUNCTION__](auto& out) { out << f; });

    T* value = new T{std::forward<Args>(args)...};
    storage->emplace_back(value);
    stack.push_back(value);
    return value;
  }

  template <typename T>
  T* pop()
  {
    // log([f = __PRETTY_FUNCTION__](auto& out) { out << f; });
    if (auto value = dynamic_cast<T*>(stack.back())) {
      stack.pop_back();
      return value;
    }
    logic_error("Expected " + demangle<T>() + ", got " + stack.back()->type());
  }

  template <typename T, std::size_t I>
  T* peek_at()
  {
    if (auto value = dynamic_cast<T*>(*(stack.end() - I))) {
      return value;
    }
    logic_error("Expected " + demangle<T>() + ", got " + stack.back()->type());
  }

  template <typename T, typename... Ts, typename... Args>
  T* replace(Args&&... args)
  {
    // log([f = __PRETTY_FUNCTION__](auto& out) { out << f; });
    return index_apply<sizeof...(Ts)>([&](auto... Is) {
      T* value = new T{std::forward<Args>(args)...,
                       this->peek_at<Ts, sizeof...(Ts) - Is>()...};
      stack.resize(stack.size() - sizeof...(Ts));
      storage->emplace_back(value);
      stack.push_back(value);
      return value;
    });
  }

  bool no_line_terminator_here()
  {
    trace();
    return match.peek([](const Token& token) {
      return !token.is_preceded_by_line_terminator();
    });
  }

  // 7.9
  bool automatic_semicolon_insertion()
  {
    trace();
    if (match(";"))
      return true;
    if (no_line_terminator_here() && !match.peek("}"))
      return false; // 7.9.1
    return true;
  }

  // A.1
  bool identifier()
  {
    trace();
    if (!match(&Token::is_identifier))
      return false;
    // emplace<IdentifierExpression>(*match.matched());
    // emplace<Identifier>(match->string_value());
    return true;
  }

  bool identifier_name()
  {
    trace();
    if (!match(&Token::is_identifier_name))
      return false;
    // emplace<IdentifierExpression>(*match);
    // emplace<Identifier>(match->string_value());
    return true;
  }

  bool literal()
  {
    trace();
    return match(&Token::is_literal);
        // null_literal() || boolean_literal() || numeric_literal()
        // || string_literal()
        // || regular_expression_literal());
  }

  // bool null_literal()
  // {
  //   trace();
  //   if (!match(&Token::is_null_literal))
  //     return false;
  //   // emplace<LiteralExpression>(*match);
  //   return true;
  // }

  // bool boolean_literal()
  // {
  //   trace();
  //   if (!match(&Token::is_boolean_literal))
  //     return false;
  //   // emplace<LiteralExpression>(*match);
  //   return true;
  // }

  bool numeric_literal()
  {
    trace();
    if (!match(&Token::is_numeric_literal))
      return false;
    // emplace<LiteralExpression>(*match);
    return true;
  }

  bool string_literal()
  {
    trace();
    if (!match(&Token::is_string_literal))
      return false;
    // emplace<LiteralExpression>(*match.matched());
    return true;
  }

  // bool regular_expression_literal()
  // {
  //   trace();
  //   if (!match(&Token::is_regular_expression_literal))
  //     return false;
  //   // emplace<LiteralExpression>(*match.matched());
  //   return true;
  // }

  // A.3
  bool primary_expression()
  {
    trace();
    if (match("(")) {
      if (!expression() || !match(")"))
        syntax_error();
    }
    else if (match("this")) {
      emplace<ThisExpression>();
    }
    else if (identifier()) {
      emplace<IdentifierExpression>(*match);
    }
    else if (literal()) {
      emplace<LiteralExpression>(*match);
    }
    else if (array_literal()) {
      // replace<ArrayExpression, ArrayLiteral>();
    }
    else if (object_literal()) {
      // replace<ObjectExpression, ObjectLiteral>();
    }
    else
      return false;
    return true;
  }

  bool array_literal()
  {
    trace();
    if (!match("["))
      return false;
    if (!element_list())
      emplace<ElementList>();

    auto expr = replace<ArrayExpression, ElementList>();

    if (elision())
      expr->elision = pop<Elision>();
    if (!match("]"))
      syntax_error("Expected ]");
    return true;
  }

  bool element_list()
  {
    trace();
    ElementList* list;
    if (elision()) {
      list = replace<ElementList, Node>();
      if (assignment_expression())
        list->push_back(pop<Expression>());
    }
    else if (assignment_expression()) {
      list = replace<ElementList, Node>();
    }
    else
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

  bool elision()
  {
    trace();
    if (!match(","))
      return false;
    auto expr = emplace<Elision>();
    while (match(","))
      ++expr->count;
    return true;
  }

  bool object_literal()
  {
    trace();
    if (!match("{"))
      return false;
    if (property_name_and_value_list())
      replace<ObjectExpression, PropertyNameAndValueList>();
    else
      emplace<ObjectExpression>();
    match(",");
    if (!match("}"))
      syntax_error("Expected }");
    return true;
  }

  bool property_name_and_value_list()
  {
    trace();
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

  bool property_assignment()
  {
    trace();
    if (match.lookahead(":")) {
      if (!property_name())
        return false;
      if (!match(":") || !assignment_expression())
        syntax_error();
      replace<PropertyAssignment, PropertyName, Expression>();
      return true;
    }
    else if (match("get")) {
      if (!property_name() || !match("(") || !match(")") || !match("{")
          || !function_body()
          || !match("}"))
        syntax_error("get");
      replace<PropertyAssignment, PropertyName, FunctionBody>();
      return true;
    }
    else if (match("set")) {
      if (!property_name() || !match("(") || !property_set_parameter_list()
          || !match(")")
          || !match("{")
          || !function_body()
          || !match("}"))
        syntax_error("set");
      replace<PropertyAssignment, PropertyName, PropertySetParameterList, FunctionBody>();
      return true;
    }
    else
      return false;
  }

  bool property_name()
  {
    trace();
    if (identifier_name() || string_literal() || numeric_literal()) {
      emplace<PropertyName>(*match);
      return true;
    }
    return false;
  }

  bool property_set_parameter_list()
  {
    trace();
    if (!identifier()) return false;
    emplace<PropertySetParameterList>(*match);
    return true;
  }

  bool member_expression()
  {
    trace();
    if (match("new")) {
      if (!member_expression())
        return false;
      if (!arguments()) {
        // new NewExpression
        replace<NewExpression, Expression>();
        return true;
      }
      replace<NewExpression, Expression, Arguments>();
    }
    else if (!primary_expression() && !function_expression())
      return false;
    while (true) {
      if (match("[")) {
        if (!expression() || !match("]"))
          syntax_error();
        replace<MemberExpression, Expression, Expression>();
      }
      else if (match(".")) {
        if (!identifier_name())
          syntax_error();
        replace<MemberExpression, Expression>(*match);
      }
      else
        break;
    }
    return true;
  }

  bool new_expression()
  {
    trace();
    return member_expression();
  }

  bool call_expression()
  {
    trace();
    if (!arguments())
      return false;
    replace<CallExpression, Expression, Arguments>();
    while (true) {
      if (arguments()) {
        replace<CallExpression, Expression, Arguments>();
      }
      else if (match("[")) {
        if (!expression() || !match("]"))
          syntax_error();
        replace<MemberExpression, Expression, Expression>();
      }
      else if (match(".")) {
        if (!identifier_name())
          syntax_error();
        replace<MemberExpression, Expression>(*match);
      }
      else
        break;
    }
    return true;
  }

  bool arguments()
  {
    trace();
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

  bool argument_list()
  {
    trace();
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

  bool left_hand_side_expression()
  {
    trace();
    if (!new_expression())
      return false;
    call_expression();
    return true;
  }

  static constexpr bool is_postfix_operator(const Token& token)
  {
    switch (token) {
    case Token("++"): return true;
    case Token("--"): return true;
    default: return false;
    }
  }

  bool postfix_expression()
  {
    trace();
    if (!left_hand_side_expression())
      return false;
    if (no_line_terminator_here()) {
      if (match(is_postfix_operator))
        replace<PostfixExpression, Expression>(match);
    }
    return true;
  }

  static constexpr bool is_unary_operator(const Token& token)
  {
    switch (token) {
    case Token("delete"): return true;
    case Token("void"): return true;
    case Token("typeof"): return true;
    case Token("++"): return true;
    case Token("--"): return true;
    case Token("+"): return true;
    case Token("-"): return true;
    case Token("~"): return true;
    case Token("!"): return true;
    default: return false;
    }
  }

  bool unary_expression()
  {
    trace();
    if (match(is_unary_operator)) {
      auto expr = emplace<UnaryExpression>(*match);
      if (!unary_expression())
        syntax_error("Expected UnaryExpression");
      expr->rhs = pop<Expression>();
      return true;
    }
    return postfix_expression();
  }

  static constexpr bool is_multiplicative_operator(const Token& token)
  {
    switch (token) {
    case Token("*"): return true;
    case Token("/"): return true;
    case Token("%"): return true;
    default: return false;
    }
  }

  bool multiplicative_expression()
  {
    trace();
    if (!unary_expression())
      return false;
    while (match(is_multiplicative_operator)) {
      auto expr = replace<BinaryExpression, Expression>(*match);
      if (!unary_expression())
        syntax_error("Expected UnaryExpression");
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool additive_expression()
  {
    trace();
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

  bool shift_expression()
  {
    trace();
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

  static constexpr bool is_relational_operator(const Token& token)
  {
    switch (token) {
    case Token("<"): return true;
    case Token(">"): return true;
    case Token("<="): return true;
    case Token(">="): return true;
    case Token("instanceof"): return true;
    case Token("in"): return true;
    default: return false;
    }
  }

  bool relational_expression()
  {
    trace();
    if (!shift_expression())
      return false;
    while (match(is_relational_operator)) {
      auto expr = replace<BinaryExpression, Expression>(match);
      if (!shift_expression())
        syntax_error("Expected ShiftExpression");
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  static constexpr bool is_relational_operator_no_in(const Token& token)
  {
    switch (token) {
    case Token("<"): return true;
    case Token(">"): return true;
    case Token("<="): return true;
    case Token(">="): return true;
    case Token("instanceof"): return true;
    default: return false;
    }
  }

  bool relational_expression_no_in()
  {
    trace();
    if (!shift_expression())
      return false;
    while (match(is_relational_operator_no_in)) {
      auto expr = replace<BinaryExpression, Expression>(match);
      if (!shift_expression())
        syntax_error("Expected ShiftExpression");
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  static constexpr bool is_equality_operator(const Token& token)
  {
    switch (token) {
    case Token("=="): return true;
    case Token("!="): return true;
    case Token("==="): return true;
    case Token("!=="): return true;
    default: return false;
    }
  }

  bool equality_expression()
  {
    trace();
    if (!relational_expression())
      return false;
    while (match(is_equality_operator)) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!relational_expression())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool equality_expression_no_in()
  {
    trace();
    if (!relational_expression_no_in())
      return false;
    while (match(is_equality_operator)) {
      auto expr = emplace<BinaryExpression>(match, pop<Expression>());
      if (!relational_expression_no_in())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool bitwise_and_expression()
  {
    trace();
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

  bool bitwise_and_expression_no_in()
  {
    trace();
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

  bool bitwise_xor_expression()
  {
    trace();
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

  bool bitwise_xor_expression_no_in()
  {
    trace();
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

  bool bitwise_or_expression()
  {
    trace();
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

  bool bitwise_or_expression_no_in()
  {
    trace();
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

  bool logical_and_expression()
  {
    trace();
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

  bool logical_and_expression_no_in()
  {
    trace();
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

  bool logical_or_expression()
  {
    trace();
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

  bool logical_or_expression_no_in()
  {
    trace();
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

  static constexpr bool is_conditional_operator(const Token& token)
  {
    switch (token) {
    case Token("?"): return true;
    default: return false;
    }
  }

  bool conditional_expression()
  {
    trace();
    if (!logical_or_expression())
      return false;
    if (match(is_conditional_operator)) {
      if (!assignment_expression() || !match(":") || !assignment_expression())
        syntax_error();
      replace<ConditionalExpression, Expression, Expression, Expression>();
    }
    return true;
  }

  bool conditional_expression_no_in()
  {
    trace();
    if (!logical_or_expression_no_in())
      return false;
    if (match(is_conditional_operator)) {
      if (!assignment_expression() || !match(":")
          || !assignment_expression_no_in())
        syntax_error();
      replace<ConditionalExpression, Expression, Expression, Expression>();
    }
    return true;
  }

  static constexpr bool is_assignment_operator(const Token& token)
  {
    switch (token) {
    case Token("="): return true;
    case Token("*="): return true;
    case Token("/="): return true;
    case Token("%="): return true;
    case Token("+="): return true;
    case Token("-="): return true;
    case Token("<<="): return true;
    case Token(">>="): return true;
    case Token(">>>="): return true;
    case Token("&="): return true;
    case Token("^="): return true;
    case Token("|="): return true;
    default: return false;
    }
  }

  bool assignment_expression()
  {
    trace();
    if (!conditional_expression())
      return false;
    if (match(is_assignment_operator)) {
      auto expr = emplace<AssignmentExpression>(match, pop<Expression>());
      if (!assignment_expression())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool assignment_expression_no_in()
  {
    trace();
    if (!conditional_expression_no_in())
      return false;
    if (match(is_assignment_operator)) {
      auto expr = emplace<AssignmentExpression>(match, pop<Expression>());
      if (!assignment_expression_no_in())
        syntax_error();
      expr->rhs = pop<Expression>();
    }
    return true;
  }

  bool expression()
  {
    trace();
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

  bool expression_no_in()
  {
    trace();
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
  bool statement()
  {
    trace();
    return block() || variable_statement() || empty_statement()
           || expression_statement() || if_statement() || iteration_statement()
           || continue_statement() || break_statement() || return_statement()
           || with_statement() || labelled_statement() || switch_statement()
           || throw_statement() || try_statement() || debugger_statement();
  }

  bool block()
  {
    trace();
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

  bool statement_list()
  {
    trace();
    auto list = emplace<StatementList>();
    while (statement())
      list->push_back(pop<Statement>());
    return true;
  }

  bool variable_statement()
  {
    trace();
    if (!match("var"))
      return false;
    if (!variable_declaration_list())
      syntax_error();
    if (!automatic_semicolon_insertion())
      syntax_error("Expected ;");
    emplace<VariableStatement>(pop<VariableDeclarationList>());
    return true;
  }

  bool variable_declaration_list()
  {
    trace();
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

  bool variable_declaration_list_no_in()
  {
    trace();
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

  bool variable_declaration()
  {
    trace();
    if (!identifier())
      return false;
    auto decl = emplace<VariableDeclaration>(*match);
    // auto id = pop<IdentifierExpression>();
    if (initializer())
      decl->initializer = pop<Expression>();
    //   emplace<VariableDeclaration>(id, pop<Expression>());
    // else
    //   emplace<VariableDeclaration>(id);
    return true;
  }

  bool variable_declaration_no_in()
  {
    trace();
    if (!identifier())
      return false;
    auto node = emplace<VariableDeclaration>(*match);
    // auto id = pop<IdentifierExpression>();
    if (initializer_no_in())
      node->initializer = pop<Expression>();
    //   emplace<VariableDeclaration>(id, pop<Expression>());
    // else
    //   emplace<VariableDeclaration>(id);
    return true;
  }

  bool initializer()
  {
    trace();
    return match("=") && (assignment_expression() || syntax_error());
  }

  bool initializer_no_in()
  {
    trace();
    return match("=") && assignment_expression_no_in();
  }

  bool empty_statement()
  {
    trace();
    if (!match(";"))
      return false;
    emplace<EmptyStatement>();
    return true;
  }

  bool expression_statement()
  {
    trace();
    if (match.peek("{") || match.peek("function") || match.lookahead(":")
        || !expression())
      return false;
    if (!automatic_semicolon_insertion())
      syntax_error();
    replace<ExpressionStatement, Expression>();
    return true;
  }

  bool if_statement()
  {
    trace();
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
    }
    else
      emplace<IfStatement>(expr, stmt);
    return true;
  }

  bool iteration_statement()
  {
    trace();
    return do_while_statement() || while_statement() || for_statement()
           || for_in_statement();
  }

  bool do_while_statement()
  {
    trace();
    if (!match("do"))
      return false;
    if (!statement() || !match("while") || !match("(") || !expression()
        || !match(")")
        || !automatic_semicolon_insertion())
      syntax_error();
    replace<DoWhileStatement, Statement, Expression>();
    return true;
  }

  bool while_statement()
  {
    trace();
    if (!match("while"))
      return false;
    if (!match("(") || !expression() || !match(")") || !statement())
      syntax_error();
    replace<WhileStatement, Expression, Statement>();
    return true;
  }

  bool for_statement()
  {
    trace();
    return match([this] {
      if (!match("for"))
        return false;
      if (!match("("))
        syntax_error("Expected (");
      ForStatement* stmt;
      if (match("var") && variable_declaration_list_no_in()) {
        if (!match(";")) {
          pop<VariableDeclarationList>();
          return false;
        }
        stmt = replace<ForStatement, VariableDeclarationList>();
      }
      else {
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

  bool for_in_statement()
  {
    trace();
    return match([this] {
      if (!match("for"))
        return false;
      if (!match("("))
        syntax_error("Expected (");
      ForInStatement* stmt;
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

  bool continue_statement()
  {
    trace();
    if (!match("continue"))
      return false;
    if (no_line_terminator_here() && identifier())
      emplace<ContinueStatement>(*match);
    else
      emplace<ContinueStatement>();
    if (!automatic_semicolon_insertion())
      syntax_error();
    return true;
  }

  bool break_statement()
  {
    trace();
    if (!match("break"))
      return false;
    if (no_line_terminator_here() && identifier())
      emplace<BreakStatement>(*match);
    else
      emplace<BreakStatement>();

    if (!automatic_semicolon_insertion())
      syntax_error("missing ;");

    return true;
  }

  bool return_statement()
  {
    trace();
    if (!match("return"))
      return false;
    auto expr = (no_line_terminator_here() && expression()) ? pop<Expression>()
                                                            : nullptr;
    if (!automatic_semicolon_insertion())
      syntax_error("missing ;");
    emplace<ReturnStatement>(expr);
    return true;
  }

  bool with_statement()
  {
    trace();
    if (!match("with"))
      return false;
    if (!match("(") || !expression() || !match(")") || !statement())
      syntax_error();
    replace<WithStatement, Expression, Statement>();
    return true;
  }

  bool switch_statement()
  {
    trace();
    if (!match("switch"))
      return false;
    if (!match("(") || !expression() || !match(")") || !case_block())
      syntax_error();
    replace<SwitchStatement, Expression, CaseBlock>();
    return true;
  }

  bool case_block()
  {
    trace();
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

  bool case_clause()
  {
    trace();
    if (!match("case"))
      return false;
    if (!expression() || !match(":") || !statement_list())
      syntax_error();
    replace<CaseClause, Expression, StatementList>();
    // auto list = pop<StatementList>();
    // auto test = pop<Expression>();
    // emplace<CaseClause>(test, list);
    return true;
  }

  bool default_clause()
  {
    trace();
    if (!match("default"))
      return false;
    if (!match(":") || !statement_list())
      syntax_error();
    replace<DefaultClause, StatementList>();
    return true;
  }

  bool labelled_statement()
  {
    trace();
    if (!match.lookahead(":") || !identifier())
      return false;
    auto label = *match;
    match(":");
    if (!statement())
      syntax_error();
    auto stmt  = pop<Statement>();
    emplace<LabelledStatement>(label, stmt);
    return true;
  }

  bool throw_statement()
  {
    trace();
    if (!match("throw"))
      return false;
    if (!no_line_terminator_here() || !expression()
        || !automatic_semicolon_insertion())
      syntax_error();
    emplace<ThrowStatement>(pop<Expression>());
    return true;
  }

  bool try_statement()
  {
    trace();
    if (!match("try"))
      return false;
    if (!block())
      syntax_error();
    auto stmt = emplace<TryStatement>(pop<Block>());
    if (match("catch")) {
      if (!match("(") || !identifier()) syntax_error();
      stmt->binding = *match;
      if (!match(")") || !block())
        syntax_error();
      stmt->handler = pop<Block>();
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

  bool debugger_statement()
  {
    trace();
    if (!match("debugger"))
      return false;
    if (!automatic_semicolon_insertion())
      syntax_error();
    emplace<DebuggerStatement>();
    return true;
  }

  // A.5
  bool function_declaration()
  {
    trace();
    if (!match("function"))
      return false;
    if (!identifier()) syntax_error();
    auto id = *match;

    if ( !match("(") || !formal_parameter_list() || !match(")")
        || !match("{")
        || !function_body()
        || !match("}"))
      syntax_error();

    replace<FunctionDeclaration, FormalParameterList, FunctionBody>(id);
    return true;
  }

  bool function_expression()
  {
    trace();
    if (!match("function"))
      return false;
    auto expr = identifier() ? emplace<FunctionExpression>(*match) : emplace<FunctionExpression>();

    if (!match("(") || !formal_parameter_list() || !match(")") || !match("{")
        || !function_body()
        || !match("}"))
      syntax_error();

    expr->body = pop<FunctionBody>();
    expr->params = pop<FormalParameterList>();

    return true;
  }

  bool formal_parameter_list()
  {
    trace();
    auto list = emplace<FormalParameterList>();
    while (identifier()) {
      list->push_back(*match);
      if (!match(","))
        break;
    }
    return true;
  }

  bool function_body()
  {
    trace();
    if (source_elements())
      replace<FunctionBody, SourceElements>();
    else
      emplace<FunctionBody>();
    return true;
  }

  bool program()
  {
    trace();
    if (source_elements())
      replace<ProgramDeclaration, SourceElements>();
    else
      emplace<ProgramDeclaration>();
    return match.done();
  }

  bool source_elements()
  {
    trace();
    auto list = emplace<SourceElements>();
    while (source_element())
      list->push_back(pop<SourceElement>());
    return true;
  }

  bool source_element()
  {
    trace();
    return statement() || function_declaration();
  }

public:
  template <typename It>
  SyntacticGrammarImpl(Lexer* lexer, It f, It l)
      : lexer(lexer),
        match(f, l),
        storage(std::make_shared<std::vector<std::unique_ptr<Node>>>())
  {
  }

  Program parse() final
  {
    trace();
    if (!program())
      syntax_error();
    return {storage, pop<ProgramDeclaration>()};
  }
};

using namespace std;
class StdLogger {
public:
  template <typename Callable>
  constexpr StdLogger(Callable&& cb)
  {
    cb(cout);
    cout << endl;
  }
};

Parser::Parser(Lexer lexer)
    : lexer(std::move(lexer)),
      grammar(new SyntacticGrammarImpl<StdLogger>{&this->lexer, lexer.begin(),
                                                  lexer.end()})
{
}

Program Parser::parse()
{
  return grammar->parse();
}

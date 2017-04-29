#ifndef ECMASCRIPT_LEXER_H
#define ECMASCRIPT_LEXER_H

#include "lexical_grammar.h"

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

std::u16string convert_utf8_to_utf16(const std::string&);

template <typename T>
class BasicLexer
{
  std::unique_ptr< LexicalGrammar<T> > grammar;

  using Iterator = std::u16string::const_iterator;

  struct DebugInfo : Token::DebugInfo
  {
    Iterator s, at;
    int col, row;

    DebugInfo(Iterator s, Iterator at, int col, int row)
      : s(s), at(at), col(col), row(row) {}

    std::string syntax_error_at() const override
    {
      auto it = at;
      while (!is_line_terminator(*it) && it != s)
      {
        --it;
      }
      std::string line1 { it, at };
      std::string line2 (line1.size()-1, '.');
      return line1 + "\n" + line2 + "^\n";
    }
  };

  mutable std::vector<Token> m_tokens;

  static bool reg_exp_allowed(const Token& token);

  std::u16string buffer;

public:

  BasicLexer() {}
  BasicLexer(std::initializer_list<Token> tokens) : m_tokens(tokens) {}

  template <typename It> BasicLexer(It begin, It end)
    : grammar(new LexicalGrammar<T>(begin, end)) {}

  BasicLexer(const std::u16string& str)
    : BasicLexer(str.begin(), str.end()) {}

  BasicLexer(const std::string& str)
    : BasicLexer(convert_utf8_to_utf16(str)) {}

  const std::vector<Token>& tokens() const
  {
    if (!m_tokens.empty() || !grammar) return m_tokens;
    bool lt = false;
    int col = 0, row = 0;
    bool re = false;
    auto s = grammar->match.mark();
    while (auto input_element = re ? grammar->input_element_reg_exp() : grammar->input_element_div())
    {
      if (input_element.is_white_space()) col++;
      else if (input_element.has_line_terminator()) {
        lt = true;
        col = 0;
        row++;
      }
      else if (auto token = input_element.to_token()) {
        token->preceded_by_line_terminator = lt;
        token->debug_info = std::make_shared<DebugInfo>(s, grammar->match.mark(), col, row);
        re = reg_exp_allowed(*token);
        lt = false;
        m_tokens.push_back(std::move(*token));
      }
    }
    return m_tokens;
  }


};

template <typename T>
bool BasicLexer<T>::reg_exp_allowed(const Token& token)
{
  return token.any_of(
    "return", "new", "delete", "throw", "else", "case", "in", "instanceof",
    "typeof", "new", "void", "delete", "+", "-", "!", "~", "&",  "|", "^", "*",
    "/", "%", ">>", "<<", ">>>", "<", ">", "<=", ">=", "==", "===", "!=", "!==",
    "?", "=", "+=", "-=", "/=", "*=", "%=", ">>=", "<<=", ">>>=", "|=", "^=",
    "&=", "&&", "||", "[", "{", "(", ",", ";", ":"
  );
}


using Lexer = BasicLexer<char16_t>;

template <typename It>
auto make_lexer(It f, It l)
{
  return Lexer {f, l};
}

template <typename Cont>
auto make_lexer(Cont&& cont)
{
  return make_lexer(std::begin(cont), std::end(cont));
}

template <typename T>
bool operator==(const BasicLexer<T>& lhs, const BasicLexer<T>& rhs)
{
  return std::equal(lhs.tokens().begin(), lhs.tokens().end(),
                    rhs.tokens().begin(), rhs.tokens().end());
}

template <typename T>
bool operator!=(const BasicLexer<T>& lhs, const BasicLexer<T>& rhs)
{
  return !(lhs == rhs);
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const BasicLexer<T>& lexer)
{
  out << "[";
  auto it = lexer.tokens().begin();
  out << *it++;
  while (it != lexer.tokens().end()) out << ", " << *it++;
  out << "]";
  return out;
}

#endif
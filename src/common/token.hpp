#pragma once
#include <spdlog/fmt/ostr.h>
#include <string>
enum class TokenType : std::uint8_t {
  // Info types
  ILLEGAL = 0x0,
  END_OF_FILE = 0x1,

  // Identifiers and literals
  IDENT = 0x10,
  INTEGER = 0x11,

  // Ops
  ASSIGN = 0x20,
  PLUS = 0x21,
  MINUS = 0x22,
  BANG = 0x23,
  ASTERISK = 0x24,
  SLASH = 0x25,

  LT = 0x26,
  GT = 0x27,
  EQ = 0x28,
  NE = 0x29,

  // Delimiters
  COMMA = 0x30,
  SEMICOLON = 0x31,

  // Syntax
  LPAREN = 0x40,
  RPAREN = 0x41,
  LBRACE = 0x42,
  RBRACE = 0x43,

  // Keywords
  FUNCTION = 0x50,
  LET = 0x51,
  TRUE = 0x52,
  FALSE = 0x53,
  IF = 0x54,
  ELSE = 0x55,
  RETURN = 0x56,
};

TokenType lookupIdentity(std::string identity);
std::string tokenTypeToString(TokenType type);

class Token {
 public:
  const TokenType type;
  const std::string literal;
  Token(TokenType type, char token);
  Token(TokenType type, std::string literal);
  template <typename OStream>
  friend OStream &operator<<(OStream &os, const Token &c) {
    return os << "[token type=" << tokenTypeToString(c.type)
              << ", literal=" << c.literal << "]";
  }
};

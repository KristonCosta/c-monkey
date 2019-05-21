#pragma once
#include <string>

enum class TokenType : std::uint8_t {
  // Info types
  ILLEGAL = 0x0,
  END_OF_FILE = 0x1,

  // Identifiers and literals
  IDENT = 0x2,
  INTEGER = 0x3,

  // Ops
  ASSIGN = 0x4,
  PLUS = 0x5,

  // Delimiters
  COMMA = 0x6,
  SEMICOLON = 0x7,

  // Syntax
  LPAREN = 0x8,
  RPAREN = 0x9,
  LBRACE = 0xA,
  RBRACE = 0xB,
  FUNCTION = 0xC,
  LET = 0xD,
};

class Token {
 public:
  const TokenType type;
  const std::string literal;
  Token(TokenType type, char token);
  Token(TokenType type, std::string literal);
  std::string toString();
};

TokenType lookupIdentity(std::string identity);
std::string tokenTypeToString(TokenType type);
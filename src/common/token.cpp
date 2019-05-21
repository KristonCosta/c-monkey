#include "token.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <map>

Token::Token(TokenType type, char token)
    : type(type), literal(std::string(1, token)) {}

Token::Token(TokenType type, std::string literal)
    : type(type), literal(std::move(literal)) {}

std::map<std::string, TokenType> keywords = {
    {"fn", TokenType::FUNCTION},
    {"let", TokenType::LET},
};

TokenType lookupIdentity(std::string identity) {
  auto lookup = keywords.find(identity);
  if (lookup != keywords.end()) {
    return lookup->second;
  }
  return TokenType::IDENT;
}

std::string tokenTypeToString(TokenType type) {
  switch (type) {
    case TokenType::ILLEGAL:
      return "ILLEGAL";
    case TokenType::END_OF_FILE:
      return "END_OF_FILE";
    case TokenType::IDENT:
      return "IDENT";
    case TokenType::INTEGER:
      return "INTEGER";
    case TokenType::ASSIGN:
      return "ASSIGN";
    case TokenType::PLUS:
      return "PLUS";
    case TokenType::COMMA:
      return "COMMA";
    case TokenType::SEMICOLON:
      return "SEMICOLON";
    case TokenType::LPAREN:
      return "LPAREN";
    case TokenType::RPAREN:
      return "RPAREN";
    case TokenType::LBRACE:
      return "LBRACE";
    case TokenType::RBRACE:
      return "RBRACE";
    case TokenType::FUNCTION:
      return "FUNCTION";
    case TokenType::LET:
      return "LET";
  }
  throw "Token type doesn't exist!";
}

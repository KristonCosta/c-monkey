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

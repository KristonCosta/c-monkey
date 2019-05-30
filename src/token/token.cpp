#include "token.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <map>

Token::Token(std::unique_ptr<Location> location, TokenType type, char token)
    : location(std::move(location)),
      type(type),
      literal(std::string(1, token)) {}

Token::Token(std::unique_ptr<Location> location, TokenType type,
             std::string literal)
    : location(std::move(location)), type(type), literal(std::move(literal)) {}

std::map<std::string, TokenType> keywords = {
    {"fn", TokenType::FUNCTION},   {"let", TokenType::LET},
    {"true", TokenType::TRUE},     {"false", TokenType::FALSE},
    {"if", TokenType::IF},         {"else", TokenType::ELSE},
    {"return", TokenType::RETURN},
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
    case TokenType::BANG:
      return "BANG";
    case TokenType::MINUS:
      return "MINUS";
    case TokenType::ASTERISK:
      return "ASTERISK";
    case TokenType::SLASH:
      return "SLASH";
    case TokenType::LT:
      return "LT";
    case TokenType::GT:
      return "GT";
    case TokenType::TRUE:
      return "TRUE";
    case TokenType::FALSE:
      return "FALSE";
    case TokenType::IF:
      return "IF";
    case TokenType::ELSE:
      return "ELSE";
    case TokenType::RETURN:
      return "RETURN";
    case TokenType::NE:
      return "NE";
    case TokenType::EQ:
      return "EQ";
    case TokenType::STRING:
      return "STRING";
  }
  throw "Token type doesn't exist!";
}

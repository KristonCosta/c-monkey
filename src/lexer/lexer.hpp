#pragma once
#include <string>
#include <token.hpp>

class Lexer {
  std::string input;
  std::uint64_t position;
  std::uint64_t readPosition;
  char tok;

 private:
  void readChar();
  void skipWhitespace();
  std::string readIdentifier();

 public:
  Lexer(std::string input);
  static std::unique_ptr<Lexer> from(std::string input);
  std::unique_ptr<Token> nextToken();
};

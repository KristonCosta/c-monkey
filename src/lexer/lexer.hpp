#pragma once
#include <functional>
#include <string>
#include <token.hpp>

const std::string LEXER_LOGGER = "lexer";

class Lexer {
  std::string input;
  std::uint64_t position;
  std::uint64_t readPosition;
  std::uint64_t currentLine;
  std::uint64_t columnOffset;
  char tok;

 private:
  void readChar();
  void skipWhitespace();
  char peek();
  std::string readIdentifier();
  std::string readNumber();
  std::unique_ptr<Location> getLocation(std::uint64_t startPosition);
  std::string extactWhile(std::function<bool(char)> condition);

 public:
  Lexer(std::string input);
  std::shared_ptr<Token> nextToken();
};

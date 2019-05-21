#include "lexer.hpp"
#include <catch2/catch.hpp>
#include <cctype>
#include <functional>
#include <iostream>

Lexer::Lexer(std::string input)
    : input(std::move(input)), position(0), readPosition(0), tok('\0'){};

void Lexer::readChar() {
  if (this->readPosition >= this->input.size()) {
    this->tok = '\0';
  } else {
    this->tok = this->input[this->readPosition];
  }
  this->position = this->readPosition;
  this->readPosition += 1;
  std::cout << "Reading character: '" << this->tok << "'" << std::endl;
  /*
    std::cout << "Read position: " << this->readPosition << std::endl;
    std::cout << "Position: " << this->position << std::endl;
    std::cout << "tok: " << this->tok << std::endl;
    */
}

std::unique_ptr<Token> Lexer::nextToken() {
  TokenType type = TokenType::ILLEGAL;
  std::cout << "Getting next token " << this->tok << std::endl;
  this->skipWhitespace();
  switch (this->tok) {
    case '=':
      type = TokenType::ASSIGN;
      break;
    case ';':
      type = TokenType::SEMICOLON;
      break;
    case '(':
      type = TokenType::LPAREN;
      break;
    case ')':
      type = TokenType::RPAREN;
      break;
    case ',':
      type = TokenType::COMMA;
      break;
    case '+':
      type = TokenType::PLUS;
      break;
    case '{':
      type = TokenType::LBRACE;
      break;
    case '}':
      type = TokenType::RBRACE;
      break;
    case '\0':
      type = TokenType::END_OF_FILE;
      break;
  }

  if (type == TokenType::ILLEGAL) {
    if (isalpha(this->tok)) {
      auto identifier = this->readIdentifier();
      std::cout << "Identifier '" << identifier << "'" << std::endl;
      return std::make_unique<Token>(lookupIdentity(identifier), identifier);
    } else if (isdigit(this->tok)) {
      return std::make_unique<Token>(TokenType::INTEGER, this->readNumber());
    }
  }
  std::cout << "Token type " << tokenTypeToString(type) << std::endl;
  auto token = std::make_unique<Token>(type, this->tok);

  this->readChar();
  return token;
}

std::unique_ptr<Lexer> Lexer::from(std::string input) {
  auto lexer = std::make_unique<Lexer>(input);
  std::cout << input << std::endl;
  lexer->readChar();
  return lexer;
}

std::string Lexer::readIdentifier() {
  std::cout << "Reading identifier" << std::endl;
  auto sub = this->extactWhile([](char tok) { return isalpha(tok); });
  std::cout << "Found identifier " << sub << std::endl;
  return std::move(sub);
}

std::string Lexer::readNumber() {
  std::cout << "Reading number" << std::endl;
  auto sub = this->extactWhile([](char tok) { return isdigit(tok); });
  std::cout << "Found number " << sub << std::endl;
  return std::move(sub);
}

std::string Lexer::extactWhile(std::function<bool(char)> condition) {
  auto position = this->position;
  while (condition(this->tok)) {
    this->readChar();
  }
  return std::move(this->input.substr(position, this->position - position));
}

void Lexer::skipWhitespace() {
  while (isspace(this->tok)) {
    std::cout << "Skipping whitespace." << std::endl;
    this->readChar();
  }
}

char Lexer::peek() {
  if (this->readPosition >= this->input.size()) {
    return '\0';
  } else {
    return this->input[this->readPosition];
  }
}

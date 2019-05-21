#include "lexer.hpp"
#include <catch2/catch.hpp>
#include <cctype>
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
  /*
    std::cout << "Read position: " << this->readPosition << std::endl;
    std::cout << "Position: " << this->position << std::endl;
    std::cout << "tok: " << this->tok << std::endl;
    */
}

std::unique_ptr<Token> Lexer::nextToken() {
  TokenType type = TokenType::ILLEGAL;
  std::cout << "Getting next token" << std::endl;
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
  std::unique_ptr<Token> token;
  if (type == TokenType::ILLEGAL && isalpha(this->tok)) {
    auto identifier = this->readIdentifier();
    std::cout << "Identifier " << identifier << std::endl;

    token = std::make_unique<Token>(lookupIdentity(identifier), identifier);
  } else {
    token = std::make_unique<Token>(type, this->tok);
  }
  this->readChar();
  return token;
}

std::unique_ptr<Lexer> Lexer::from(std::string input) {
  auto lexer = std::make_unique<Lexer>(input);
  lexer->readChar();
  return lexer;
}

std::string Lexer::readIdentifier() {
  std::cout << "Reading identifier" << std::endl;
  auto position = this->position;
  while (isalpha(this->tok) && !isspace(this->tok)) {
    std::cout << this->tok << " is alpha" << std::endl;
    this->readChar();
  }
  std::cout << "Identifier at " << position << " ," << this->position
            << std::endl;
  auto sub = this->input.substr(position, this->position);
  std::cout << "Found identifier " << sub << std::endl;
  return std::move(sub);
}

void Lexer::skipWhitespace() {
  while (isspace(this->tok)) {
    this->readChar();
  }
}

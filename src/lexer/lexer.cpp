#include "lexer.hpp"
#include "spdlog/sinks/null_sink.h"

#include <spdlog/spdlog.h>
#include <catch2/catch.hpp>
#include <cctype>
#include <functional>
#include <iostream>

Lexer::Lexer(std::string input)
    : input(std::move(input)), position(0), readPosition(0), tok('\0') {
  if (!spdlog::get(LEXER_LOGGER)) {
    spdlog::create<spdlog::sinks::null_sink_st>(LEXER_LOGGER);
  }
};

void Lexer::readChar() {
  if (this->readPosition >= this->input.size()) {
    this->tok = '\0';
  } else {
    this->tok = this->input[this->readPosition];
  }
  this->position = this->readPosition;
  this->readPosition += 1;
  spdlog::get(LEXER_LOGGER)->info("Reading character: '{}'", this->tok);
}

std::unique_ptr<Token> Lexer::nextToken() {
  spdlog::get(LEXER_LOGGER)->info("Getting next token '{}'", this->tok);
  this->skipWhitespace();

  TokenType type = TokenType::ILLEGAL;
  std::string literal = std::string(1, this->tok);
  switch (this->tok) {
    case '=':
      if (this->peek() == '=') {
        this->readChar();
        literal = "==";
        type = TokenType::EQ;
      } else {
        type = TokenType::ASSIGN;
      }
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
    case '-':
      type = TokenType::MINUS;
      break;
    case '!':
      if (this->peek() == '=') {
        this->readChar();
        literal = "!=";
        type = TokenType::NE;
      } else {
        type = TokenType::BANG;
      }
      break;
    case '*':
      type = TokenType::ASTERISK;
      break;
    case '/':
      type = TokenType::SLASH;
      break;
    case '<':
      type = TokenType::LT;
      break;
    case '>':
      type = TokenType::GT;
      break;
    case '\0':
      type = TokenType::END_OF_FILE;
      break;
  }

  if (type == TokenType::ILLEGAL) {
    if (isalpha(this->tok)) {
      auto identifier = this->readIdentifier();
      return std::make_unique<Token>(lookupIdentity(identifier), identifier);
    } else if (isdigit(this->tok)) {
      auto number = this->readNumber();
      return std::make_unique<Token>(TokenType::INTEGER, number);
    }
  }
  spdlog::get(LEXER_LOGGER)->info("Token type '{}'", tokenTypeToString(type));
  auto token = std::make_unique<Token>(type, literal);
  this->readChar();
  return token;
}

std::unique_ptr<Lexer> Lexer::from(std::string input) {
  auto lexer = std::make_unique<Lexer>(input);
  lexer->readChar();
  return lexer;
}

std::string Lexer::readIdentifier() {
  auto sub = this->extactWhile([](char tok) { return isalpha(tok); });
  spdlog::get(LEXER_LOGGER)->info("Found identifier '{}'", sub);
  return std::move(sub);
}

std::string Lexer::readNumber() {
  auto sub = this->extactWhile([](char tok) { return isdigit(tok); });
  spdlog::get(LEXER_LOGGER)->info("Found number '{}'", sub);
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

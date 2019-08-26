#include "lexer.hpp"
#include "spdlog/sinks/null_sink.h"

#include <spdlog/spdlog.h>
#include <cctype>
#include <functional>
#include <utility>


Lexer::Lexer(std::string  input)
    : input(std::move(input)),
      position(0),
      readPosition(0),
      currentLine(1),
      columnOffset(0),
      tok('\0') {
  if (!spdlog::get(LEXER_LOGGER)) {
    spdlog::create<spdlog::sinks::null_sink_st>(LEXER_LOGGER);
  }
  this->readChar();
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

void Lexer::skipWhitespace() {
  while (isspace(this->tok)) {
    auto token = this->tok;
    this->readChar();
    if (token == '\n') {
      this->currentLine++;
      this->columnOffset = this->position;
    }
  }
}

void Lexer::skipLine() {
  while (this->tok != '\n' && this->tok != '\0') {
    this->readChar();
  }
  this->skipWhitespace();
}

char Lexer::peek() {
  if (this->readPosition >= this->input.size()) {
    return '\0';
  } else {
    return this->input[this->readPosition];
  }
}

std::string Lexer::readIdentifier() {
  auto sub = this->extactWhile(
      [](char token) { return isalpha(token) || token == '_' || isdigit(token); });
  spdlog::get(LEXER_LOGGER)->info("Found identifier '{}'", sub);
  return sub;
}

std::string Lexer::readNumber() {
  auto sub = this->extactWhile([](char token) { return isdigit(token); });
  spdlog::get(LEXER_LOGGER)->info("Found number '{}'", sub);
  return sub;
}

std::string Lexer::readString() {
  auto sub =
      this->extactWhile([](char token) { return (token != '\0' && token != '"'); });
  spdlog::get(LEXER_LOGGER)->info("Found string '{}'", sub);
  return sub;
}

std::shared_ptr<Token> Lexer::nextToken() {
  spdlog::get(LEXER_LOGGER)->info("Getting next token '{}'", this->tok);
  this->skipWhitespace();

  TokenType type = TokenType::ILLEGAL;
  std::string literal = std::string(1, this->tok);
  auto pos = this->position;

  while (this->tok == '/' && this->peek() == '/') {
    spdlog::get(LEXER_LOGGER)
        ->info("Found comment skipping line '{}'", this->tok);
    this->skipLine();
  }

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
    case ':':
      type = TokenType::COLON;
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
    case '[':
      type = TokenType::LBRACKET;
      break;
    case ']':
      type = TokenType::RBRACKET;
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
    case '"':
      this->readChar();
      literal = this->readString();
      type = TokenType::STRING;
      break;
    case '\0':
      type = TokenType::END_OF_FILE;
      break;
  }

  if (type == TokenType::ILLEGAL) {
    if (isalpha(this->tok)) {
      auto identifier = this->readIdentifier();
      return std::make_shared<Token>(this->getLocation(pos),
                                     lookupIdentity(identifier), identifier);
    } else if (isdigit(this->tok)) {
      auto number = this->readNumber();
      return std::make_shared<Token>(this->getLocation(pos),
                                     TokenType::INTEGER, number);
    }
  }
  spdlog::get(LEXER_LOGGER)->info("Token type '{}'", tokenTypeToString(type));
  auto token =
      std::make_shared<Token>(this->getLocation(pos), type, literal);
  this->readChar();
  return token;
}

std::string Lexer::extactWhile(const std::function<bool(char)>& condition) {
  auto pos = this->position;
  while (condition(this->tok)) {
    this->readChar();
  }
  return this->input.substr(pos, this->position - pos);
}

std::unique_ptr<Location> Lexer::getLocation(std::uint64_t startPosition) {
  return std::make_unique<Location>(this->currentLine,
                                    startPosition - this->columnOffset);
}

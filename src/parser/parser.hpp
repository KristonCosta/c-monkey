#pragma once
#include "spdlog/sinks/null_sink.h"

#include <spdlog/spdlog.h>
#include <let_statement.hpp>
#include <lexer.hpp>
#include <list>
#include <parser_errors.hpp>
#include <program.hpp>

const std::string PARSER_LOGGER = "parser";

class Parser {
 private:
  std::unique_ptr<Lexer> lexer;
  std::shared_ptr<Token> currentToken;
  std::shared_ptr<Token> peekToken;
  std::list<std::shared_ptr<ParserError>> _errors;

  void nextToken();

  std::shared_ptr<Statement> parseStatement();
  std::shared_ptr<Statement> parseLetStatement();

  void addError(std::shared_ptr<Token> token, std::string message) {
    this->_errors.push_back(std::make_shared<ParserError>(token, message));
  }

  bool expectPeek(TokenType type);
  bool currentTokenIs(TokenType type) const;
  bool peekTokenIs(TokenType type) const;

 public:
  Parser(std::unique_ptr<Lexer> lexer) : lexer(std::move(lexer)) {
    if (!spdlog::get(PARSER_LOGGER)) {
      spdlog::create<spdlog::sinks::null_sink_st>(PARSER_LOGGER);
    }
  }
  static std::unique_ptr<Parser> from(std::unique_ptr<Lexer> lexer) {
    auto parser = std::make_unique<Parser>(std::move(lexer));
    parser->nextToken();
    parser->nextToken();
    return std::move(parser);
  }
  std::unique_ptr<Program> parseProgram();

  const std::list<std::shared_ptr<ParserError>>& errors() {
    return this->_errors;
  }
};
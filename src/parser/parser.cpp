#include <spdlog/spdlog.h>
#include <iostream>
#include <parser.hpp>

void Parser::nextToken() {
  this->currentToken = this->peekToken;
  this->peekToken = this->lexer->nextToken();
}

std::unique_ptr<Program> Parser::parseProgram() {
  auto program = std::make_unique<Program>();
  while (this->currentToken->type != TokenType::END_OF_FILE) {
    spdlog::get(PARSER_LOGGER)->info("Current token {}", *this->currentToken);

    auto stmt = this->parseStatement();

    spdlog::get(PARSER_LOGGER)->info("Adding statement {}", stmt->toString());
    if (stmt) {
      program->addStatement(stmt);
    }
    this->nextToken();
  };
  return std::move(program);
}

std::shared_ptr<Statement> Parser::parseStatement() {
  switch (this->currentToken->type) {
    case TokenType::LET:
      return this->parseLetStatement();
    default:
      return nullptr;
  }
}

std::shared_ptr<Statement> Parser::parseLetStatement() {
  auto stmt = std::make_shared<LetStatement>(this->currentToken);

  if (!this->expectPeek(TokenType::IDENT)) {
    return nullptr;
  }
  spdlog::get(PARSER_LOGGER)
      ->info("Current token (ident) {}", *this->currentToken);
  stmt->setName(std::make_shared<Identifier>(this->currentToken,
                                             this->currentToken->literal));
  if (!this->expectPeek(TokenType::ASSIGN)) {
    return nullptr;
  }
  spdlog::get(PARSER_LOGGER)
      ->info("Current token (assign) {}", *this->currentToken);
  while (!this->currentTokenIs(TokenType::SEMICOLON)) {
    this->nextToken();
  }
  return std::dynamic_pointer_cast<Statement>(stmt);
}

bool Parser::currentTokenIs(TokenType type) const {
  return this->currentToken->type == type;
}
bool Parser::peekTokenIs(TokenType type) const {
  return this->peekToken->type == type;
}
bool Parser::expectPeek(TokenType type) {
  if (this->peekTokenIs(type)) {
    this->nextToken();
    return true;
  } else {
    return false;
  }
}
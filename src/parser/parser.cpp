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

    if (stmt) {
      spdlog::get(PARSER_LOGGER)->info("Adding statement {}", stmt->toString());
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
    case TokenType::RETURN:
      return this->parseReturnStatement();
    default:
      return this->parseExpressionStatement();
  }
}

std::shared_ptr<Statement> Parser::parseExpressionStatement() {
  auto tok = this->currentToken;
  auto stmt = std::make_shared<ExpressionStatement>(
      tok, this->parseExpression(Precedence::BOTTOM));
  while (!this->currentTokenIs(TokenType::SEMICOLON)) {
    this->nextToken();
  };
  return std::dynamic_pointer_cast<Statement>(stmt);
}

std::shared_ptr<Expression> Parser::parseExpression(Precedence prec) {
  auto prefixFnPair = this->prefixParseFunctions.find(this->currentToken->type);
  if (prefixFnPair == this->prefixParseFunctions.end()) {
    this->addError(this->currentToken,
                   fmt::format("No prefix expression found for {}",
                               this->currentToken->literal));
    return nullptr;
  }
  auto fn = prefixFnPair->second;
  return (this->*fn)();
}

std::shared_ptr<Expression> Parser::parsePrefixExpression() {
  auto tok = this->currentToken;
  this->nextToken();
  auto right = this->parseExpression(Precedence::PREFIX);
  auto expr = std::make_shared<PrefixExpression>(tok, right, tok->literal);
  return std::dynamic_pointer_cast<Expression>(expr);
}

std::shared_ptr<Expression> Parser::parseIdentifier() {
  auto expr = std::make_shared<Identifier>(this->currentToken,
                                           this->currentToken->literal);
  return std::dynamic_pointer_cast<Expression>(expr);
}

std::shared_ptr<Expression> Parser::parseIntegerLiteral() {
  int value;
  try {
    value = std::stoi(this->currentToken->literal);
  } catch (std::invalid_argument const &e) {
    this->addError(this->currentToken, "Token value was not an integer");
    return nullptr;
  } catch (std::out_of_range const &e) {
    this->addError(this->currentToken, "Integer value out of range");
    return nullptr;
  }
  auto expr = std::make_shared<IntegerLiteral>(this->currentToken, value);
  return std::dynamic_pointer_cast<Expression>(expr);
}

std::shared_ptr<Statement> Parser::parseReturnStatement() {
  auto stmt = std::make_shared<ReturnStatement>(this->currentToken);
  this->nextToken();
  while (!this->currentTokenIs(TokenType::SEMICOLON)) {
    this->nextToken();
  };
  return std::dynamic_pointer_cast<Statement>(stmt);
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
    std::cout << "Adding error" << std::endl;
    this->addError(
        this->peekToken,
        fmt::format("Expected {} but found {}", tokenTypeToString(type),
                    tokenTypeToString(this->peekToken->type)));
    std::cout << "Added error" << std::endl;
    return false;
  }
}
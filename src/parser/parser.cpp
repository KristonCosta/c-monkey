#include <spdlog/spdlog.h>
#include <iostream>
#include <parser.hpp>

std::map<TokenType, Precedence> precedences = {
    {TokenType::EQ, Precedence::EQUALS},
    {TokenType::NE, Precedence::EQUALS},
    {TokenType::LT, Precedence::LESSGREATER},
    {TokenType::GT, Precedence::LESSGREATER},
    {TokenType::PLUS, Precedence::SUM},
    {TokenType::MINUS, Precedence::SUM},
    {TokenType::SLASH, Precedence::PRODUCT},
    {TokenType::ASTERISK, Precedence::PRODUCT}};

std::string precedenceToString(Precedence prec) {
  switch (prec) {
    case Precedence::BOTTOM:
      return "BOTTOM";
    case Precedence::EQUALS:
      return "EQUALS";
    case Precedence::LESSGREATER:
      return "LESSGREATER";
    case Precedence::SUM:
      return "SUM";
    case Precedence::PRODUCT:
      return "PRODUCT";
    case Precedence::PREFIX:
      return "PREFIX";
    case Precedence::CALL:
      return "CALL";
  }
  return "UNKNOWN";
}

void Parser::nextToken() {
  this->currentToken = this->peekToken;
  this->peekToken = this->lexer->nextToken();
}

Precedence lookupPrecedence(TokenType type) {
  auto pair = precedences.find(type);
  if (pair == precedences.end()) {
    return Precedence::BOTTOM;
  }
  return pair->second;
}

Precedence Parser::peekPrecedence() {
  return lookupPrecedence(this->peekToken->type);
}

Precedence Parser::currentPrecedence() {
  return lookupPrecedence(this->currentToken->type);
}

std::unique_ptr<AST::Program> Parser::parseProgram() {
  auto program = std::make_unique<AST::Program>();
  while (this->currentToken->type != TokenType::END_OF_FILE) {
    spdlog::get(PARSER_LOGGER)->info("Current token {}", *this->currentToken);

    auto stmt = this->parseStatement();

    if (stmt) {
      spdlog::get(PARSER_LOGGER)
          ->info("Adding statement {}", stmt->toDebugString());
      program->addStatement(stmt);
    }
    this->nextToken();
  };
  return std::move(program);
}

std::shared_ptr<AST::Statement> Parser::parseStatement() {
  switch (this->currentToken->type) {
    case TokenType::LET:
      return this->parseLetStatement();
    case TokenType::RETURN:
      return this->parseReturnStatement();
    default:
      return this->parseExpressionStatement();
  }
}

std::shared_ptr<AST::Statement> Parser::parseExpressionStatement() {
  auto tok = this->currentToken;
  auto stmt = std::make_shared<AST::ExpressionStatement>(
      tok, this->parseExpression(Precedence::BOTTOM));
  if (this->peekTokenIs(TokenType::SEMICOLON)) {
    this->nextToken();
  };
  return std::dynamic_pointer_cast<AST::Statement>(stmt);
}

std::shared_ptr<AST::Expression> Parser::parseExpression(Precedence prec) {
  auto prefixFnPair = this->prefixParseFunctions.find(this->currentToken->type);
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing expression {}", *this->currentToken);
  if (prefixFnPair == this->prefixParseFunctions.end()) {
    this->addError(this->currentToken,
                   fmt::format("No prefix expression found for {}",
                               this->currentToken->literal));
    return nullptr;
  }
  auto fn = prefixFnPair->second;
  auto left = (this->*fn)();

  while (!this->peekTokenIs(TokenType::SEMICOLON) &&
         prec < this->peekPrecedence()) {
    spdlog::get(PARSER_LOGGER)
        ->info("Finding right expression {}", *this->peekToken);
    auto infixFnPair = this->infixParseFunctions.find(this->peekToken->type);
    if (infixFnPair == this->infixParseFunctions.end()) {
      spdlog::get(PARSER_LOGGER)
          ->info("Didn't find infix function {}", *this->peekToken);
      return left;
    }
    this->nextToken();
    spdlog::get(PARSER_LOGGER)
        ->info("Found infix function {}", *this->currentToken);
    auto infixFn = infixFnPair->second;
    left = (this->*infixFn)(left);
  };
  return left;
}

std::shared_ptr<AST::Expression> Parser::parsePrefixExpression() {
  auto tok = this->currentToken;
  this->nextToken();
  auto right = this->parseExpression(Precedence::PREFIX);
  auto expr = std::make_shared<AST::PrefixExpression>(tok, right, tok->literal);
  return std::dynamic_pointer_cast<AST::Expression>(expr);
}

std::shared_ptr<AST::Expression> Parser::parseInfixExpression(
    std::shared_ptr<AST::Expression> left) {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing infix for {} with prec {}", *this->currentToken,
             precedenceToString(this->currentPrecedence()));
  auto tok = this->currentToken;
  auto prec = this->currentPrecedence();
  this->nextToken();
  auto right = this->parseExpression(prec);
  auto expr =
      std::make_shared<AST::InfixExpression>(tok, left, right, tok->literal);
  spdlog::get(PARSER_LOGGER)->info("Returning infix for {}", *tok);
  return std::dynamic_pointer_cast<AST::Expression>(expr);
}

std::shared_ptr<AST::Expression> Parser::parseIdentifier() {
  auto expr = std::make_shared<AST::Identifier>(this->currentToken,
                                                this->currentToken->literal);
  return std::dynamic_pointer_cast<AST::Expression>(expr);
}

std::shared_ptr<AST::Expression> Parser::parseIntegerLiteral() {
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
  auto expr = std::make_shared<AST::IntegerLiteral>(this->currentToken, value);
  return std::dynamic_pointer_cast<AST::Expression>(expr);
}

std::shared_ptr<AST::Statement> Parser::parseReturnStatement() {
  auto stmt = std::make_shared<AST::ReturnStatement>(this->currentToken);
  this->nextToken();
  while (!this->currentTokenIs(TokenType::SEMICOLON)) {
    this->nextToken();
  };
  return std::dynamic_pointer_cast<AST::Statement>(stmt);
}

std::shared_ptr<AST::Statement> Parser::parseLetStatement() {
  auto stmt = std::make_shared<AST::LetStatement>(this->currentToken);

  if (!this->expectPeek(TokenType::IDENT)) {
    return nullptr;
  }
  spdlog::get(PARSER_LOGGER)
      ->info("Current token (ident) {}", *this->currentToken);
  stmt->setName(std::make_shared<AST::Identifier>(this->currentToken,
                                                  this->currentToken->literal));
  if (!this->expectPeek(TokenType::ASSIGN)) {
    return nullptr;
  }
  spdlog::get(PARSER_LOGGER)
      ->info("Current token (assign) {}", *this->currentToken);
  while (!this->currentTokenIs(TokenType::SEMICOLON)) {
    this->nextToken();
  }
  return std::dynamic_pointer_cast<AST::Statement>(stmt);
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
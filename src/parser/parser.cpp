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
    {TokenType::ASTERISK, Precedence::PRODUCT},
    {TokenType::LPAREN, Precedence::CALL},
    {TokenType::LBRACKET, Precedence::INDEX}};

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
    case Precedence::INDEX:
      return "INDEX";
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
    } else {
      spdlog::get(PARSER_LOGGER)
          ->info("Null statement found for {}", this->currentToken);
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

std::shared_ptr<AST::Expression> Parser::parseArrayLiteral() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing array literal for {} ", *this->currentToken);

  auto tok = this->currentToken;
  std::vector<std::shared_ptr<AST::Expression>> args;
  auto foundElements =
      this->parseParameters(std::back_inserter(args), TokenType::RBRACKET);
  if (!foundElements) {
    return nullptr;
  }
  auto array = std::make_shared<AST::ArrayLiteral>(tok);
  for (auto arg : args) {
    array->addValue(arg);
  };
  return array;
}

std::shared_ptr<AST::Expression> Parser::parseHashLiteral() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing hash literal for {} ", *this->currentToken);
  auto hash = std::make_shared<AST::HashLiteral>(this->currentToken);

  while (!this->peekTokenIs(TokenType::RBRACE)) {
    this->nextToken();
    auto key = this->parseExpression(Precedence::BOTTOM);
    if (!this->expectPeek(TokenType::COLON)) {
      return nullptr;
    }
    this->nextToken();

    auto value = this->parseExpression(Precedence::BOTTOM);
    hash->addPair(key, value);
    if (!this->peekTokenIs(TokenType::RBRACE) &&
        !this->expectPeek(TokenType::COMMA)) {
      return nullptr;
    }
  }
  if (!this->expectPeek(TokenType::RBRACE)) {
    return nullptr;
  }
  return hash;
}

std::shared_ptr<AST::Expression> Parser::parseIndexExpression(
    std::shared_ptr<AST::Expression> left) {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing index expression for {} ", *this->currentToken);
  auto tok = this->currentToken;
  this->nextToken();
  auto index = parseExpression(Precedence::BOTTOM);

  if (!this->expectPeek(TokenType::RBRACKET)) {
    return nullptr;
  }
  return std::make_shared<AST::IndexExpression>(tok, left, index);
}

std::shared_ptr<AST::Statement> Parser::parseExpressionStatement() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing expression statement for {} ", *this->currentToken);
  auto tok = this->currentToken;
  auto stmt = std::make_shared<AST::ExpressionStatement>(
      tok, this->parseExpression(Precedence::BOTTOM));
  if (this->peekTokenIs(TokenType::SEMICOLON)) {
    this->nextToken();
  };
  return stmt;
}

std::shared_ptr<AST::Expression> Parser::parseExpression(Precedence prec) {
  auto prefixFnPair = this->prefixParseFunctions.find(this->currentToken->type);
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing expression {}", *this->currentToken);
  if (prefixFnPair == this->prefixParseFunctions.end()) {
    auto msg = fmt::format("No prefix expression found for {}",
                           this->currentToken->literal);
    spdlog::get(PARSER_LOGGER)->warn(msg);
    this->addError(this->currentToken, msg);
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

std::shared_ptr<AST::Expression> Parser::parseBoolean() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing boolean expression for {} ", *this->currentToken);
  return std::make_shared<AST::Boolean>(this->currentToken,
                                        this->currentTokenIs(TokenType::TRUE));
}

std::shared_ptr<AST::Expression> Parser::parsePrefixExpression() {
  auto tok = this->currentToken;
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing prefix for {} ", *this->currentToken);
  this->nextToken();
  auto right = this->parseExpression(Precedence::PREFIX);
  return std::make_shared<AST::PrefixExpression>(tok, right, tok->literal);
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
  spdlog::get(PARSER_LOGGER)
      ->info("Returning infix expression {}", expr->toDebugString());
  return expr;
}

std::shared_ptr<AST::Expression> Parser::parseIdentifier() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing identifier for {} ", *this->currentToken);
  return std::make_shared<AST::Identifier>(this->currentToken,
                                           this->currentToken->literal);
}

std::shared_ptr<AST::Expression> Parser::parseString() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing string for {} ", *this->currentToken);
  return std::make_shared<AST::StringLiteral>(this->currentToken,
                                              this->currentToken->literal);
}

std::shared_ptr<AST::Expression> Parser::parseIntegerLiteral() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing integer literal for {} ", *this->currentToken);
  int64_t value;
  try {
    value = std::stoll(this->currentToken->literal);
  } catch (std::invalid_argument const &e) {
    this->addError(this->currentToken, "Token value was not an integer");
    return nullptr;
  } catch (std::out_of_range const &e) {
    this->addError(this->currentToken, "Integer value out of range");
    return nullptr;
  }
  return std::make_shared<AST::IntegerLiteral>(this->currentToken, value);
}

std::shared_ptr<AST::Expression> Parser::parseIfExpression() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing if expression for {} ", *this->currentToken);
  auto tok = this->currentToken;
  if (!this->expectPeek(TokenType::LPAREN)) {
    return nullptr;
  }
  this->nextToken();
  auto cond = this->parseExpression(Precedence::BOTTOM);
  if (!this->expectPeek(TokenType::RPAREN)) {
    return nullptr;
  }
  if (!this->expectPeek(TokenType::LBRACE)) {
    return nullptr;
  }
  auto whenTrue = std::dynamic_pointer_cast<AST::BlockStatement>(
      this->parseBlockStatement());
  std::shared_ptr<AST::BlockStatement> whenFalse = nullptr;
  if (this->peekTokenIs(TokenType::ELSE)) {
    this->nextToken();
    if (!this->expectPeek(TokenType::LBRACE)) {
      return nullptr;
    }
    whenFalse = std::dynamic_pointer_cast<AST::BlockStatement>(
        this->parseBlockStatement());
  }
  return std::make_shared<AST::IfExpression>(tok, cond, whenTrue, whenFalse);
}

std::shared_ptr<AST::Expression> Parser::parseWhileExpression() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing while expression for {} ", *this->currentToken);
  auto tok = this->currentToken;
  if (!this->expectPeek(TokenType::LBRACE)) {
    return nullptr;
  }
  auto body = std::dynamic_pointer_cast<AST::BlockStatement>(
      this->parseBlockStatement());
  return std::make_shared<AST::WhileExpression>(tok, body);
}

std::shared_ptr<AST::Expression> Parser::parseFunctionLiteral() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing function literal for {} ", *this->currentToken);
  auto tok = this->currentToken;
  if (!this->expectPeek(TokenType::LPAREN)) {
    return nullptr;
  }
  std::vector<std::shared_ptr<AST::Expression>> args;
  auto foundParams =
      this->parseParameters(std::back_inserter(args), TokenType::RPAREN);
  if (!foundParams) {
    return nullptr;
  }
  if (!this->expectPeek(TokenType::LBRACE)) {
    return nullptr;
  }
  auto body = this->parseBlockStatement();
  auto fn = std::make_shared<AST::FunctionLiteral>(
      tok, std::dynamic_pointer_cast<AST::BlockStatement>(body));
  for (auto arg : args) {
    std::shared_ptr<AST::Identifier> ident =
        convertExpressionToType<AST::Identifier>(arg);
    if (ident) {
      fn->addArgument(ident);
    } else {
      return nullptr;
    }
  };
  return fn;
}

std::shared_ptr<AST::Expression> Parser::parseGroupedExpression() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing grouped expression for {} ", *this->currentToken);
  this->nextToken();
  auto expr = this->parseExpression(Precedence::BOTTOM);
  if (!this->expectPeek(TokenType::RPAREN)) {
    return nullptr;
  }
  return expr;
}

std::shared_ptr<AST::Statement> Parser::parseBlockStatement() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing block statement for {} ", *this->currentToken);
  auto tok = this->currentToken;
  auto block = std::make_shared<AST::BlockStatement>(tok);
  this->nextToken();
  while (!this->currentTokenIs(TokenType::RBRACE)) {
    if (this->currentTokenIs(TokenType::END_OF_FILE)) {
      this->addError(
          tok, fmt::format("Couldn't find matching } for block {}", *tok));
      return nullptr;
    }
    block->addStatement(this->parseStatement());
    this->nextToken();
  }
  return block;
}

std::shared_ptr<AST::Expression> Parser::parseCallExpression(
    std::shared_ptr<AST::Expression> func) {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing call expression for {} ", *this->currentToken);
  auto tok = this->currentToken;
  std::vector<std::shared_ptr<AST::Expression>> args;
  auto foundParams =
      this->parseParameters(std::back_inserter(args), TokenType::RPAREN);
  if (!foundParams) {
    return nullptr;
  }
  auto fn = std::make_shared<AST::CallExpression>(tok, func);
  for (auto arg : args) {
    fn->addArgument(arg);
  };
  return fn;
}

std::shared_ptr<AST::Statement> Parser::parseReturnStatement() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing return statement for {} ", *this->currentToken);
  auto tok = this->currentToken;

  if (this->peekTokenIs(TokenType::RBRACE)) {
    return std::make_shared<AST::ReturnStatement>(tok, nullptr);
  }
  this->nextToken();
  if (this->currentTokenIs(TokenType::SEMICOLON)) {
    return std::make_shared<AST::ReturnStatement>(tok, nullptr);
  }
  auto ret = this->parseExpression(Precedence::BOTTOM);
  auto stmt = std::make_shared<AST::ReturnStatement>(tok, ret);
  if (this->peekTokenIs(TokenType::SEMICOLON)) {
    this->nextToken();
  };
  return stmt;
}

std::shared_ptr<AST::Statement> Parser::parseLetStatement() {
  spdlog::get(PARSER_LOGGER)
      ->info("Parsing let statement for {} ", *this->currentToken);
  auto tok = this->currentToken;
  if (!this->expectPeek(TokenType::IDENT)) {
    return nullptr;
  }
  auto name = std::make_shared<AST::Identifier>(this->currentToken,
                                                this->currentToken->literal);
  if (!this->expectPeek(TokenType::ASSIGN)) {
    return nullptr;
  }
  this->nextToken();
  auto val = this->parseExpression(Precedence::BOTTOM);
  if (this->peekTokenIs(TokenType::SEMICOLON)) {
    this->nextToken();
  }
  return std::make_shared<AST::LetStatement>(tok, name, val);
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
    this->addError(
        this->peekToken,
        fmt::format("Expected {} but found {}", tokenTypeToString(type),
                    tokenTypeToString(this->peekToken->type)));
    return false;
  }
}

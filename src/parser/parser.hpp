#pragma once
#include "spdlog/sinks/null_sink.h"

#include <spdlog/spdlog.h>
#include <ast.hpp>
#include <functional>
#include <lexer.hpp>
#include <list>
#include <map>
#include <parser_errors.hpp>
#include <token.hpp>

const std::string PARSER_LOGGER = "parser";

class Parser;

typedef std::shared_ptr<Expression> (Parser::*PrefixParseFunction)();
typedef std::shared_ptr<Expression> (Parser::*InfixParseFunction)(
    std::shared_ptr<Expression>);

enum class Precedence : std::uint8_t {
  BOTTOM = 1,
  EQUALS = 2,
  LESSGREATER = 3,
  SUM = 4,
  PRODUCT = 5,
  PREFIX = 6,
  CALL = 7,
};

class Parser {
 private:
  std::unique_ptr<Lexer> lexer;
  std::shared_ptr<Token> currentToken;
  std::shared_ptr<Token> peekToken;
  std::list<std::shared_ptr<ParserError>> _errors;

  std::map<TokenType, PrefixParseFunction> prefixParseFunctions;
  std::map<TokenType, InfixParseFunction> infixParseFunctions;

  void nextToken();

  std::shared_ptr<Statement> parseStatement();
  std::shared_ptr<Statement> parseLetStatement();
  std::shared_ptr<Statement> parseReturnStatement();
  std::shared_ptr<Statement> parseExpressionStatement();

  std::shared_ptr<Expression> parseExpression(Precedence);
  std::shared_ptr<Expression> parseIdentifier();
  std::shared_ptr<Expression> parseIntegerLiteral();
  std::shared_ptr<Expression> parsePrefixExpression();
  std::shared_ptr<Expression> parseInfixExpression(std::shared_ptr<Expression>);

  Precedence peekPrecedence();
  Precedence currentPrecedence();

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

    parser->registerPrefix(TokenType::IDENT, &Parser::parseIdentifier);
    parser->registerPrefix(TokenType::INTEGER, &Parser::parseIntegerLiteral);
    parser->registerPrefix(TokenType::BANG, &Parser::parsePrefixExpression);
    parser->registerPrefix(TokenType::MINUS, &Parser::parsePrefixExpression);

    parser->registerInfix(TokenType::PLUS, &Parser::parseInfixExpression);
    parser->registerInfix(TokenType::MINUS, &Parser::parseInfixExpression);
    parser->registerInfix(TokenType::SLASH, &Parser::parseInfixExpression);
    parser->registerInfix(TokenType::ASTERISK, &Parser::parseInfixExpression);
    parser->registerInfix(TokenType::EQ, &Parser::parseInfixExpression);
    parser->registerInfix(TokenType::NE, &Parser::parseInfixExpression);
    parser->registerInfix(TokenType::LT, &Parser::parseInfixExpression);
    parser->registerInfix(TokenType::GT, &Parser::parseInfixExpression);
    return std::move(parser);
  }

  std::unique_ptr<Program> parseProgram();

  const std::list<std::shared_ptr<ParserError>> &errors() {
    return this->_errors;
  }

  void registerPrefix(TokenType type, PrefixParseFunction func) {
    this->prefixParseFunctions.insert(
        std::pair<TokenType, PrefixParseFunction>(type, func));
  }

  void registerInfix(TokenType type, InfixParseFunction func) {
    this->infixParseFunctions.insert(
        std::pair<TokenType, InfixParseFunction>(type, func));
  }
};
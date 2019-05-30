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

typedef std::shared_ptr<AST::Expression> (Parser::*PrefixParseFunction)();
typedef std::shared_ptr<AST::Expression> (Parser::*InfixParseFunction)(
    std::shared_ptr<AST::Expression>);

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
  std::list<ParserError> _errors;

  std::map<TokenType, PrefixParseFunction> prefixParseFunctions;
  std::map<TokenType, InfixParseFunction> infixParseFunctions;

  void nextToken();
  bool expectPeek(TokenType type);
  bool currentTokenIs(TokenType type) const;
  bool peekTokenIs(TokenType type) const;

  std::shared_ptr<AST::Statement> parseStatement();
  std::shared_ptr<AST::Statement> parseLetStatement();
  std::shared_ptr<AST::Statement> parseReturnStatement();
  std::shared_ptr<AST::Statement> parseExpressionStatement();
  std::shared_ptr<AST::Statement> parseBlockStatement();

  std::shared_ptr<AST::Expression> parseExpression(Precedence);
  std::shared_ptr<AST::Expression> parseIdentifier();
  std::shared_ptr<AST::Expression> parseString();
  std::shared_ptr<AST::Expression> parseIntegerLiteral();
  std::shared_ptr<AST::Expression> parseBoolean();
  std::shared_ptr<AST::Expression> parsePrefixExpression();
  std::shared_ptr<AST::Expression> parseInfixExpression(
      std::shared_ptr<AST::Expression>);
  std::shared_ptr<AST::Expression> parseGroupedExpression();
  std::shared_ptr<AST::Expression> parseIfExpression();
  std::shared_ptr<AST::Expression> parseFunctionLiteral();
  std::shared_ptr<AST::Expression> parseCallExpression(
      std::shared_ptr<AST::Expression> func);

  template <typename OutputIterator>
  bool parseFunctionParameters(OutputIterator out) {
    if (this->peekTokenIs(TokenType::RPAREN)) {
      return true;
    }
    this->nextToken();

    *(out++) = (std::make_shared<AST::Identifier>(this->currentToken,
                                                  this->currentToken->literal));
    while (this->peekTokenIs(TokenType::COMMA)) {
      this->nextToken();
      this->nextToken();
      *(out++) = (std::make_shared<AST::Identifier>(
          this->currentToken, this->currentToken->literal));
    }
    if (!this->expectPeek(TokenType::RPAREN)) {
      return false;
    }
    return true;
  }

  template <typename OutputIterator>
  bool parseCallArguments(OutputIterator out) {
    if (this->peekTokenIs(TokenType::COMMA)) {
      return true;
    }
    this->nextToken();

    *(out++) = this->parseExpression(Precedence::BOTTOM);

    while (this->peekTokenIs(TokenType::COMMA)) {
      this->nextToken();
      this->nextToken();
      *(out++) = this->parseExpression(Precedence::BOTTOM);
    }
    if (!this->expectPeek(TokenType::RPAREN)) {
      return false;
    }
    return true;
  }

  Precedence peekPrecedence();
  Precedence currentPrecedence();

  void addError(std::shared_ptr<Token> token, std::string message) {
    this->_errors.push_back(ParserError(token, message));
  }

 public:
  Parser(std::unique_ptr<Lexer> lexer) : lexer(std::move(lexer)) {
    if (!spdlog::get(PARSER_LOGGER)) {
      spdlog::create<spdlog::sinks::null_sink_st>(PARSER_LOGGER);
    }
    this->nextToken();
    this->nextToken();
    initRegisterMaps();
  }

  void initRegisterMaps() {
    this->registerPrefix(TokenType::IDENT, &Parser::parseIdentifier);
    this->registerPrefix(TokenType::INTEGER, &Parser::parseIntegerLiteral);
    this->registerPrefix(TokenType::BANG, &Parser::parsePrefixExpression);
    this->registerPrefix(TokenType::MINUS, &Parser::parsePrefixExpression);
    this->registerPrefix(TokenType::TRUE, &Parser::parseBoolean);
    this->registerPrefix(TokenType::FALSE, &Parser::parseBoolean);
    this->registerPrefix(TokenType::LPAREN, &Parser::parseGroupedExpression);
    this->registerPrefix(TokenType::IF, &Parser::parseIfExpression);
    this->registerPrefix(TokenType::FUNCTION, &Parser::parseFunctionLiteral);
    this->registerPrefix(TokenType::STRING, &Parser::parseString);

    this->registerInfix(TokenType::PLUS, &Parser::parseInfixExpression);
    this->registerInfix(TokenType::MINUS, &Parser::parseInfixExpression);
    this->registerInfix(TokenType::SLASH, &Parser::parseInfixExpression);
    this->registerInfix(TokenType::ASTERISK, &Parser::parseInfixExpression);
    this->registerInfix(TokenType::EQ, &Parser::parseInfixExpression);
    this->registerInfix(TokenType::NE, &Parser::parseInfixExpression);
    this->registerInfix(TokenType::LT, &Parser::parseInfixExpression);
    this->registerInfix(TokenType::GT, &Parser::parseInfixExpression);
    this->registerInfix(TokenType::LPAREN, &Parser::parseCallExpression);
  }

  std::unique_ptr<AST::Program> parseProgram();

  const std::list<ParserError> &errors() { return this->_errors; }

  void registerPrefix(TokenType type, PrefixParseFunction func) {
    this->prefixParseFunctions.insert(
        std::pair<TokenType, PrefixParseFunction>(type, func));
  }

  void registerInfix(TokenType type, InfixParseFunction func) {
    this->infixParseFunctions.insert(
        std::pair<TokenType, InfixParseFunction>(type, func));
  }
};
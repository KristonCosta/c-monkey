#pragma once
#include <spdlog/spdlog.h>
#include <catch2/catch.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <print_dispatcher.hpp>
#include <sstream>
#include "spdlog/sinks/stdout_color_sinks.h"

void printErrors(const std::list<std::shared_ptr<ParserError>> errors) {
  for (const auto &error : errors) {
    WARN(fmt::format("{}", *error));
  }
}

std::unique_ptr<AST::Program> testParserWithInput(std::string input) {
  auto lexer = Lexer::from(input);
  auto parser = Parser::from(std::move(lexer));
  auto program = parser->parseProgram();
  auto errors = parser->errors();
  printErrors(errors);
  REQUIRE(parser->errors().size() == 0);
  return program;
}

AST::IntegerLiteral *testIntegerLiteral(std::shared_ptr<AST::Expression> node,
                                        std::string str, int num) {
  const auto expression = dynamic_cast<AST::IntegerLiteral *>(node.get());
  REQUIRE(expression);
  REQUIRE(expression->getValue() == num);
  REQUIRE(expression->tokenLiteral() == str);
  return expression;
}

AST::IntegerLiteral *testIntegerLiteral(AST::Expression *node, std::string str,
                                        int num) {
  const auto expression = dynamic_cast<AST::IntegerLiteral *>(node);
  REQUIRE(expression);
  REQUIRE(expression->getValue() == num);
  REQUIRE(expression->tokenLiteral() == str);
  return expression;
}

AST::LetStatement *testLetStatement(AST::Statement *node, std::string str) {
  REQUIRE(node->tokenLiteral() == "let");
  const auto statement = dynamic_cast<AST::LetStatement *>(node);
  REQUIRE(statement);
  auto name = statement->getName();
  REQUIRE(statement->getName()->getValue() == str);
  REQUIRE(statement->getName()->tokenLiteral() == str);
  return statement;
}

AST::ReturnStatement *testReturnStatement(AST::Statement *node) {
  const auto statement = dynamic_cast<AST::ReturnStatement *>(node);
  REQUIRE(statement);
  REQUIRE(statement->tokenLiteral() == "return");
  return statement;
}

AST::ExpressionStatement *testExpressionStatement(AST::Statement *node) {
  const auto statement = dynamic_cast<AST::ExpressionStatement *>(node);
  REQUIRE(statement);
  return statement;
}

AST::IfExpression *testIfExpression(std::shared_ptr<AST::Expression> node) {
  const auto statement = dynamic_cast<AST::IfExpression *>(node.get());
  REQUIRE(statement);
  return statement;
}

AST::IfExpression *testIfExpression(AST::Expression *node) {
  const auto statement = dynamic_cast<AST::IfExpression *>(node);
  REQUIRE(statement);
  return statement;
}

AST::FunctionLiteral *testFunctionLiteral(
    std::shared_ptr<AST::Expression> node) {
  const auto statement = dynamic_cast<AST::FunctionLiteral *>(node.get());
  REQUIRE(statement);
  return statement;
}

AST::FunctionLiteral *testFunctionLiteral(AST::Expression *node) {
  const auto statement = dynamic_cast<AST::FunctionLiteral *>(node);
  REQUIRE(statement);
  return statement;
}

AST::Identifier *testIdentifier(std::shared_ptr<AST::Expression> node,
                                std::string val) {
  const auto expression = dynamic_cast<AST::Identifier *>(node.get());
  REQUIRE(expression);
  REQUIRE(expression->getValue() == val);
  REQUIRE(expression->tokenLiteral() == val);
  return expression;
}

AST::Identifier *testIdentifier(AST::Expression *node, std::string val) {
  const auto expression = dynamic_cast<AST::Identifier *>(node);
  REQUIRE(expression);
  REQUIRE(expression->getValue() == val);
  REQUIRE(expression->tokenLiteral() == val);
  return expression;
}

AST::InfixExpression *testInfixExpression(AST::Expression *node,
                                          std::string op) {
  const auto expression = dynamic_cast<AST::InfixExpression *>(node);
  REQUIRE(expression);
  REQUIRE(expression->getOp() == op);
  return expression;
}

AST::InfixExpression *testInfixExpression(std::shared_ptr<AST::Expression> node,
                                          std::string op) {
  const auto expression = dynamic_cast<AST::InfixExpression *>(node.get());
  REQUIRE(expression);
  REQUIRE(expression->getOp() == op);
  return expression;
}

AST::CallExpression *testCallExpression(AST::Expression *node,
                                        std::string name) {
  const auto expression = dynamic_cast<AST::CallExpression *>(node);
  REQUIRE(expression);
  return expression;
}

AST::CallExpression *testCallExpression(std::shared_ptr<AST::Expression> node,
                                        std::string name) {
  const auto expression = dynamic_cast<AST::CallExpression *>(node.get());
  REQUIRE(expression);
  testIdentifier(expression->getName(), name);
  return expression;
}

AST::PrefixExpression *testPrefixExpression(
    std::shared_ptr<AST::Expression> node, std::string op) {
  const auto expression = dynamic_cast<AST::PrefixExpression *>(node.get());
  REQUIRE(expression);
  REQUIRE(expression->getOp() == op);
  return expression;
}

AST::PrefixExpression *testPrefixExpression(AST::Expression *node,
                                            std::string op) {
  const auto expression = dynamic_cast<AST::PrefixExpression *>(node);
  REQUIRE(expression);
  REQUIRE(expression->getOp() == op);
  return expression;
}

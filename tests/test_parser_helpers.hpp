#pragma once
#include <spdlog/spdlog.h>
#include <catch2/catch.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <print_dispatcher.hpp>
#include <sstream>
#include "spdlog/sinks/stdout_color_sinks.h"

inline AST::IntegerLiteral *testIntegerLiteral(
    std::shared_ptr<AST::Expression> node, const std::string &str, int num) {
  const auto expression = dynamic_cast<AST::IntegerLiteral *>(node.get());
  REQUIRE(expression);
  REQUIRE(expression->getValue() == num);
  REQUIRE(expression->tokenLiteral() == str);
  return expression;
}

inline AST::IntegerLiteral *testIntegerLiteral(AST::Expression *node,
                                               const std::string &str,
                                               int num) {
  const auto expression = dynamic_cast<AST::IntegerLiteral *>(node);
  REQUIRE(expression);
  REQUIRE(expression->getValue() == num);
  REQUIRE(expression->tokenLiteral() == str);
  return expression;
}

inline AST::ArrayLiteral *testArrayLiteral(
    std::shared_ptr<AST::Expression> node, int num) {
  const auto expression = dynamic_cast<AST::ArrayLiteral *>(node.get());
  REQUIRE(expression);
  REQUIRE(expression->size() == num);
  return expression;
}

inline AST::IndexExpression *testIndexExpression(
    std::shared_ptr<AST::Expression> node) {
  const auto expression = dynamic_cast<AST::IndexExpression *>(node.get());
  REQUIRE(expression);
  return expression;
}

inline AST::WhileExpression *testWhileExpression(
    std::shared_ptr<AST::Expression> node) {
  const auto expression = dynamic_cast<AST::WhileExpression *>(node.get());
  REQUIRE(expression);
  return expression;
}

inline AST::LetStatement *testLetStatement(AST::Statement *node,
                                           const std::string &str) {
  REQUIRE(node->tokenLiteral() == "let");
  const auto statement = dynamic_cast<AST::LetStatement *>(node);
  REQUIRE(statement);
  auto name = statement->getName();
  REQUIRE(statement->getName()->getValue() == str);
  REQUIRE(statement->getName()->tokenLiteral() == str);
  return statement;
}

inline AST::ReturnStatement *testReturnStatement(AST::Statement *node) {
  const auto statement = dynamic_cast<AST::ReturnStatement *>(node);
  REQUIRE(statement);
  REQUIRE(statement->tokenLiteral() == "return");
  return statement;
}

inline AST::ExpressionStatement *testExpressionStatement(AST::Statement *node) {
  const auto statement = dynamic_cast<AST::ExpressionStatement *>(node);
  REQUIRE(statement);
  return statement;
}

inline AST::Boolean *testBoolean(std::shared_ptr<AST::Expression> node,
                                 bool expect) {
  const auto statement = dynamic_cast<AST::Boolean *>(node.get());
  REQUIRE(statement);
  REQUIRE(statement->getValue() == expect);
  return statement;
}

inline AST::Boolean *testBoolean(AST::Expression *node, bool expect) {
  const auto statement = dynamic_cast<AST::Boolean *>(node);
  REQUIRE(statement);
  REQUIRE(statement->getValue() == expect);
  return statement;
}

inline AST::StringLiteral *testString(std::shared_ptr<AST::Expression> node,
                                      const std::string &expect) {
  const auto statement = dynamic_cast<AST::StringLiteral *>(node.get());
  REQUIRE(statement);
  REQUIRE(statement->getValue() == expect);
  return statement;
}

inline AST::StringLiteral *testString(AST::Expression *node,
                                      const std::string &expect) {
  const auto statement = dynamic_cast<AST::StringLiteral *>(node);
  REQUIRE(statement);
  REQUIRE(statement->getValue() == expect);
  return statement;
}

inline AST::IfExpression *testIfExpression(
    std::shared_ptr<AST::Expression> node) {
  const auto statement = dynamic_cast<AST::IfExpression *>(node.get());
  REQUIRE(statement);
  return statement;
}

inline AST::IfExpression *testIfExpression(AST::Expression *node) {
  const auto statement = dynamic_cast<AST::IfExpression *>(node);
  REQUIRE(statement);
  return statement;
}

inline AST::HashLiteral *testHashLiteral(std::shared_ptr<AST::Expression> node,
                                         int numberOfPairs) {
  const auto statement = dynamic_cast<AST::HashLiteral *>(node.get());
  REQUIRE(statement);
  REQUIRE(statement->getPairs().size() == numberOfPairs);
  return statement;
}

inline AST::FunctionLiteral *testFunctionLiteral(
    std::shared_ptr<AST::Expression> node) {
  const auto statement = dynamic_cast<AST::FunctionLiteral *>(node.get());
  REQUIRE(statement);
  return statement;
}

inline AST::FunctionLiteral *testFunctionLiteral(AST::Expression *node) {
  const auto statement = dynamic_cast<AST::FunctionLiteral *>(node);
  REQUIRE(statement);
  return statement;
}

inline AST::Identifier *testIdentifier(std::shared_ptr<AST::Expression> node,
                                       const std::string &val) {
  const auto expression = dynamic_cast<AST::Identifier *>(node.get());
  REQUIRE(expression);
  REQUIRE(expression->getValue() == val);
  REQUIRE(expression->tokenLiteral() == val);
  return expression;
}

inline AST::Identifier *testIdentifier(AST::Expression *node,
                                       const std::string &val) {
  const auto expression = dynamic_cast<AST::Identifier *>(node);
  REQUIRE(expression);
  REQUIRE(expression->getValue() == val);
  REQUIRE(expression->tokenLiteral() == val);
  return expression;
}

inline AST::InfixExpression *testInfixExpression(AST::Expression *node,
                                                 const std::string &op) {
  const auto expression = dynamic_cast<AST::InfixExpression *>(node);
  REQUIRE(expression);
  REQUIRE(expression->getOp() == op);
  return expression;
}

inline AST::InfixExpression *testInfixExpression(
    std::shared_ptr<AST::Expression> node, const std::string &op) {
  const auto expression = dynamic_cast<AST::InfixExpression *>(node.get());
  REQUIRE(expression);
  REQUIRE(expression->getOp() == op);
  return expression;
}

inline AST::CallExpression *testCallExpression(AST::Expression *node,
                                               const std::string &name) {
  const auto expression = dynamic_cast<AST::CallExpression *>(node);
  REQUIRE(expression);
  return expression;
}

inline AST::CallExpression *testCallExpression(
    std::shared_ptr<AST::Expression> node, const std::string &name) {
  const auto expression = dynamic_cast<AST::CallExpression *>(node.get());
  REQUIRE(expression);
  testIdentifier(expression->getFunction(), name);
  return expression;
}

inline AST::PrefixExpression *testPrefixExpression(
    std::shared_ptr<AST::Expression> node, const std::string &op) {
  const auto expression = dynamic_cast<AST::PrefixExpression *>(node.get());
  REQUIRE(expression);
  REQUIRE(expression->getOp() == op);
  return expression;
}

inline AST::PrefixExpression *testPrefixExpression(AST::Expression *node,
                                                   const std::string &op) {
  const auto expression = dynamic_cast<AST::PrefixExpression *>(node);
  REQUIRE(expression);
  REQUIRE(expression->getOp() == op);
  return expression;
}

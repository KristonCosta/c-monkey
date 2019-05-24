#include <spdlog/spdlog.h>
#include <catch2/catch.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <print_dispatcher.hpp>
#include <sstream>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "test_parser_helpers.hpp"

TEST_CASE("Let statement parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);

  std::string input =
      R"V0G0N(let x = 5;
let y = 10;
let foobar = 838383;)V0G0N";
  std::string expectedIdentifiers[] = {"x", "y", "foobar"};

  auto program = testParserWithInput(input);
  REQUIRE(program->size() == 3);

  auto stmt = program->getStatements().begin();

  for (const std::string &identifier : expectedIdentifiers) {
    testLetStatement(stmt->get(), identifier);
    std::advance(stmt, 1);
  };
};

TEST_CASE("Return statement parsing", "[parser]") {
  std::string input =
      R"V0G0N(return 5;
return 10;
return 9999999;)V0G0N";
  std::string expectedIdentifiers[] = {"x", "y", "foobar"};
  auto program = testParserWithInput(input);
  REQUIRE(program->size() == 3);

  auto stmt = program->getStatements().begin();

  for (const std::string &identifier : expectedIdentifiers) {
    testReturnStatement(stmt->get());
    std::advance(stmt, 1);
  };
};

TEST_CASE("Identifier expression statement parsing", "[parser]") {
  std::string input = R"V0G0N(foobar;)V0G0N";
  auto program = testParserWithInput(input);
  REQUIRE(program->size() == 1);

  auto stmt = program->getStatements().begin();
  const auto statement = testExpressionStatement(stmt->get());
  testIdentifier(statement->getExpression(), "foobar");
};

TEST_CASE("Integer literal expression statement parsing", "[parser]") {
  std::string input = R"V0G0N(5;)V0G0N";
  auto program = testParserWithInput(input);
  REQUIRE(program->size() == 1);

  auto stmt = program->getStatements().begin();
  const auto statement = testExpressionStatement(stmt->get());
  testIntegerLiteral(statement->getExpression(), "5", 5);
};

TEST_CASE("Prefix operator parsing", "[parser]") {
  struct testPair {
    std::string input;
    std::string op;
    int val;
  };
  testPair pairs[] = {{"!5;", "!", 5}, {"-15;", "-", 15}};
  for (const auto &pair : pairs) {
    auto program = testParserWithInput(pair.input);
    REQUIRE(program->size() == 1);

    auto stmt = program->getStatements().begin();

    const auto statement = testExpressionStatement(stmt->get());
    const auto expression =
        testPrefixExpression(statement->getExpression(), pair.op);
    testIntegerLiteral(expression->getRight(), std::to_string(pair.val),
                       pair.val);
  };
};

TEST_CASE("Prefix operator precedence parsing", "[parser]") {
  struct testPair {
    std::string input;
    std::string expected;
  };
  testPair pairs[] = {
      {"-a * b", "((-a) * b)"},
      {"!-a", "(!(-a))"},
      {"a + b + c", "((a + b) + c)"},
      {"a + b - c", "((a + b) - c)"},
      {"a * b * c", "((a * b) * c)"},
      {"a * b / c", "((a * b) / c)"},
      {"a + b / c", "(a + (b / c))"},
      {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
      {"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
      {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
      {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
      {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
      {"true", "true"},
      {"false", "false"},
      {"3 > 5 == false", "((3 > 5) == false)"},
      {"3 < 5 == true", "((3 < 5) == true)"},
      {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
      {"(5 + 5) * 2", "((5 + 5) * 2)"},
      {"2 / (5 + 5)", "(2 / (5 + 5))"},
      {"-(5 + 5)", "(-(5 + 5))"},
      {"!(true == true)", "(!(true == true))"},
  };
  for (const auto &pair : pairs) {
    auto program = testParserWithInput(pair.input);
    std::stringstream ss;
    ASTPrinter::write([&](std::string message) { ss << message; }, *program);
    REQUIRE(ss.str() == pair.expected);
  };
};

TEST_CASE("If expression parsing", "[parser]") {
  auto input = "if (x < y) { x }";
  auto program = testParserWithInput(input);

  REQUIRE(program->size() == 1);
  auto stmt = program->getStatements().begin();
  const auto statement = testExpressionStatement(stmt->get());
  const auto expression = testIfExpression(statement->getExpression());
  const auto condition = testInfixExpression(expression->getCondition(), "<");
  testIdentifier(condition->getLeft(), "x");
  testIdentifier(condition->getRight(), "y");
  auto block = expression->getWhenTrue()->getStatements().begin()->get();
  testIdentifier(testExpressionStatement(block)->getExpression(), "x");
}

TEST_CASE("If else expression parsing", "[parser]") {
  auto input = "if (x > y) { w } else { z; q }";
  auto program = testParserWithInput(input);

  REQUIRE(program->size() == 1);
  auto stmt = program->getStatements().begin();
  const auto statement = testExpressionStatement(stmt->get());
  const auto expression = testIfExpression(statement->getExpression());
  const auto condition = testInfixExpression(expression->getCondition(), ">");
  testIdentifier(condition->getLeft(), "x");
  testIdentifier(condition->getRight(), "y");
  auto tblockstatements = expression->getWhenTrue()->getStatements();
  testIdentifier(
      testExpressionStatement(tblockstatements.begin()->get())->getExpression(),
      "w");
  auto fblockstatements = expression->getWhenFalse()->getStatements();
  REQUIRE(fblockstatements.size() == 2);
  auto itr = fblockstatements.begin();
  std::advance(itr, 1);
  testIdentifier(testExpressionStatement(itr->get())->getExpression(), "q");
}
/*
future


      {"a + add(b * c) + d", "((a + add((b * c))) + d)"},
      {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
       "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
      {"add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))"},
      {"a * [1, 2, 3, 4][b * c] * d", "((a * ([1, 2, 3, 4][(b * c)])) * d)"},
      {"add(a * b[2], b[1], 2 * [1, 2][1])",
       "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"},
       */

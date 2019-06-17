#include <spdlog/spdlog.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <lexer.hpp>
#include <parser.hpp>
#include <print_dispatcher.hpp>
#include <sstream>
#include <test_helpers.hpp>
#include <test_parser_helpers.hpp>
#include "spdlog/sinks/stdout_color_sinks.h"

TEST_CASE("Let statement parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);
  std::string input =
      R"V0G0N(let x = 5;
let y = 10;
let foobar = 838383;)V0G0N";
  std::string expectedIdentifiers[] = {"x", "y", "foobar"};

  auto program = testProgramWithInput(input);
  REQUIRE(program->size() == 3);

  auto stmt = program->getStatements().begin();

  for (const std::string &identifier : expectedIdentifiers) {
    testLetStatement(stmt->get(), identifier);
    std::advance(stmt, 1);
  };
};

TEST_CASE("Let statement with expression", "[parser]") {
  std::string input = "let x = 5 + 50;\nlet y = true;";
  auto program = testProgramWithInput(input);
  REQUIRE(program->size() == 2);
  auto stmt = program->getStatements().begin();
  const auto let1 = testLetStatement(stmt->get(), "x");
  const auto infix = testInfixExpression(let1->getValue(), "+");
  testIntegerLiteral(infix->getLeft(), "5", 5);
  testIntegerLiteral(infix->getRight(), "50", 50);
  stmt++;
  const auto let2 = testLetStatement(stmt->get(), "y");
  testBoolean(let2->getValue(), true);
};

TEST_CASE("Return statement parsing", "[parser]") {
  std::string input =
      R"V0G0N(return 5;
return 10;
return 9999999;)V0G0N";
  std::string expectedIdentifiers[] = {"x", "y", "foobar"};
  auto program = testProgramWithInput(input);
  REQUIRE(program->size() == 3);

  auto stmt = program->getStatements().begin();

  for (const std::string &identifier : expectedIdentifiers) {
    testReturnStatement(stmt->get());
    std::advance(stmt, 1);
  };
};

TEST_CASE("Return statement with expression", "[parser]") {
  std::string input = "return 6 + 7;\nreturn add(1,2);";
  auto program = testProgramWithInput(input);
  REQUIRE(program->size() == 2);
  auto stmt = program->getStatements().begin();
  const auto ret1 = testReturnStatement(stmt->get());
  const auto infix = testInfixExpression(ret1->getReturnValue(), "+");
  testIntegerLiteral(infix->getLeft(), "6", 6);
  testIntegerLiteral(infix->getRight(), "7", 7);
  stmt++;
  const auto ret2 = testReturnStatement(stmt->get());
  const auto call = testCallExpression(ret2->getReturnValue(), "add");
  auto callArgs = call->getArguments().begin();
  testIntegerLiteral(callArgs->get(), "1", 1);
  callArgs++;
  testIntegerLiteral(callArgs->get(), "2", 2);
};

TEST_CASE("Identifier expression statement parsing", "[parser]") {
  std::string input = R"V0G0N(foobar;)V0G0N";
  auto program = testProgramWithInput(input);
  REQUIRE(program->size() == 1);

  auto stmt = program->getStatements().begin();
  const auto statement = testExpressionStatement(stmt->get());
  testIdentifier(statement->getExpression(), "foobar");
};

TEST_CASE("Integer literal expression statement parsing", "[parser]") {
  std::string input = R"V0G0N(5;)V0G0N";
  auto program = testProgramWithInput(input);
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
    auto program = testProgramWithInput(pair.input);
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
  // spdlog::stdout_color_mt(PARSER_LOGGER);
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
      {"a + add(b * c) + d", "((a + add((b * c))) + d)"},
      {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
       "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
      {"add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))"},
      {"a * [1, 2, 3, 4][b * c] * d", "((a * ([1, 2, 3, 4][(b * c)])) * d)"},
      {"add(a * b[2], b[1], 2 * [1, 2][1])",
       "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"},

  };
  for (const auto &pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    std::stringstream ss;
    ASTPrinter::write([&](std::string message) { ss << message; }, *program);
    REQUIRE(ss.str() == pair.expected);
  };
};

TEST_CASE("Hash literal parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);
  auto input = R"V0G0N({"one": 1, "two": 2, "three": 3})V0G0N";
  auto program = testProgramWithInput(input);

  REQUIRE(program->size() == 1);
  const auto statement =
      testExpressionStatement(program->getStatements().begin()->get());
  const auto hash = testHashLiteral(statement->getExpression(), 3);
  struct testPair {
    std::string key;
    int value;
  };
  std::map<std::string, int> pairs = {
      {"one", 1},
      {"two", 2},
      {"three", 3},
  };
  auto hashPairs = hash->getPairs();
  for (const auto &pair : hashPairs) {
    auto expectedPair = pairs.find(pair.first->tokenLiteral());
    REQUIRE(expectedPair != pairs.end());
    testString(pair.first.get(), expectedPair->first);
    testIntegerLiteral(pair.second.get(), std::to_string(expectedPair->second),
                       expectedPair->second);
  };
}

TEST_CASE("Empty hash literal parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);
  auto input = R"V0G0N({})V0G0N";
  auto program = testProgramWithInput(input);

  REQUIRE(program->size() == 1);
  const auto statement =
      testExpressionStatement(program->getStatements().begin()->get());
  const auto hash = testHashLiteral(statement->getExpression(), 0);
  REQUIRE(hash->getPairs().size() == 0);
}

TEST_CASE("If expression parsing", "[parser]") {
  auto input = "if (x < y) { x }";
  auto program = testProgramWithInput(input);

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
  auto program = testProgramWithInput(input);

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

TEST_CASE("Function literal parsing", "[parser]") {
  auto input = "fn(x, y) { x + y }";
  auto program = testProgramWithInput(input);
  REQUIRE(program->size() == 1);
  const auto statement =
      testExpressionStatement(program->getStatements().begin()->get());
  const auto fnLit = testFunctionLiteral(statement->getExpression());
  REQUIRE(fnLit->getArguments().size() == 2);
  auto itr = fnLit->getArguments().begin();
  testIdentifier(itr->get(), "x");
  std::advance(itr, 1);
  testIdentifier(itr->get(), "y");
  REQUIRE(fnLit->getBody()->getStatements().size() == 1);
  const auto expr =
      testExpressionStatement(fnLit->getBody()->getStatements().begin()->get());
  const auto infx = testInfixExpression(expr->getExpression(), "+");
  testIdentifier(infx->getLeft(), "x");
  testIdentifier(infx->getRight(), "y");
}

TEST_CASE("Call expression testing", "[parser]") {
  auto input = "add(1, 2 * 3, 4 + 5);";
  auto program = testProgramWithInput(input);
  REQUIRE(program->size() == 1);
  const auto statement =
      testExpressionStatement(program->getStatements().begin()->get());
  const auto call = testCallExpression(statement->getExpression(), "add");
  REQUIRE(call->getArguments().size() == 3);
  auto itr = call->getArguments().begin();
  testIntegerLiteral(itr->get(), "1", 1);
  itr++;

  const auto expr1 = testInfixExpression(itr->get(), "*");
  testIntegerLiteral(expr1->getLeft(), "2", 2);
  testIntegerLiteral(expr1->getRight(), "3", 3);
  itr++;
  const auto expr2 = testInfixExpression(itr->get(), "+");
  testIntegerLiteral(expr2->getLeft(), "4", 4);
  testIntegerLiteral(expr2->getRight(), "5", 5);
}

TEST_CASE("String parsing", "[parser]") {
  auto input = "let x = \"test 123\";";
  auto program = testProgramWithInput(input);
  REQUIRE(program->size() == 1);
  const auto statement =
      testLetStatement(program->getStatements().begin()->get(), "x");
  testString(statement->getValue(), "test 123");
}

TEST_CASE("Array parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);
  auto input = "[3, 4, 5 + 6, fn(x) { x; }];";
  auto program = testProgramWithInput(input);
  REQUIRE(program->size() == 1);
  const auto statement =
      testExpressionStatement(program->getStatements().begin()->get());
  const auto array = testArrayLiteral(statement->getExpression(), 4);
  auto itr = array->getValues().begin();
  testIntegerLiteral(itr->get(), "3", 3);
  itr++;
  testIntegerLiteral(itr->get(), "4", 4);
  itr++;
  testInfixExpression(itr->get(), "+");
  itr++;
  const auto fn = testFunctionLiteral(itr->get());
  const auto body =
      testExpressionStatement(fn->getBody()->getStatements().front().get());
  testIdentifier(body->getExpression(), "x");
}

TEST_CASE("Index expression parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);
  auto input = "someArray[3+1];";
  auto program = testProgramWithInput(input);
  REQUIRE(program->size() == 1);
  const auto statement =
      testExpressionStatement(program->getStatements().begin()->get());
  const auto expr = testIndexExpression(statement->getExpression());
  testIdentifier(expr->getLeft(), "someArray");
  const auto infix = testInfixExpression(expr->getIndex(), "+");
  testIntegerLiteral(infix->getLeft(), "3", 3);
  testIntegerLiteral(infix->getRight(), "1", 1);
}
/*
future
      {"a * [1, 2, 3, 4][b * c] * d", "((a * ([1, 2, 3, 4][(b * c)])) * d)"},
      {"add(a * b[2], b[1], 2 * [1, 2][1])",
       "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"},


       */

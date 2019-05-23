#include <spdlog/spdlog.h>
#include <catch2/catch.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <sstream>
#include "spdlog/sinks/stdout_color_sinks.h"

void printErrors(const std::list<std::shared_ptr<ParserError>> errors) {
  for (const auto &error : errors) {
    WARN(fmt::format("{}", *error));
  }
}

TEST_CASE("Let statement parsing", "[parser]") {
  spdlog::stdout_color_mt(PARSER_LOGGER);

  std::string input =
      R"V0G0N(let x = 5;
let y = 10;
let foobar = 838383;)V0G0N";
  std::string expectedIdentifiers[] = {"x", "y", "foobar"};
  auto lexer = Lexer::from(input);
  auto parser = Parser::from(std::move(lexer));
  auto program = parser->parseProgram();
  auto errors = parser->errors();
  printErrors(errors);

  REQUIRE(parser->errors().size() == 0);
  REQUIRE(program->size() == 3);

  auto stmt = program->begin();

  for (const std::string &identifier : expectedIdentifiers) {
    REQUIRE(stmt->get()->literal() == "let");
    const auto statement = dynamic_cast<LetStatement *>(stmt->get());
    REQUIRE(statement);
    auto name = statement->getName();
    REQUIRE(statement->getName()->getValue() == identifier);
    REQUIRE(statement->getName()->literal() == identifier);
    std::advance(stmt, 1);
  };
};

TEST_CASE("Return statement parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);

  std::string input =
      R"V0G0N(return 5;
return 10;
return 9999999;)V0G0N";
  std::string expectedIdentifiers[] = {"x", "y", "foobar"};
  auto lexer = Lexer::from(input);
  auto parser = Parser::from(std::move(lexer));
  auto program = parser->parseProgram();
  auto errors = parser->errors();
  printErrors(errors);

  REQUIRE(parser->errors().size() == 0);
  REQUIRE(program->size() == 3);

  auto stmt = program->begin();

  for (const std::string &identifier : expectedIdentifiers) {
    const auto statement = dynamic_cast<ReturnStatement *>(stmt->get());
    REQUIRE(statement);
    REQUIRE(statement->literal() == "return");
    std::advance(stmt, 1);
  };
};

TEST_CASE("Identifier expression statement parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);

  std::string input = R"V0G0N(foobar;)V0G0N";
  auto lexer = Lexer::from(input);
  auto parser = Parser::from(std::move(lexer));
  auto program = parser->parseProgram();
  auto errors = parser->errors();
  printErrors(errors);

  REQUIRE(parser->errors().size() == 0);
  REQUIRE(program->size() == 1);

  auto stmt = program->begin();
  const auto statement = dynamic_cast<ExpressionStatement *>(stmt->get());
  REQUIRE(statement);
  const auto expression =
      dynamic_cast<Identifier *>(statement->getExpression().get());
  REQUIRE(expression);
  REQUIRE(expression->getValue() == "foobar");
  REQUIRE(expression->literal() == "foobar");
};

TEST_CASE("Integer literal expression statement parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);

  std::string input = R"V0G0N(5;)V0G0N";
  auto lexer = Lexer::from(input);
  auto parser = Parser::from(std::move(lexer));
  auto program = parser->parseProgram();
  auto errors = parser->errors();
  printErrors(errors);

  REQUIRE(parser->errors().size() == 0);
  REQUIRE(program->size() == 1);

  auto stmt = program->begin();
  const auto statement = dynamic_cast<ExpressionStatement *>(stmt->get());
  REQUIRE(statement);
  const auto expression =
      dynamic_cast<IntegerLiteral *>(statement->getExpression().get());
  REQUIRE(expression);
  REQUIRE(expression->getValue() == 5);
  REQUIRE(expression->literal() == "5");
};

TEST_CASE("Prefix operator parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);
  struct testPair {
    std::string input;
    std::string op;
    int val;
  };
  testPair pairs[] = {{"!5;", "!", 5}, {"-15;", "-", 15}};
  for (const auto &pair : pairs) {
    auto lexer = Lexer::from(pair.input);
    auto parser = Parser::from(std::move(lexer));
    auto program = parser->parseProgram();
    auto errors = parser->errors();
    printErrors(errors);

    REQUIRE(parser->errors().size() == 0);
    REQUIRE(program->size() == 1);

    auto stmt = program->begin();
    const auto statement = dynamic_cast<ExpressionStatement *>(stmt->get());
    REQUIRE(statement);
    const auto expression =
        dynamic_cast<PrefixExpression *>(statement->getExpression().get());
    REQUIRE(expression);
    REQUIRE(expression->getOp() == pair.op);
    const auto lit =
        dynamic_cast<IntegerLiteral *>(expression->getRight().get());
    REQUIRE(lit->getValue() == pair.val);
    REQUIRE(lit->literal() == std::to_string(pair.val));
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
      {"a + add(b * c) + d", "((a + add((b * c))) + d)"},
      {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
       "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
      {"add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))"},
      {"a * [1, 2, 3, 4][b * c] * d", "((a * ([1, 2, 3, 4][(b * c)])) * d)"},
      {"add(a * b[2], b[1], 2 * [1, 2][1])",
       "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"},
  };
  for (const auto &pair : pairs) {
    auto lexer = Lexer::from(pair.input);
    auto parser = Parser::from(std::move(lexer));
    auto program = parser->parseProgram();
    auto errors = parser->errors();
    printErrors(errors);

    REQUIRE(parser->errors().size() == 0);
    REQUIRE(program->toString() == pair.expected);
  };
};

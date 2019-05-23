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
  // spdlog::stdout_color_mt(PARSER_LOGGER);

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

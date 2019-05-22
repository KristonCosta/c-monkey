#include <spdlog/spdlog.h>
#include <catch2/catch.hpp>
#include <let_statement.hpp>
#include <lexer.hpp>
#include "spdlog/sinks/stdout_color_sinks.h"

#include <parser.hpp>

TEST_CASE("Let statement parsing", "[parser]") {
  // spdlog::stdout_color_mt(PARSER_LOGGER);

  std::string input =
      "let x = 5;\
      let y = 10;\
      let foobar = 838383;";
  std::string expectedIdentifiers[] = {"x", "y", "foobar"};
  auto lexer = Lexer::from(input);
  auto parser = Parser::from(std::move(lexer));
  auto program = parser->parseProgram();
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

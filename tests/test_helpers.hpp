#pragma once
#include <spdlog/spdlog.h>
#include <catch2/catch.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <print_dispatcher.hpp>

inline void printErrors(const std::list<ParserError> &errors) {
  for (const auto &error : errors) {
    WARN(fmt::format("{}", error));
  }
}

inline std::unique_ptr<AST::Program> testProgramWithInput(std::string input) {
  auto lexer = std::make_unique<Lexer>(input);
  auto parser = Parser(std::move(lexer));
  auto program = parser.parseProgram();
  auto errors = parser.errors();
  printErrors(errors);
  REQUIRE(parser.errors().size() == 0);
  return program;
}

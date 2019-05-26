#include <catch2/catch.hpp>
#include <eval.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <test_eval_helpers.hpp>
#include <test_helpers.hpp>
#include "spdlog/sinks/stdout_color_sinks.h"

TEST_CASE("Integer literal eval testing", "[eval]") {
  // spdlog::stdout_color_mt(EVAL_LOGGER);
  auto program = testProgramWithInput("213");
  auto bag = ASTEvaluator::eval(*program);
  testIntegerBag(bag, 213);
};

TEST_CASE("Boolean eval testing", "[eval]") {
  auto program = testProgramWithInput("true");
  auto bag = ASTEvaluator::eval(*program);
  testBooleanBag(bag, true);
};

TEST_CASE("Bang eval testing", "[eval]") {
  Pair<bool> pairs[] = {
      {"!true", false}, {"!false", true},   {"!5", false},
      {"!!true", true}, {"!!false", false},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto bag = ASTEvaluator::eval(*program);
    testBooleanBag(bag, pair.expected);
  }
};
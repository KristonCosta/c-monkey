#include <catch2/catch.hpp>
#include <eval.hpp>
#include <lexer.hpp>
#include <optional>
#include <parser.hpp>
#include <test_eval_helpers.hpp>
#include <test_helpers.hpp>
#include "spdlog/sinks/stdout_color_sinks.h"

TEST_CASE("Integer eval testing", "[eval]") {
  // spdlog::stdout_color_mt(EVAL_LOGGER);
  Pair<int64_t> pairs[] = {
      {"5", 5},
      {"123", 123},
      {"-5", -5},
      {"-52345", -52345},
      {"5 + 5 + 6 - 9", 7},
      {"5 * 5 - 6", 19},
      {"20 + 2 * -10", 0},
      {"3 * (3 * 4) + 5", 41},
      {"2 * (10 + 2)", 24},
      {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto bag = ASTEvaluator::eval(*program);
    testIntegerBag(bag, pair.expected);
  }
};

TEST_CASE("Boolean eval testing", "[eval]") {
  Pair<bool> pairs[] = {
      {"true", true},
      {"false", false},
      {"!true", false},
      {"!!false", false},
      {"!5", false},
      {"true == true", true},
      {"false == true", false},
      {"false == false", true},
      {"(1 < 10) == true", true},
      {"(1 + 10) < 10 == true", false},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto bag = ASTEvaluator::eval(*program);
    testBooleanBag(bag, pair.expected);
  }
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

TEST_CASE("If else testing", "[eval]") {
  // spdlog::stdout_color_mt(EVAL_LOGGER);
  Pair<std::optional<int64_t>> pairs[] = {
      {"if (true) { 10 }", 10},
      {"if (false) { 10 }", std::nullopt},
      {"if (1) { 10}", 10},
      {"if (1 < 2) { 10 }", 10},
      {"if (1 > 2) { 10 }", std::nullopt},
      {"if (1 > 2) {10} else {20}", 20},
      {"if (1 < 2) {10} else {20}", 10},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto bag = ASTEvaluator::eval(*program);
    if (pair.expected) {
      testIntegerBag(bag, pair.expected.value());
    } else {
      testNullBag(bag);
    }
  }
};

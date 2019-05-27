#include <catch2/catch.hpp>
#include <eval.hpp>
#include <lexer.hpp>
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
  Pair<int64_t> pairs[] = {
      {"if (true) { 10 }", 10},

      {"if (1) { 10}", 10},
      {"if (1 < 2) { 10 }", 10},

      {"if (1 > 2) {10} else {20}", 20},
      {"if (1 < 2) {10} else {20}", 10},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto bag = ASTEvaluator::eval(*program);
    testIntegerBag(bag, pair.expected);
  }
  // Expecting nulls
  Pair<int64_t> pairs2[] = {{"if (false) { 10 }", 1}, {"if (1 > 2) { 10 }", 1}};
  for (const auto& pair : pairs2) {
    auto program = testProgramWithInput(pair.input);
    auto bag = ASTEvaluator::eval(*program);
    testNullBag(bag);
  }
};

TEST_CASE("Return statement testing", "[eval]") {
  // spdlog::stdout_color_mt(EVAL_LOGGER);
  Pair<int64_t> pairs[] = {
      {"return 10;", 10},
      {"return 9; 10", 9},
      {"return 2 * 5; 20", 10},
      {"1; return 1 * 8; 10;", 8},
      {R"V0G0N(
      if (10 > 2) {
        if (true) {
          return 10;
        }
        return 1;
      })V0G0N",
       10},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto bag = ASTEvaluator::eval(*program);
    testIntegerBag(bag, pair.expected);
  }
};

TEST_CASE("Error testing", "[eval]") {
  // spdlog::stdout_color_mt(EVAL_LOGGER);
  Pair<std::string> pairs[] = {
      {"5 + true;", "type mismatch: INTEGER + BOOLEAN"},
      {"5 + true; 5;", "type mismatch: INTEGER + BOOLEAN"},
      {"-true;", "unknown operator: -BOOLEAN"},
      {"true + false;", "unknown operator: BOOLEAN + BOOLEAN"},
      {"5; true + true;", "unknown operator: BOOLEAN + BOOLEAN"},
      {"if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN"},
      {R"V0G0N(
      if (10 > 2) {
        if (true) {
          return true + true;
        }
        return 1;
      })V0G0N",
       "unknown operator: BOOLEAN + BOOLEAN"}

  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto bag = ASTEvaluator::eval(*program);
    testErrorBag(bag, pair.expected);
  }
}
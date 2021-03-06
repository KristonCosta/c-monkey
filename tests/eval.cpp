#include <catch2/catch.hpp>
#include <env.hpp>
#include <eval.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <test_eval_helpers.hpp>
#include <test_helpers.hpp>
#include "spdlog/sinks/stdout_color_sinks.h"

TEST_CASE("Integer eval testing", "[eval]") {
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
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
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
      {"let x = \"comp\"; x == \"comp\"", true},
      {"let x = \"com p\"; x == \"comp\"", false},
      {"let x = \" p\"; x != \"whatisthis\"", true},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
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
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testBooleanBag(bag, pair.expected);
  }
};

TEST_CASE("If else testing", "[eval]") {
  Pair<int64_t> pairs[] = {
      {"if (true) { 10 }", 10},

      {"if (1) { 10}", 10},
      {"if (1 < 2) { 10 }", 10},

      {"if (1 > 2) {10} else {20}", 20},
      {"if (1 < 2) {10} else {20}", 10},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testIntegerBag(bag, pair.expected);
  }
  // Expecting nulls
  Pair<int64_t> pairs2[] = {{"if (false) { 10 }", 1}, {"if (1 > 2) { 10 }", 1}};
  for (const auto& pair : pairs2) {
    auto program = testProgramWithInput(pair.input);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testNullBag(bag);
  }
};

TEST_CASE("Return statement testing", "[eval]") {
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
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testIntegerBag(bag, pair.expected);
  }
};

TEST_CASE("Error testing", "[eval]") {
  Pair<std::string> pairs[] = {
      {"5 + true;", "type mismatch: INTEGER + BOOLEAN"},
      {"5 + true; 5;", "type mismatch: INTEGER + BOOLEAN"},
      {"-true;", "unknown operator: -BOOLEAN"},
      {"true + false;", "unknown operator: BOOLEAN + BOOLEAN"},
      {"5; true + true;", "unknown operator: BOOLEAN + BOOLEAN"},
      {"if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN"},
      {"test;", "identifier not found: test"},
      {R"V0G0N(
      if (10 > 2) {
        if (true) {
          return true + true;
        }
        return 1;
      })V0G0N",
       "unknown operator: BOOLEAN + BOOLEAN"},
      {"\"test\" - \"test\"", "unknown operator: STRING - STRING"},
      {"len(1)", "argument to `len` not supported, got INTEGER"},
      {"len(\"one\", \"two\")",
       "wrong number of arguments to `len`: expected 1, found 2"},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testErrorBag(bag, pair.expected);
  }
}

TEST_CASE("Let statement testing", "[eval]") {
  //  spdlog::stdout_color_mt(EVAL_LOGGER);
  Pair<int64_t> pairs[] = {
      {"let a = 5; a;", 5},
      {"let a = 5 * 5; a;", 25},
      {"let a = 5; let b = a; b;", 5},
      {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testIntegerBag(bag, pair.expected);
  }
}

TEST_CASE("Index evaluation testing", "[eval]") {
  //  spdlog::stdout_color_mt(EVAL_LOGGER);
  Pair<int64_t> pairs[] = {
      {"[1,2,3][0]", 1},
      {"[1,2,3][2]", 3},
      {"let i = 0; [1][i]", 1},
      {"let array = [7,5,4];array[1]", 5},
      {"let arr = [1,2,3]; arr[0] + arr[1] + arr[2];", 6},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testIntegerBag(bag, pair.expected);
  }
}

TEST_CASE("Index evaluation testing invalid index", "[eval]") {
  //  spdlog::stdout_color_mt(EVAL_LOGGER);
  std::string vals[] = {"[1,2,3][-1];", "[1,2,3][3];"};
  for (const auto& val : vals) {
    auto program = testProgramWithInput(val);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testNullBag(bag);
  }
}

TEST_CASE("String eval testing", "[eval]") {
  //  spdlog::stdout_color_mt(EVAL_LOGGER);
  Pair<std::string> pairs[] = {
      {"let a = \"test123\"; a;", "test123"},
      {"\"a blank and multilength string\";", "a blank and multilength string"},
      {"\"lets\" + \" test\"", "lets test"},
      {"let x = \"com p\"; x;", "com p"},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testStringBag(bag, pair.expected);
  }
}

TEST_CASE("Array builtins int", "[eval]") {
  Pair<int64_t> pairs[] = {
      {"len([1,2,3,4])", 4},
      {"len([])", 0},
      {"head([3,4,5])", 3},
  };
  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testIntegerBag(bag, pair.expected);
  }
}

TEST_CASE("Array builtins array", "[eval]") {
  std::string inputs[] = {
      {"tail([1,2,3,4,5])"},
      {"push([2,3,4], 5)"},
  };
  for (const auto& input : inputs) {
    auto program = testProgramWithInput(input);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    auto arr = testArrayBag(bag, 4);
    testIntegerBag(arr->values().at(0), 2);
    testIntegerBag(arr->values().at(1), 3);
    testIntegerBag(arr->values().at(2), 4);
    testIntegerBag(arr->values().at(3), 5);
  }
}

TEST_CASE("While eval testing", "[eval]") {
  std::string input =
      "let x = 1; while { if ( x > 3 ) { return x; }; let x = x + 1; }; x ";
  auto program = testProgramWithInput(input);
  auto env = std::make_shared<Env::Environment>();
  auto bag = ASTEvaluator::eval(*program, env);
  testIntegerBag(bag, 4);
}

TEST_CASE("Array eval testing", "[eval]") {
  // spdlog::stdout_color_mt(EVAL_LOGGER);
  auto input = "[1, 2 + 2, 3 * 3]";
  auto program = testProgramWithInput(input);
  auto env = std::make_shared<Env::Environment>();
  auto bag = ASTEvaluator::eval(*program, env);
  auto arr = testArrayBag(bag, 3);
  auto itr = arr->values().begin();
  testIntegerBag(itr->get(), 1);
  itr++;
  testIntegerBag(itr->get(), 4);
  itr++;
  testIntegerBag(itr->get(), 9);
}

TEST_CASE("Function eval testing", "[eval]") {
  //  spdlog::stdout_color_mt(EVAL_LOGGER);
  Pair<int64_t> pairs[] = {
      {"let identity = fn(x) { x;}; identity(12);", 12},
      {"let greater = fn(x, y) { x > y; }; if (greater(2, 1)){ 10; } else "
       "{20;}",
       10},
      {"fn(x) { x + 1;}(5)", 6},
      {"let newAdder = fn(x) { fn(y) { x + y }; }; let addTwo = newAdder(2); "
       "addTwo(10);",
       12},
      {"let add = fn(x, y) { x + y }; let sub = fn(x, y) {x - y}; let "
       "applyFunc = fn(a, b, func) { func(a, b) }; applyFunc(2, 2, add);",
       4},
      {"len(\"123\")", 3},
      {"len(\"\")", 0},
      {"len(\"hello\" + \" world!\")", 12},
  };

  for (const auto& pair : pairs) {
    auto program = testProgramWithInput(pair.input);
    auto env = std::make_shared<Env::Environment>();
    auto bag = ASTEvaluator::eval(*program, env);
    testIntegerBag(bag, pair.expected);
  }
}
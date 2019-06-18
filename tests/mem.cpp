#include <env.hpp>
#include <eval.hpp>
#include <iostream>
#include <lexer.hpp>
#include <parser.hpp>

inline std::unique_ptr<AST::Program> testProgramWithInput(std::string input) {
  auto lexer = std::make_unique<Lexer>(input);
  auto parser = Parser(std::move(lexer));
  auto program = parser.parseProgram();
  auto errors = parser.errors();
  assert(parser.errors().size() == 0);
  return program;
}

int main() {
  auto input = R"V0G0N(
let range = fn(start, end) {
  let iter = fn(start, end, res) {
    if (start == end) {
      res
    } else {
      iter(start + 1, end, push(res, start))
    }
  }
  iter(start, end, []);
}
  )V0G0N";
  auto program = testProgramWithInput(input);
  auto env = std::make_shared<Env::Environment>();
  auto bag = ASTEvaluator::eval(*program, env);
  std::string itr[] = {"range(1,1000);", "range(1,1000);", "range(1,1000);",
                       "range(1,1000);", "range(1,1000);", "range(1,1000);",
                       "range(1,1000);", "range(1,1000);", "range(1,1000);"};
  for (const auto &str : itr) {
    program = testProgramWithInput(str);
    bag = ASTEvaluator::eval(*program, env);
    std::cout << bag->inspect() << std::endl;
  }
}
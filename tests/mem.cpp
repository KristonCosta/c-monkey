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
  let acc = []
  while {
    if (start > end) {
      return acc
    } else {
      let acc = push(acc, start);
      let start = start + 1;
    }
  }
}
  )V0G0N";
  auto program = testProgramWithInput(input);
  auto env = std::make_shared<Env::Environment>();
  auto bag = ASTEvaluator::eval(*program, env);
  std::string itr[] = {"range(1,1000);", "range(1,1000);", "range(1,1000);",
                       "range(1,1000);", "range(1,1000);", "range(1,1000);",
                       "range(1,1000);", "range(1,1000);", "range(1,1000);",
                       "range(1,1000);", "range(1,1000);", "range(1,1000);",
                       "range(1,1000);", "range(1,1000);", "range(1,1000);",
                       "range(1,1000);", "range(1,1000);", "range(1,10000);",};
  for (const auto &str : itr) {
    program = testProgramWithInput(str);
    bag = ASTEvaluator::eval(*program, env);
    std::cout << bag->inspect() << std::endl;
  }
}

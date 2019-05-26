#include "repl.hpp"
#include <eval_dispatcher.hpp>
#include <iostream>
#include <lexer.hpp>
#include <parser.hpp>
#include <print_dispatcher.hpp>
#include <string>

namespace Repl {
void run() {
  const std::string prompt = ">> ";
  const std::string prompt_indent = "   ";
  fmt::print("{}", prompt);
  for (std::string line; std::getline(std::cin, line);) {
    auto lexer = Lexer::from(line);
    auto parser = Parser::from(std::move(lexer));
    auto program = parser->parseProgram();
    if (parser->errors().size() != 0) {
      for (const auto &error : parser->errors()) {
        auto columnNumber = error->token->location->columnNumber;
        std::string padding(error->token->location->columnNumber, ' ');
        fmt::print("{}{}{}\n{}\n", prompt_indent, padding, "^", error->message);
      }
      fmt::print("{}", prompt);
      continue;
    }
    auto evaluated = ASTEvaluator::eval(*program);
    if (evaluated) {
      fmt::print("{}\n", evaluated->inspect());
    }
    /*
    std::stringstream ss;
    ASTPrinter::write([&](std::string message) { ss << message; }, *program);
    fmt::print("{}\n", ss.str());
    */
    fmt::print("{}", prompt);
  }
}
}  // namespace Repl
#include "repl.hpp"
#include <fmt/printf.h>
#include <iostream>
#include <lexer.hpp>
#include <string>
namespace Repl {
void run() {
  const std::string prompt = ">> ";
  fmt::print("{}", prompt);
  for (std::string line; std::getline(std::cin, line);) {
    auto lexer = Lexer::from(line);
    while (auto token = lexer->nextToken()) {
      if (token->type == TokenType::END_OF_FILE) {
        break;
      }
      fmt::print("{}\n", *token);
    }
    fmt::print("{}", prompt);
  }
}
}  // namespace Repl
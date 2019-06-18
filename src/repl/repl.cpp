#include "repl.hpp"
#include <cstdlib>
#include <env.hpp>
#include <eval.hpp>
#include <fstream>
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
  auto env = std::make_shared<Env::Environment>();
  for (std::string line; std::getline(std::cin, line);) {
    if (line.size() > 0 && line.at(0) == '@') {
      auto file = line.substr(1, std::string::npos);
      auto installDir = std::getenv("CMONKEY_INSTALL_DIR");
      if (!installDir) {
        fmt::print("ERROR: CMONKEY_INSTALL_DIR not set!\n");
        fmt::print("{}", prompt);
        continue;
      }
      std::stringstream ss;
      ss << installDir << file << ".monkey";
      fmt::print("Loading {}\n", ss.str());
      std::ifstream ifs(ss.str());
      std::string contents((std::istreambuf_iterator<char>(ifs)),
                           (std::istreambuf_iterator<char>()));
      line = contents;
    }
    auto lexer = std::make_unique<Lexer>(line);
    auto parser = Parser(std::move(lexer));
    auto program = parser.parseProgram();
    if (parser.errors().size() != 0) {
      for (const auto &error : parser.errors()) {
        auto columnNumber = error.token->location->columnNumber;
        std::string padding(error.token->location->columnNumber, ' ');
        fmt::print("{}{}{}\n{}({}, {})\n", prompt_indent, padding, "^",
                   error.message, error.token->location->lineNumber,
                   error.token->location->columnNumber);
      }
      fmt::print("{}", prompt);
      continue;
    }
    auto evaluated = ASTEvaluator::eval(*program, env);
    if (evaluated && evaluated->type() != Eval::Type::NULL_OBJ) {
      fmt::print("{}\n", evaluated->inspect());
    }

    /*
    std::stringstream ss;
    ASTPrinter::write([&](std::string message) { ss << message; },
    *program); fmt::print("{}\n", ss.str());
    */
    fmt::print("{}", prompt);
  }
}
}  // namespace Repl
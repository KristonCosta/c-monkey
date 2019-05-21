#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <lexer.hpp>
#include <random>
#include <repl.hpp>

void registerLogger(std::string name, uint suffix) {
  spdlog::basic_logger_mt(name, fmt::format("{}-{}.log", name, suffix));
}

int main() {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<uint> dist(1, 4294967295);
  auto suffix = dist(mt);
  spdlog::info("Creating log files with suffix: {}", suffix);
  registerLogger(LEXER_LOGGER, suffix);

  Repl::run();
}
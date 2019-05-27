#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <eval.hpp>
#include <iostream>
#include <lexer.hpp>
#include <parser.hpp>
#include <random>
#include <repl.hpp>
#include "spdlog/sinks/stdout_color_sinks.h"

void registerLogger(std::string name, uint suffix) {
  auto logger =
      spdlog::basic_logger_mt(name, fmt::format("{}-{}.log", name, suffix));
  logger->set_level(spdlog::level::info);
}

int main() {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<uint> dist(1, 4294967295);
  auto suffix = dist(mt);
  spdlog::info("Creating log files with suffix: {}", suffix);
  registerLogger(LEXER_LOGGER, suffix);
  registerLogger(PARSER_LOGGER, suffix);
  registerLogger(EVAL_LOGGER, suffix);

  Repl::run();
}
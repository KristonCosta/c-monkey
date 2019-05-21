#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <random>
#include "lexer/lexer.hpp"

int main() {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<uint> dist(1, 4294967295);
  auto suffix = dist(mt);
  spdlog::info("Creating log files with suffix: {}", suffix);
  auto lexer_logger = spdlog::basic_logger_mt(
      LEXER_LOGGER, fmt::format("{}-{}.log", LEXER_LOGGER, suffix));
  lexer_logger->info("Hello world");
}
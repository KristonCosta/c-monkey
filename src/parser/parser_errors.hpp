#pragma once
#include <spdlog/fmt/ostr.h>
#include <string>
#include <token.hpp>
#include <utility>

struct ParserError {
  ParserError(std::shared_ptr<Token> token, std::string message)
      : token(std::move(token)), message(std::move(message)){};
  std::shared_ptr<Token> token;
  std::string message;
  template <typename OStream>
  friend OStream &operator<<(OStream &os, const ParserError &c) {
    return os << "Error: " << c.message << " at " << *c.token;
  }
};
#pragma once
#include <spdlog/fmt/ostr.h>
#include <string>
#include <token.hpp>

struct ParserError {
  ParserError(std::shared_ptr<Token> token, std::string message)
      : token(token), message(message){};
  std::shared_ptr<Token> token;
  std::string message;
  template <typename OStream>
  friend OStream &operator<<(OStream &os, const ParserError &c) {
    return os << "Error: " << c.message << " at " << *c.token;
  }
};
#pragma once
#include <node.hpp>
#include <sstream>
#include <string>
#include <token.hpp>

class Identifier : Expression {
 private:
  std::shared_ptr<Token> token;
  std::string value;

 public:
  Identifier(std::shared_ptr<Token> token, std::string value)
      : token(token), value(value) {}
  virtual std::string literal() const override { return token->literal; };

  std::string toString() const override {
    std::stringstream ss;
    ss << "[identifier";
    if (this->token) {
      ss << " token=" << *this->token;
    }
    ss << " value=" << this->value << "]";
    return ss.str();
  }

  const std::string getValue() { return this->value; }
};
#pragma once
#include <spdlog/spdlog.h>
#include <string>

class Node {
 public:
  virtual std::string literal() const = 0;
  virtual std::string toString() const = 0;
};

class Statement : Node {
 public:
  virtual std::string literal() const override { return "base_statement"; };
  virtual std::string toString() const override {
    return fmt::format("[statement literal={}]", this->literal());
  };
};

class Expression : Node {
 public:
  virtual std::string literal() const override { return "base_expression"; };
  virtual std::string toString() const override {
    return fmt::format("[expression literal={}]", this->literal());
  };
};
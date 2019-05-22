#pragma once
#include <list>
#include <node.hpp>

class Program : Node {
 private:
  std::list<std::shared_ptr<Statement>> statements;

 public:
  virtual std::string literal() const override {
    if (this->statements.size() > 0) {
      return this->statements.front()->literal();
    }
    return "";
  };

  auto begin() { return this->statements.begin(); }

  const uint64_t size() { return this->statements.size(); }

  void addStatement(std::shared_ptr<Statement> statement) {
    this->statements.push_back(statement);
  }

  std::string toString() const override {
    return fmt::format("[program first_token={}]",
                       this->statements.front()->literal());
  }
};
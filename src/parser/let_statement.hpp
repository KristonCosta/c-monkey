#pragma once
#include <identifier.hpp>
#include <node.hpp>
#include <sstream>
#include <token.hpp>

class LetStatement : public Statement {
 private:
  std::shared_ptr<Token> token;
  std::shared_ptr<Identifier> name;
  std::shared_ptr<Expression> value;

 public:
  LetStatement(std::shared_ptr<Token> token)
      : token(token), name(nullptr), value(nullptr){};
  LetStatement(std::shared_ptr<Token> token, std::shared_ptr<Identifier> name,
               std::shared_ptr<Expression> value)
      : token(token), name(name), value(value){};

  virtual std::string literal() const override { return token->literal; };
  virtual std::string toString() const override {
    std::stringstream ss;
    ss << "[letstatement";
    if (this->token) {
      ss << " token=" << *this->token;
    }
    if (this->name) {
      ss << " name=" << this->name->toString();
    }
    if (this->value) {
      ss << " value=" << this->value->toString();
    }
    ss << "]";
    return ss.str();
  }

  void setName(std::shared_ptr<Identifier> name) { this->name = name; }

  void setValue(std::shared_ptr<Expression> value) { this->value = value; }

  const auto getName() { return this->name; }

  const auto getValue() { return this->value; }
};
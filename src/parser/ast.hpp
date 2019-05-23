#pragma once
#include <spdlog/spdlog.h>
#include <list>
#include <sstream>
#include <string>
#include <token.hpp>

typedef std::string Operator;

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

class Identifier : public Expression {
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

class IntegerLiteral : public Expression {
 private:
  std::shared_ptr<Token> token;
  int value;

 public:
  IntegerLiteral(std::shared_ptr<Token> token, int value)
      : token(token), value(value) {}

  virtual std::string literal() const override { return token->literal; };

  std::string toString() const override {
    std::stringstream ss;
    ss << "[integer";
    if (this->token) {
      ss << " token=" << *this->token;
    }
    ss << " value=" << this->value << "]";
    return ss.str();
  }

  const int getValue() { return this->value; }
};

class PrefixExpression : public Expression {
 private:
  std::shared_ptr<Token> token;
  std::shared_ptr<Expression> right;
  Operator op;

 public:
  PrefixExpression(std::shared_ptr<Token> token,
                   std::shared_ptr<Expression> right, Operator op)
      : token(token), right(right), op(op) {}

  virtual std::string literal() const override { return token->literal; };

  std::string toString() const override {
    std::stringstream ss;
    ss << "[prefix";
    if (this->token) {
      ss << " token=" << *this->token;
    }
    if (this->right) {
      ss << " right=" << this->right->toString();
    }
    ss << " op=" << this->op << "]";
    return ss.str();
  }

  const std::shared_ptr<Expression> getRight() { return this->right; }
  const Operator getOp() { return this->op; }
};

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
    std::stringstream ss;
    ss << "[program" << std::endl;
    for (const auto &statement : statements) {
      ss << statement->toString() << std::endl;
    }
    ss << "]";
    return ss.str();
  }
};

class ReturnStatement : public Statement {
 private:
  std::shared_ptr<Token> token;
  std::shared_ptr<Expression> returnValue;

 public:
  ReturnStatement(std::shared_ptr<Token> token) : token(token){};
  virtual std::string literal() const override { return token->literal; };
  std::string toString() const override {
    std::stringstream ss;
    ss << "[return token=" << *this->token;
    if (this->returnValue) {
      ss << " returnValue=" << this->returnValue->toString();
    }
    ss << "]";
    return ss.str();
  }
};

class ExpressionStatement : public Statement {
 private:
  std::shared_ptr<Token> token;
  std::shared_ptr<Expression> expression;

 public:
  ExpressionStatement(std::shared_ptr<Token> token,
                      std::shared_ptr<Expression> expression)
      : token(token), expression(expression){};

  virtual std::string literal() const override { return token->literal; };

  std::string toString() const override {
    std::stringstream ss;
    ss << "[expression token=" << *this->token;
    if (this->expression) {
      ss << " expression=" << this->expression->toString();
    }
    ss << "]";
    return ss.str();
  }

  std::shared_ptr<Expression> getExpression() const { return this->expression; }
};

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
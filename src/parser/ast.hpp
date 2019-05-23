#pragma once
#include <spdlog/spdlog.h>
#include <list>
#include <sstream>
#include <string>
#include <token.hpp>

namespace AST {
typedef std::string Operator;

class Node;
class Statement;
class Expression;
class Program;
class Identifier;
class IntegerLiteral;
class PrefixExpression;
class InfixExpression;
class ReturnStatement;
class ExpressionStatement;
class LetStatement;

class AbstractDispatcher {
 public:
  virtual void dispatch(Node &node) = 0;
  virtual void dispatch(Statement &node) = 0;
  virtual void dispatch(Expression &node) = 0;
  virtual void dispatch(Program &node) = 0;
  virtual void dispatch(Identifier &node) = 0;
  virtual void dispatch(IntegerLiteral &node) = 0;
  virtual void dispatch(PrefixExpression &node) = 0;
  virtual void dispatch(InfixExpression &node) = 0;
  virtual void dispatch(ReturnStatement &node) = 0;
  virtual void dispatch(ExpressionStatement &node) = 0;
  virtual void dispatch(LetStatement &node) = 0;
};

/*

  Base Types

*/
class Node {
 public:
  virtual std::string tokenLiteral() const = 0;
  virtual std::string toDebugString() const = 0;
  virtual void visit(AbstractDispatcher &dispatcher) = 0;
};

class Statement : Node {
 public:
  virtual std::string tokenLiteral() const override;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class Expression : Node {
 public:
  virtual std::string tokenLiteral() const override;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

/*

  High-level node types

*/
class Program : public Node {
 private:
  std::list<std::shared_ptr<Statement>> statements;

 public:
  const std::list<std::shared_ptr<Statement>> &getStatements() const;
  const uint64_t size();
  void addStatement(std::shared_ptr<Statement> statement);
  virtual std::string tokenLiteral() const override;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

/*

  Expression Types

*/

class Identifier : public Expression {
 private:
  std::shared_ptr<Token> token;
  std::string value;

 public:
  Identifier(std::shared_ptr<Token> token, std::string value)
      : token(token), value(value) {}

  const std::string getValue();
  virtual std::string tokenLiteral() const override;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class IntegerLiteral : public Expression {
 private:
  std::shared_ptr<Token> token;
  int value;

 public:
  IntegerLiteral(std::shared_ptr<Token> token, int value)
      : token(token), value(value) {}

  const int getValue();
  virtual std::string tokenLiteral() const override;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

/*

  Expression Types - operators

*/
class PrefixExpression : public Expression {
 private:
  std::shared_ptr<Token> token;
  std::shared_ptr<Expression> right;
  Operator op;

 public:
  PrefixExpression(std::shared_ptr<Token> token,
                   std::shared_ptr<Expression> right, Operator op)
      : token(token), right(right), op(op) {}

  const std::shared_ptr<Expression> getRight();
  const Operator getOp();
  virtual std::string tokenLiteral() const override;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class InfixExpression : public Expression {
 private:
  std::shared_ptr<Token> token;
  std::shared_ptr<Expression> left;
  std::shared_ptr<Expression> right;
  Operator op;

 public:
  InfixExpression(std::shared_ptr<Token> token,
                  std::shared_ptr<Expression> left,
                  std::shared_ptr<Expression> right, Operator op)
      : token(token), left(left), right(right), op(op) {}

  const std::shared_ptr<Expression> getLeft();
  const std::shared_ptr<Expression> getRight();
  const Operator getOp();
  virtual std::string tokenLiteral() const override;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

/*

  Statement Types

*/
class ReturnStatement : public Statement {
 private:
  std::shared_ptr<Token> token;
  std::shared_ptr<Expression> returnValue;

 public:
  ReturnStatement(std::shared_ptr<Token> token) : token(token){};
  std::shared_ptr<Expression> getReturnValue() const;
  virtual std::string tokenLiteral() const override;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
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

  std::shared_ptr<Expression> getExpression() const;
  virtual std::string tokenLiteral() const override;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
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

  void setName(std::shared_ptr<Identifier> name);
  void setValue(std::shared_ptr<Expression> value);
  const std::shared_ptr<Identifier> getName();
  const std::shared_ptr<Expression> getValue();
  virtual std::string tokenLiteral() const override;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

}  // namespace AST
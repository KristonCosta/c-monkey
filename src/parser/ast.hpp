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
class Boolean;
class PrefixExpression;
class InfixExpression;
class IfExpression;
class CallExpression;

class ReturnStatement;
class ExpressionStatement;
class LetStatement;
class BlockStatement;
class FunctionLiteral;

class AbstractDispatcher {
 public:
  virtual void dispatch(Node &node) = 0;
  virtual void dispatch(Statement &node) = 0;
  virtual void dispatch(Expression &node) = 0;

  virtual void dispatch(Program &node) = 0;

  virtual void dispatch(Identifier &node) = 0;
  virtual void dispatch(IntegerLiteral &node) = 0;
  virtual void dispatch(Boolean &node) = 0;
  virtual void dispatch(PrefixExpression &node) = 0;
  virtual void dispatch(InfixExpression &node) = 0;
  virtual void dispatch(IfExpression &node) = 0;
  virtual void dispatch(FunctionLiteral &node) = 0;
  virtual void dispatch(CallExpression &node) = 0;

  virtual void dispatch(ReturnStatement &node) = 0;
  virtual void dispatch(ExpressionStatement &node) = 0;
  virtual void dispatch(LetStatement &node) = 0;
  virtual void dispatch(BlockStatement &node) = 0;
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
 protected:
  std::shared_ptr<Token> token;

 public:
  const std::shared_ptr<Token> getToken() const { return this->token; };
  virtual std::string tokenLiteral() const override {
    return this->token->literal;
  }
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class Expression : Node {
 protected:
  std::shared_ptr<Token> token;

 public:
  const std::shared_ptr<Token> getToken() const { return this->token; };
  virtual std::string tokenLiteral() const override {
    return this->token->literal;
  }
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
  std::string value;

 public:
  Identifier(std::shared_ptr<Token> token, std::string value) : value(value) {
    this->token = token;
  }

  const std::string getValue();
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class IntegerLiteral : public Expression {
 private:
  int value;

 public:
  IntegerLiteral(std::shared_ptr<Token> token, int value) : value(value) {
    this->token = token;
  }

  const int getValue();
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class Boolean : public Expression {
 private:
  bool value;

 public:
  Boolean(std::shared_ptr<Token> token, bool value) : value(value) {
    this->token = token;
  };
  const bool getValue();
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
  std::shared_ptr<Expression> right;
  Operator op;

 public:
  PrefixExpression(std::shared_ptr<Token> token,
                   std::shared_ptr<Expression> right, Operator op)
      : right(right), op(op) {
    this->token = token;
  }

  const std::shared_ptr<Expression> getRight();
  const Operator getOp();
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class InfixExpression : public Expression {
 private:
  std::shared_ptr<Expression> left;
  std::shared_ptr<Expression> right;
  Operator op;

 public:
  InfixExpression(std::shared_ptr<Token> token,
                  std::shared_ptr<Expression> left,
                  std::shared_ptr<Expression> right, Operator op)
      : left(left), right(right), op(op) {
    this->token = token;
  }

  const std::shared_ptr<Expression> getLeft();
  const std::shared_ptr<Expression> getRight();
  const Operator getOp();
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

/*

  Expression Types - complex

*/
class IfExpression : public Expression {
  std::shared_ptr<Expression> condition;
  std::shared_ptr<BlockStatement> whenTrue;
  std::shared_ptr<BlockStatement> whenFalse;

 public:
  IfExpression(std::shared_ptr<Token> token,
               std::shared_ptr<Expression> condition,
               std::shared_ptr<BlockStatement> whenTrue,
               std::shared_ptr<BlockStatement> whenFalse)
      : condition(condition), whenTrue(whenTrue), whenFalse(whenFalse) {
    this->token = token;
  };
  std::shared_ptr<Expression> &getCondition();
  std::shared_ptr<BlockStatement> &getWhenTrue();
  std::shared_ptr<BlockStatement> &getWhenFalse();
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class FunctionLiteral : public Expression {
  std::list<std::shared_ptr<Identifier>> arguments;
  std::shared_ptr<BlockStatement> body;

 public:
  FunctionLiteral(std::shared_ptr<Token> token,
                  std::shared_ptr<BlockStatement> body)
      : body(body) {
    this->token = token;
  };
  const std::list<std::shared_ptr<Identifier>> &getArguments() const;
  const uint64_t size();
  void addArgument(std::shared_ptr<Identifier> identifier);
  std::shared_ptr<BlockStatement> &getBody();
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class CallExpression : public Expression {
  std::shared_ptr<Expression> name;
  std::list<std::shared_ptr<Expression>> arguments;

 public:
  CallExpression(std::shared_ptr<Token> token, std::shared_ptr<Expression> name)
      : name(name) {
    this->token = token;
  };
  const std::list<std::shared_ptr<Expression>> &getArguments() const;
  const uint64_t size();
  void addArgument(std::shared_ptr<Expression> expr);
  std::shared_ptr<Expression> getName();
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
  std::shared_ptr<Expression> returnValue;

 public:
  ReturnStatement(std::shared_ptr<Token> token,
                  std::shared_ptr<Expression> returnValue)
      : returnValue(returnValue) {
    this->token = token;
  };
  std::shared_ptr<Expression> getReturnValue() const;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class ExpressionStatement : public Statement {
 private:
  std::shared_ptr<Expression> expression;

 public:
  ExpressionStatement(std::shared_ptr<Token> token,
                      std::shared_ptr<Expression> expression)
      : expression(expression) {
    this->token = token;
  };

  std::shared_ptr<Expression> getExpression() const;
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class LetStatement : public Statement {
 private:
  std::shared_ptr<Identifier> name;
  std::shared_ptr<Expression> value;

 public:
  LetStatement(std::shared_ptr<Token> token, std::shared_ptr<Identifier> name,
               std::shared_ptr<Expression> value)
      : name(name), value(value) {
    this->token = token;
  };

  const std::shared_ptr<Identifier> getName();
  const std::shared_ptr<Expression> getValue();
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

class BlockStatement : public Statement {
 private:
  std::list<std::shared_ptr<Statement>> statements;

 public:
  BlockStatement(std::shared_ptr<Token> token) { this->token = token; };
  const std::list<std::shared_ptr<Statement>> &getStatements() const;
  const uint64_t size();
  void addStatement(std::shared_ptr<Statement> statement);
  virtual std::string toDebugString() const override;
  void visit(AbstractDispatcher &dispatcher) override {
    dispatcher.dispatch(*this);
  }
};

}  // namespace AST
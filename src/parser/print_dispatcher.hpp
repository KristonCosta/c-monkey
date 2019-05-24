#pragma once
#include <spdlog/fmt/ostr.h>
#include <sstream>
#include <string>
#include "ast.hpp"
typedef std::function<void(std::string)> WriterFn;

class ASTPrinter : public AST::AbstractDispatcher {
 private:
  WriterFn writer;

  ASTPrinter(WriterFn writer) : writer(writer){};
  virtual void dispatch(AST::Node &node) override {
    writer(node.tokenLiteral());
  };
  virtual void dispatch(AST::Statement &node) override {
    writer(node.tokenLiteral());
  };
  virtual void dispatch(AST::Expression &node) override {
    writer(node.tokenLiteral());
  };
  virtual void dispatch(AST::Program &node) override {
    for (const auto &statement : node.getStatements()) {
      statement.get()->visit(*this);
    }
  };
  virtual void dispatch(AST::Identifier &node) override {
    writer(node.getValue());
  };
  virtual void dispatch(AST::Boolean &node) override {
    writer(node.tokenLiteral());
  };
  virtual void dispatch(AST::IntegerLiteral &node) override {
    writer(node.tokenLiteral());
  };
  virtual void dispatch(AST::PrefixExpression &node) override {
    writer("(");
    writer(node.getOp());
    node.getRight()->visit(*this);
    writer(")");
  };
  virtual void dispatch(AST::InfixExpression &node) override {
    writer("(");
    node.getLeft()->visit(*this);
    writer(" ");
    writer(node.getOp());
    writer(" ");
    if (node.getRight()) {
      node.getRight()->visit(*this);
    }
    writer(")");
  };
  virtual void dispatch(AST::IfExpression &node) override {
    writer("if ");
    node.getCondition()->visit(*this);
    writer(" ");
    node.getWhenTrue()->visit(*this);
    if (node.getWhenFalse()) {
      writer("else ");
      node.getWhenFalse()->visit(*this);
    }
  };
  virtual void dispatch(AST::ReturnStatement &node) override {
    writer(node.tokenLiteral());
    if (node.getReturnValue()) {
      writer(" ");
      node.getReturnValue()->visit(*this);
    }
    writer(";");
  };
  virtual void dispatch(AST::ExpressionStatement &node) override {
    node.getExpression()->visit(*this);
  };
  virtual void dispatch(AST::LetStatement &node) override {
    writer(node.tokenLiteral());
    writer(" ");
    node.getName()->visit(*this);
    writer(" = ");
    node.getValue()->visit(*this);
    writer(";");
  };
  virtual void dispatch(AST::BlockStatement &node) override{};

 public:
  static void write(WriterFn writer, AST::Node &n) {
    auto printer = new ASTPrinter(writer);
    n.visit(*printer);
    delete printer;
  }

  template <typename OStream>
  friend OStream &operator<<(OStream &os, const AST::Node &n) {
    write([os](std::string message) { os << message; }, n);
    return os;
  }
};
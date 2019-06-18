#pragma once
#include <spdlog/fmt/ostr.h>
#include <sstream>
#include <string>
#include "ast.hpp"
typedef std::function<void(std::string)> WriterFn;

class ASTPrinter : public AST::AbstractDispatcher {
 private:
  WriterFn writer;
  uint8_t indentLevel;
  explicit ASTPrinter(WriterFn writer) : indentLevel(0), writer(writer) {}
  static const uint8_t PADDING = 2;
  std::string getPadding() {
    std::string padding(indentLevel * PADDING, ' ');
    return padding;
  }

  void increaseIndent() { indentLevel++; }

  void decreaseIndent() { indentLevel--; }

 public:
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
  virtual void dispatch(AST::HashLiteral &node) override {
    writer("{");
    auto pairs = node.getPairs();
    for (auto pair = pairs.begin(); pair != pairs.end(); ++pair) {
      if (pair != pairs.begin()) {
        writer(", ");
      }
      pair->first->visit(*this);
      writer(":");
      pair->second->visit(*this);
    }
    writer("}");
  };

  virtual void dispatch(AST::IntegerLiteral &node) override {
    writer(node.tokenLiteral());
  };
  virtual void dispatch(AST::StringLiteral &node) override {
    writer("\"" + node.tokenLiteral() + "\"");
  }
  virtual void dispatch(AST::ArrayLiteral &node) override {
    writer("[");
    auto args = node.getValues();
    for (auto arg = args.begin(); arg != args.end(); ++arg) {
      if (arg != args.begin()) {
        writer(", ");
      }
      arg->get()->visit(*this);
    }
    writer("]");
  }
  virtual void dispatch(AST::IndexExpression &node) override {
    writer("(");
    node.getLeft()->visit(*this);
    writer("[");
    node.getIndex()->visit(*this);
    writer("])");
  }
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
      writer(" else ");
      node.getWhenFalse()->visit(*this);
    }
  };
  virtual void dispatch(AST::WhileExpression &node) override {
    writer("while ");
    node.getBody()->visit(*this);
  };
  virtual void dispatch(AST::FunctionLiteral &node) override {
    writer(fmt::format("{}(", node.tokenLiteral()));
    auto args = node.getArguments();
    for (auto arg = args.begin(); arg != args.end(); ++arg) {
      if (arg != args.begin()) {
        writer(", ");
      }
      arg->get()->visit(*this);
    }
    writer(") ");
    node.getBody()->visit(*this);
  }
  virtual void dispatch(AST::CallExpression &node) override {
    node.getFunction()->visit(*this);
    writer("(");
    auto args = node.getArguments();
    for (auto arg = args.begin(); arg != args.end(); ++arg) {
      if (arg != args.begin()) {
        writer(", ");
      }
      arg->get()->visit(*this);
    }
    writer(")");
  }
  virtual void dispatch(AST::ReturnStatement &node) override {
    writer(node.tokenLiteral());
    if (node.getReturnValue()) {
      writer(" ");
      node.getReturnValue()->visit(*this);
    }
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
  };
  virtual void dispatch(AST::BlockStatement &node) override {
    writer("{ ");
    increaseIndent();
    auto stmts = node.getStatements();
    for (auto stmt = stmts.begin(); stmt != stmts.end(); ++stmt) {
      writer("\n" + getPadding());
      stmt->get()->visit(*this);
      if (stmt->get() != stmts.back().get()) {
        writer(";");
      }
    }
    decreaseIndent();
    writer("\n" + getPadding() + "}");
  };

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

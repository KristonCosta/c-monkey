#pragma once
#include <spdlog/fmt/ostr.h>
#include <bag.hpp>
#include <sstream>
#include <string>
#include "ast.hpp"
#include "spdlog/sinks/null_sink.h"

const std::string EVAL_LOGGER = "eval";

class ASTEvaluator : public AST::AbstractDispatcher {
 private:
  ASTEvaluator() {
    if (!spdlog::get(EVAL_LOGGER)) {
      spdlog::create<spdlog::sinks::null_sink_st>(EVAL_LOGGER);
    }
  };
  std::shared_ptr<Eval::Bag> bag = nullptr;

 public:
  virtual void dispatch(AST::Node &node) override{

  };
  virtual void dispatch(AST::Statement &node) override{

  };
  virtual void dispatch(AST::Expression &node) override{

  };
  virtual void dispatch(AST::Program &node) override {
    spdlog::get(EVAL_LOGGER)->info("Evaluating program");

    bag = std::make_shared<Eval::BaseBag>("no result");
    for (const auto &statement : node.getStatements()) {
      spdlog::get(EVAL_LOGGER)->info("Processing statement.");
      bag = eval(*statement.get());
      spdlog::get(EVAL_LOGGER)->info("Returning expression {}", bag);
    }
  };
  virtual void dispatch(AST::Identifier &node) override{

  };
  virtual void dispatch(AST::Boolean &node) override {
    spdlog::get(EVAL_LOGGER)->info("Creating boolean {}", node.getValue());
    bag = std::make_shared<Eval::BooleanBag>(node.getValue());
  };
  virtual void dispatch(AST::IntegerLiteral &node) override {
    spdlog::get(EVAL_LOGGER)
        ->info("Creating integer literal {}", node.getValue());
    this->bag = std::make_shared<Eval::IntegerBag>(node.getValue());
    spdlog::get(EVAL_LOGGER)->info("Returning integer literal {}", bag);
  };
  virtual void dispatch(AST::PrefixExpression &node) override {
    switch (node.getOp()) {
      case "!":
        bag =
    }
  };
  virtual void dispatch(AST::InfixExpression &node) override{

  };
  virtual void dispatch(AST::IfExpression &node) override{

  };
  virtual void dispatch(AST::FunctionLiteral &node) override {}
  virtual void dispatch(AST::CallExpression &node) override {}
  virtual void dispatch(AST::ReturnStatement &node) override{

  };
  virtual void dispatch(AST::ExpressionStatement &node) override {
    this->bag = eval(*node.getExpression());
    spdlog::get(EVAL_LOGGER)->info("Returning expression {}", bag);
  };
  virtual void dispatch(AST::LetStatement &node) override{

  };
  virtual void dispatch(AST::BlockStatement &node) override{

  };

  static std::shared_ptr<Eval::Bag> eval(AST::Node &n) {
    auto eval = new ASTEvaluator();
    n.visit(*eval);
    auto bag = eval->bag;
    delete eval;
    return (bag);
  }
};

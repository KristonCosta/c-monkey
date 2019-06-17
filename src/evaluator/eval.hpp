#pragma once
#include <spdlog/fmt/ostr.h>
#include <bag.hpp>
#include <sstream>
#include <string>
#include "ast.hpp"
#include "env.hpp"
#include "output.hpp"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

const std::string EVAL_LOGGER = "eval";

class ASTEvaluator : public AST::AbstractDispatcher {
 private:
  explicit ASTEvaluator(std::shared_ptr<Env::Environment> env) : env(env) {
    if (!spdlog::get(EVAL_LOGGER)) {
      spdlog::create<spdlog::sinks::null_sink_st>(EVAL_LOGGER);
    }
    if (!spdlog::get(EVAL_OUTPUT)) {
      auto output = spdlog::stdout_color_mt(EVAL_OUTPUT);
      output->set_pattern("%v");
      output->flush_on(spdlog::level::trace);
    }
  };
  std::shared_ptr<Eval::Bag> bag = nullptr;
  std::shared_ptr<Env::Environment> env;

 public:
  virtual void dispatch(AST::Node &node) override;
  virtual void dispatch(AST::Statement &node) override;
  virtual void dispatch(AST::Expression &node) override;
  virtual void dispatch(AST::Program &node) override;
  virtual void dispatch(AST::Identifier &node) override;
  virtual void dispatch(AST::Boolean &node) override;
  virtual void dispatch(AST::HashLiteral &node) override;
  virtual void dispatch(AST::StringLiteral &node) override;
  virtual void dispatch(AST::ArrayLiteral &node) override;
  virtual void dispatch(AST::IntegerLiteral &node) override;
  virtual void dispatch(AST::IndexExpression &node) override;
  virtual void dispatch(AST::PrefixExpression &node) override;
  virtual void dispatch(AST::InfixExpression &node) override;
  virtual void dispatch(AST::IfExpression &node) override;
  virtual void dispatch(AST::FunctionLiteral &node) override;
  virtual void dispatch(AST::CallExpression &node) override;
  virtual void dispatch(AST::ReturnStatement &node) override;
  virtual void dispatch(AST::ExpressionStatement &node) override;
  virtual void dispatch(AST::LetStatement &node) override;
  virtual void dispatch(AST::BlockStatement &node) override;

  static std::shared_ptr<Eval::Bag> eval(
      AST::Node &n, std::shared_ptr<Env::Environment> env) {
    auto eval = new ASTEvaluator(env);
    n.visit(*eval);
    auto bag = eval->bag;
    delete eval;
    return (bag);
  }
};

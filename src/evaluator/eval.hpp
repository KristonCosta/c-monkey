#pragma once
#include <spdlog/fmt/ostr.h>
#include <bag.hpp>
#include <sstream>
#include <string>
#include <utility>
#include "ast.hpp"
#include "env.hpp"
#include "output.hpp"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

const std::string EVAL_LOGGER = "eval";

class ASTEvaluator : public AST::AbstractDispatcher {
 private:
  explicit ASTEvaluator(std::shared_ptr<Env::Environment> env) : env(std::move(env)) {
    if (!spdlog::get(EVAL_LOGGER)) {
      spdlog::create<spdlog::sinks::null_sink_st>(EVAL_LOGGER);
    }
    if (!spdlog::get(EVAL_OUTPUT)) {
      auto output = spdlog::stdout_color_mt(EVAL_OUTPUT);
      output->set_pattern("%v");
      output->flush_on(spdlog::level::info);
    }
  };
  std::shared_ptr<Eval::Bag> bag = nullptr;
  std::shared_ptr<Env::Environment> env;

 public:
  void dispatch(AST::Node &node) override;
  void dispatch(AST::Statement &node) override;
  void dispatch(AST::Expression &node) override;
  void dispatch(AST::Program &node) override;
  void dispatch(AST::Identifier &node) override;
  void dispatch(AST::Boolean &node) override;
  void dispatch(AST::HashLiteral &node) override;
  void dispatch(AST::StringLiteral &node) override;
  void dispatch(AST::ArrayLiteral &node) override;
  void dispatch(AST::IntegerLiteral &node) override;
  void dispatch(AST::IndexExpression &node) override;
  void dispatch(AST::PrefixExpression &node) override;
  void dispatch(AST::InfixExpression &node) override;
  void dispatch(AST::IfExpression &node) override;
  void dispatch(AST::WhileExpression &node) override;
  void dispatch(AST::FunctionLiteral &node) override;
  void dispatch(AST::CallExpression &node) override;
  void dispatch(AST::ReturnStatement &node) override;
  void dispatch(AST::ExpressionStatement &node) override;
  void dispatch(AST::LetStatement &node) override;
  void dispatch(AST::BlockStatement &node) override;

  static std::shared_ptr<Eval::Bag> eval(
      AST::Node &n, std::shared_ptr<Env::Environment> env) {
    auto eval = new ASTEvaluator(std::move(env));
    n.visit(*eval);
    auto bag = eval->bag;
    delete eval;
    return (bag);
  }
};

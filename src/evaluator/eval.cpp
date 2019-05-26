#include <spdlog/fmt/ostr.h>
#include <bag.hpp>
#include <eval.hpp>
#include <sstream>
#include <string>
#include "ast.hpp"
#include "spdlog/sinks/null_sink.h"

const std::shared_ptr<Eval::BooleanBag> TRUE_BAG =
    std::make_shared<Eval::BooleanBag>(true);

const std::shared_ptr<Eval::BooleanBag> FALSE_BAG =
    std::make_shared<Eval::BooleanBag>(false);

const std::shared_ptr<Eval::NullBag> NULL_BAG =
    std::shared_ptr<Eval::NullBag>();

std::shared_ptr<Eval::BooleanBag> evalBangOperator(
    std::shared_ptr<Eval::Bag> right) {
  switch (right->type()) {
    case Eval::Type::BOOLEAN_OBJ: {
      auto bag = Eval::convertToBoolean(right);
      if (bag->value()) {
        return FALSE_BAG;
      }
      return TRUE_BAG;
    }
    case Eval::Type::NULL_OBJ:
      return TRUE_BAG;
    default:
      return FALSE_BAG;
  };
  return nullptr;
}

void ASTEvaluator::dispatch(AST::Node &node){

};
void ASTEvaluator::dispatch(AST::Statement &node){

};
void ASTEvaluator::dispatch(AST::Expression &node){

};
void ASTEvaluator::dispatch(AST::Program &node) {
  spdlog::get(EVAL_LOGGER)->info("Evaluating program");

  bag = std::make_shared<Eval::BaseBag>("no result");
  for (const auto &statement : node.getStatements()) {
    spdlog::get(EVAL_LOGGER)->info("Processing statement");
    bag = eval(*statement.get());
  }
};
void ASTEvaluator::dispatch(AST::Identifier &node){

};
void ASTEvaluator::dispatch(AST::Boolean &node) {
  spdlog::get(EVAL_LOGGER)->info("Creating boolean {}", node.getValue());
  bag = std::make_shared<Eval::BooleanBag>(node.getValue());
};
void ASTEvaluator::dispatch(AST::IntegerLiteral &node) {
  spdlog::get(EVAL_LOGGER)
      ->info("Creating integer literal {}", node.getValue());
  this->bag = std::make_shared<Eval::IntegerBag>(node.getValue());
};
void ASTEvaluator::dispatch(AST::PrefixExpression &node) {
  spdlog::get(EVAL_LOGGER)
      ->info("Evaluating prefix expression {}", node.getOp());
  if (node.getOp() == "!") {
    bag = evalBangOperator(eval(*node.getRight()));
    spdlog::get(EVAL_LOGGER)
        ->info("Evaluated bang operator to {}", bag->inspect());
  }
};
void ASTEvaluator::dispatch(AST::InfixExpression &node){

};
void ASTEvaluator::dispatch(AST::IfExpression &node){

};
void ASTEvaluator::dispatch(AST::FunctionLiteral &node) {}
void ASTEvaluator::dispatch(AST::CallExpression &node) {}
void ASTEvaluator::dispatch(AST::ReturnStatement &node){

};
void ASTEvaluator::dispatch(AST::ExpressionStatement &node) {
  spdlog::get(EVAL_LOGGER)
      ->info("Evaluating expression statement {}", node.tokenLiteral());
  this->bag = eval(*node.getExpression());
};
void ASTEvaluator::dispatch(AST::LetStatement &node){

};
void ASTEvaluator::dispatch(AST::BlockStatement &node){

};

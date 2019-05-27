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
    std::make_shared<Eval::NullBag>();

std::shared_ptr<Eval::IntegerBag> makeIntegerBag(int64_t value) {
  return std::make_shared<Eval::IntegerBag>(value);
}

std::shared_ptr<Eval::BooleanBag> getBooleanBag(bool val) {
  if (val) {
    return TRUE_BAG;
  }
  return FALSE_BAG;
}

std::shared_ptr<Eval::Bag> evalBangOperator(std::shared_ptr<Eval::Bag> right) {
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
  return NULL_BAG;
}

std::shared_ptr<Eval::Bag> evalNegateOperator(
    std::shared_ptr<Eval::Bag> right) {
  if (right->type() != Eval::Type::INTEGER_OBJ) {
    return NULL_BAG;
  }
  return std::make_shared<Eval::IntegerBag>(convertToInteger(right)->value() *
                                            -1);
}

std::shared_ptr<Eval::Bag> evalIntegerInfixExpression(
    std::string op, std::shared_ptr<Eval::IntegerBag> left,
    std::shared_ptr<Eval::IntegerBag> right) {
  if (op == "+") {
    return makeIntegerBag(left->value() + right->value());
  } else if (op == "-") {
    return makeIntegerBag(left->value() - right->value());
  } else if (op == "*") {
    return makeIntegerBag(left->value() * right->value());
  } else if (op == "/") {
    return makeIntegerBag(left->value() / right->value());
  } else if (op == "<") {
    return getBooleanBag(left->value() < right->value());
  } else if (op == ">") {
    return getBooleanBag(left->value() > right->value());
  } else if (op == "==") {
    return getBooleanBag(left->value() == right->value());
  } else if (op == "!=") {
    return getBooleanBag(left->value() != right->value());
  }
  return NULL_BAG;
}

std::shared_ptr<Eval::Bag> evalBooleanInfixExpression(
    std::string op, std::shared_ptr<Eval::BooleanBag> left,
    std::shared_ptr<Eval::BooleanBag> right) {
  if (op == "==") {
    return getBooleanBag(left->value() == right->value());
  } else if (op == "!=") {
    return getBooleanBag(left->value() != right->value());
  }
  return NULL_BAG;
}

std::shared_ptr<Eval::Bag> evalInfixExpression(
    std::string op, std::shared_ptr<Eval::Bag> left,
    std::shared_ptr<Eval::Bag> right) {
  switch (left->type()) {
    case Eval::Type::INTEGER_OBJ: {
      if (right->type() == Eval::Type::INTEGER_OBJ) {
        return evalIntegerInfixExpression(op, convertToInteger(left),
                                          convertToInteger(right));
      }
      return NULL_BAG;
    }
    case Eval::Type::BOOLEAN_OBJ: {
      if (right->type() == Eval::Type::BOOLEAN_OBJ) {
        return evalBooleanInfixExpression(op, convertToBoolean(left),
                                          convertToBoolean(right));
      }
      return NULL_BAG;
    }
    default:
      return NULL_BAG;
  }
}

bool isTruthy(Eval::Bag &bag) {
  switch (bag.type()) {
    case Eval::Type::BOOLEAN_OBJ: {
      return static_cast<Eval::BooleanBag &>(bag).value();
    }
    case Eval::Type::NULL_OBJ:
      return false;
    default:
      return true;
  }
}

std::shared_ptr<Eval::Bag> evalIfExpression(AST::IfExpression &node) {
  std::shared_ptr<Eval::Bag> bag = NULL_BAG;
  spdlog::get(EVAL_LOGGER)
      ->info("Evaluating when {} expression", Eval::typeToString(bag->type()));

  auto condition = ASTEvaluator::eval(*node.getCondition());
  if (isTruthy(*condition)) {
    spdlog::get(EVAL_LOGGER)->info("Evaluating when true expression");
    bag = ASTEvaluator::eval(*node.getWhenTrue());
  } else if (node.getWhenFalse()) {
    spdlog::get(EVAL_LOGGER)->info("Evaluating when false expression");
    bag = ASTEvaluator::eval(*node.getWhenFalse());
  }
  return bag;
}

std::shared_ptr<Eval::Bag> evalStatements(
    std::list<std::shared_ptr<AST::Statement>> &statements) {
  std::shared_ptr<Eval::Bag> bag = NULL_BAG;
  for (const auto &statement : statements) {
    spdlog::get(EVAL_LOGGER)
        ->info("Processing statement {}", statement.get()->toDebugString());
    bag = ASTEvaluator::eval(*statement.get());
  }
  return bag;
}

void ASTEvaluator::dispatch(AST::Node &node){

};
void ASTEvaluator::dispatch(AST::Statement &node){

};
void ASTEvaluator::dispatch(AST::Expression &node){

};
void ASTEvaluator::dispatch(AST::Program &node) {
  spdlog::get(EVAL_LOGGER)->info("Evaluating program");
  auto statements = node.getStatements();
  bag = evalStatements(statements);
};
void ASTEvaluator::dispatch(AST::Identifier &node){

};
void ASTEvaluator::dispatch(AST::Boolean &node) {
  spdlog::get(EVAL_LOGGER)->info("Fetching boolean {}", node.getValue());
  bag = std::make_shared<Eval::BooleanBag>(node.getValue());
};
void ASTEvaluator::dispatch(AST::IntegerLiteral &node) {
  spdlog::get(EVAL_LOGGER)
      ->info("Creating integer literal {}", node.getValue());
  bag = std::make_shared<Eval::IntegerBag>(node.getValue());
};
void ASTEvaluator::dispatch(AST::PrefixExpression &node) {
  spdlog::get(EVAL_LOGGER)
      ->info("Evaluating prefix expression {}", node.getOp());
  if (node.getOp() == "!") {
    bag = evalBangOperator(eval(*node.getRight()));
  } else if (node.getOp() == "-") {
    bag = evalNegateOperator(eval(*node.getRight()));
  }
};
void ASTEvaluator::dispatch(AST::InfixExpression &node) {
  bag = evalInfixExpression(node.getOp(), eval(*node.getLeft()),
                            eval(*node.getRight()));
};
void ASTEvaluator::dispatch(AST::IfExpression &node) {
  bag = evalIfExpression(node);
  spdlog::get(EVAL_LOGGER)
      ->info("Returning if of type {}", Eval::typeToString(bag->type()));
};
void ASTEvaluator::dispatch(AST::FunctionLiteral &node) {}
void ASTEvaluator::dispatch(AST::CallExpression &node) {}
void ASTEvaluator::dispatch(AST::ReturnStatement &node){

};
void ASTEvaluator::dispatch(AST::ExpressionStatement &node) {
  spdlog::get(EVAL_LOGGER)
      ->info("Evaluating expression statement {}", node.tokenLiteral());
  bag = eval(*node.getExpression());
};
void ASTEvaluator::dispatch(AST::LetStatement &node){

};
void ASTEvaluator::dispatch(AST::BlockStatement &node) {
  spdlog::get(EVAL_LOGGER)->info("Evaluating block expression");
  auto statements = node.getStatements();
  bag = evalStatements(statements);
};

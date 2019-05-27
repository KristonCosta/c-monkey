#include <spdlog/fmt/ostr.h>
#include <bag.hpp>
#include <eval.hpp>
#include <eval_errors.hpp>
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

std::shared_ptr<Eval::ReturnBag> makeReturnBag(
    std::shared_ptr<Eval::Bag> value) {
  return std::make_shared<Eval::ReturnBag>(value);
}

std::shared_ptr<Eval::FunctionBag> makeFunctionBag(
    Env::Environment env, std::list<std::shared_ptr<AST::Identifier>> arguments,
    std::shared_ptr<AST::BlockStatement> body) {
  return std::make_shared<Eval::FunctionBag>(env, arguments, body);
}

std::shared_ptr<Eval::BooleanBag> getBooleanBag(bool val) {
  if (val) {
    return TRUE_BAG;
  }
  return FALSE_BAG;
}

bool isError(std::shared_ptr<Eval::Bag> bag) {
  if (bag) {
    return bag->type() == Eval::Type::ERROR_OBJ;
  }
  return false;
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
  return makePrefixOperatorError(right->type(), "!");
}

std::shared_ptr<Eval::Bag> evalNegateOperator(
    std::shared_ptr<Eval::Bag> right) {
  if (right->type() != Eval::Type::INTEGER_OBJ) {
    return makePrefixOperatorError(right->type(), "-");
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
  return makeInfixUnknownOperatorError(left->type(), right->type(), op);
}

std::shared_ptr<Eval::Bag> evalBooleanInfixExpression(
    std::string op, std::shared_ptr<Eval::BooleanBag> left,
    std::shared_ptr<Eval::BooleanBag> right) {
  if (op == "==") {
    return getBooleanBag(left->value() == right->value());
  } else if (op == "!=") {
    return getBooleanBag(left->value() != right->value());
  }
  return makeInfixUnknownOperatorError(left->type(), right->type(), op);
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
      break;
    }
    case Eval::Type::BOOLEAN_OBJ: {
      if (right->type() == Eval::Type::BOOLEAN_OBJ) {
        return evalBooleanInfixExpression(op, convertToBoolean(left),
                                          convertToBoolean(right));
      }
      break;
    }
    default:
      // continue..
      break;
  }
  if (left->type() != right->type()) {
    return makeInfixTypeMismatchError(left->type(), right->type(), op);
  } else {
    return makeInfixUnknownOperatorError(left->type(), right->type(), op);
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

std::shared_ptr<Eval::Bag> evalIfExpression(
    AST::IfExpression &node, std::shared_ptr<Env::Environment> env) {
  std::shared_ptr<Eval::Bag> bag = NULL_BAG;
  spdlog::get(EVAL_LOGGER)
      ->info("Evaluating when {} expression", Eval::typeToString(bag->type()));

  auto condition = ASTEvaluator::eval(*node.getCondition(), env);
  if (isError(condition)) {
    return condition;
  }
  if (isTruthy(*condition)) {
    spdlog::get(EVAL_LOGGER)->info("Evaluating when true expression");
    bag = ASTEvaluator::eval(*node.getWhenTrue(), env);
  } else if (node.getWhenFalse()) {
    spdlog::get(EVAL_LOGGER)->info("Evaluating when false expression");
    bag = ASTEvaluator::eval(*node.getWhenFalse(), env);
  }
  return bag;
}

std::shared_ptr<Eval::Bag> evalProgram(
    std::list<std::shared_ptr<AST::Statement>> &statements,
    std::shared_ptr<Env::Environment> env) {
  std::shared_ptr<Eval::Bag> bag = NULL_BAG;
  for (const auto &statement : statements) {
    bag = ASTEvaluator::eval(*statement.get(), env);
    if (!bag) {
      continue;
    }
    if (bag->type() == Eval::Type::RETURN_OBJ) {
      return convertToReturn(bag)->value();
    }
    if (bag->type() == Eval::Type::ERROR_OBJ) {
      return bag;
    }
  }
  return bag;
}

std::shared_ptr<Eval::Bag> evalBlockStatement(
    std::list<std::shared_ptr<AST::Statement>> &statements,
    std::shared_ptr<Env::Environment> env) {
  std::shared_ptr<Eval::Bag> bag = NULL_BAG;
  for (const auto &statement : statements) {
    bag = ASTEvaluator::eval(*statement.get(), env);
    if (bag && ((bag->type() == Eval::Type::RETURN_OBJ) ||
                (bag->type() == Eval::Type::ERROR_OBJ))) {
      return bag;
    }
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
  bag = evalProgram(statements, env);
  spdlog::get(EVAL_LOGGER)->info("Finished evaulating program");
};
void ASTEvaluator::dispatch(AST::Identifier &node) {
  spdlog::get(EVAL_LOGGER)->info("Fetching identifier {}", node.getValue());
  auto val = env->get(node.getValue());
  if (val) {
    bag = val;
  } else {
    bag = makeIdentifierNotFoundError(node.getValue());
  }
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
    auto right = eval(*node.getRight(), env);
    if (isError(right)) {
      bag = right;
      return;
    }
    bag = evalBangOperator(right);
  } else if (node.getOp() == "-") {
    auto right = eval(*node.getRight(), env);
    if (isError(right)) {
      bag = right;
      return;
    }
    bag = evalNegateOperator(right);
  }
};
void ASTEvaluator::dispatch(AST::InfixExpression &node) {
  spdlog::get(EVAL_LOGGER)
      ->info("Evaluating infix expression {}", node.getOp());
  auto left = eval(*node.getLeft(), env);
  if (isError(left)) {
    bag = left;
    return;
  }
  auto right = eval(*node.getRight(), env);
  if (isError(right)) {
    bag = right;
    return;
  }
  bag = evalInfixExpression(node.getOp(), left, right);
  spdlog::get(EVAL_LOGGER)
      ->info("Returning infix statement {}", bag->inspect());
};
void ASTEvaluator::dispatch(AST::IfExpression &node) {
  bag = evalIfExpression(node, env);
  spdlog::get(EVAL_LOGGER)
      ->info("Returning if of type {}", Eval::typeToString(bag->type()));
};
void ASTEvaluator::dispatch(AST::FunctionLiteral &node) {
  bag = makeFunctionBag(env, node.getArguments(), node.getBody());
}
void ASTEvaluator::dispatch(AST::CallExpression &node) {
  auto val = eval(*node.getFunction(), env);
  if (isError(val)) {
    bag = val;
    return;
  }
  if (val->type() != Eval::Type::FUNC_OBJ) {
    bag = makeNotAFunctionError(node.getFunction()->tokenLiteral());
    return;
  }
  auto func = Eval::convertToFunction(val);
  std::map<std::string, std::shared_ptr<Eval::Bag>> args;
  auto identIter = func->arguments().begin();
  for (const auto &arg : node.getArguments()) {
    auto evalArg = eval(*arg, env);
    if (isError(evalArg)) {
      bag = evalArg;
      return;
    }
    args[identIter->get()->getValue()] = evalArg;
    identIter++;
  }
  auto wrappedEnv = std::make_shared<Env::Environment>(env, args);
  bag = eval(*func->body(), wrappedEnv);
  if (bag->type() == Eval::Type::RETURN_OBJ) {
    bag = convertToReturn(bag)->value();
  }
}
void ASTEvaluator::dispatch(AST::ReturnStatement &node) {
  spdlog::get(EVAL_LOGGER)->info("Evaluating return statement");
  auto ret = eval(*node.getReturnValue(), env);
  if (isError(ret)) {
    bag = ret;
    return;
  }
  bag = makeReturnBag(ret);
};
void ASTEvaluator::dispatch(AST::ExpressionStatement &node) {
  spdlog::get(EVAL_LOGGER)
      ->info("Evaluating expression statement {}", node.tokenLiteral());
  bag = eval(*node.getExpression(), env);
};
void ASTEvaluator::dispatch(AST::LetStatement &node) {
  spdlog::get(EVAL_LOGGER)->info("Evaluating let statement");
  auto val = eval(*node.getValue(), env);
  if (isError(val)) {
    bag = val;
    return;
  }

  spdlog::get(EVAL_LOGGER)->info("Setting let statemen {}", env);
  env->set(node.getName()->getValue(), val);
  spdlog::get(EVAL_LOGGER)->info("Set let statement");

  bag = nullptr;
};
void ASTEvaluator::dispatch(AST::BlockStatement &node) {
  spdlog::get(EVAL_LOGGER)->info("Evaluating block expression");
  auto statements = node.getStatements();
  bag = evalBlockStatement(statements, env);
};

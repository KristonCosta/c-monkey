#include "builtin.hpp"
#include "bag.hpp"
#include "eval_errors.hpp"
#include "output.hpp"

std::shared_ptr<Eval::Bag> evalLenBuiltin(
    const std::string& name,
    const std::vector<std::shared_ptr<Eval::Bag>>& arguments) {
  if (arguments.size() != 1) {
    return makeBuiltinInvalidNumberOfArguments(name, 1, arguments.size());
  }
  auto arg = arguments.begin()->get();

  switch (arg->type()) {
    case (Eval::Type::STRING_OBJ): {
      return std::make_shared<Eval::IntegerBag>(
          ((Eval::StringBag*)arg)->value().size());
    }
    case (Eval::Type::ARRAY_OBJ): {
      return std::make_shared<Eval::IntegerBag>(
          ((Eval::ArrayBag*)arg)->values().size());
    }
    default:
      return makeBuiltinInvalidArgument(name, arg->type());
  }
}

std::shared_ptr<Eval::Bag> evalHeadBuiltin(
    const std::string& name,
    const std::vector<std::shared_ptr<Eval::Bag>>& arguments) {
  if (arguments.size() != 1) {
    return makeBuiltinInvalidNumberOfArguments(name, 1, arguments.size());
  }
  auto arg = arguments.begin()->get();
  if (arg->type() == Eval::Type::ARRAY_OBJ) {
    auto vec = ((Eval::ArrayBag*)arg)->values();
    if (vec.empty()) {
      return Eval::NULL_BAG;
    }
    return vec.at(0);
  }
  return makeBuiltinInvalidArgument(name, arg->type());
}

std::shared_ptr<Eval::Bag> evalTailBuiltin(
    const std::string& name,
    const std::vector<std::shared_ptr<Eval::Bag>>& arguments) {
  if (arguments.size() != 1) {
    return makeBuiltinInvalidNumberOfArguments(name, 1, arguments.size());
  }
  auto arg = arguments.begin()->get();
  if (arg->type() == Eval::Type::ARRAY_OBJ) {
    auto vec = ((Eval::ArrayBag*)arg)->values();
    if (vec.empty()) {
      return Eval::NULL_BAG;
    }
    std::vector<std::shared_ptr<Eval::Bag>> sub(vec.begin() + 1, vec.end());
    return std::make_shared<Eval::ArrayBag>(sub);
  }
  return makeBuiltinInvalidArgument(name, arg->type());
}

std::shared_ptr<Eval::Bag> evalPushBuiltin(
    const std::string& name,
    const std::vector<std::shared_ptr<Eval::Bag>>& arguments) {
  if (arguments.size() != 2) {
    return makeBuiltinInvalidNumberOfArguments(name, 2, arguments.size());
  }
  auto arg = arguments.at(0);
  auto elem = arguments.at(1);
  if (arg->type() == Eval::Type::ARRAY_OBJ) {
    convertToArray(arg)->add(elem);
    return arg; // std::make_shared<Eval::ArrayBag>(newVec);
  }
  return makeBuiltinInvalidArgument(name, arg->type());
}

std::shared_ptr<Eval::Bag> evalPrintBuiltin(
    const std::string& name,
    const std::vector<std::shared_ptr<Eval::Bag>>& arguments) {
  for (const auto& arg : arguments) {
    spdlog::get(EVAL_OUTPUT)->info("{}", arg.get()->inspect());
  }
  return Eval::NULL_BAG;
}

std::shared_ptr<Eval::Bag> evalSPrintBuiltin(
    const std::string& name,
    const std::vector<std::shared_ptr<Eval::Bag>>& arguments) {
  std::stringstream ss;
  for (const auto& arg : arguments) {
    ss << arg.get()->inspect();
  }
  return std::make_shared<Eval::StringBag>(ss.str());
}

std::map<std::string, std::shared_ptr<Eval::BuiltinBag>> Builtin::_builtins = {
    {"len", std::make_shared<Eval::BuiltinBag>("len", evalLenBuiltin)},
    {"head", std::make_shared<Eval::BuiltinBag>("head", evalHeadBuiltin)},
    {"tail", std::make_shared<Eval::BuiltinBag>("tail", evalTailBuiltin)},
    {"push", std::make_shared<Eval::BuiltinBag>("push", evalPushBuiltin)},
    {"print", std::make_shared<Eval::BuiltinBag>("print", evalPrintBuiltin)},
    {"sprint", std::make_shared<Eval::BuiltinBag>("sprint", evalSPrintBuiltin)},
};

std::shared_ptr<Eval::BuiltinBag> Builtin::get(const std::string& name) {
  if (Builtin::_builtins.find(name) != Builtin::_builtins.end()) {
    return Builtin::_builtins.at(name);
  }
  return nullptr;
}

bool Builtin::contains(const std::string& name) {
  return Builtin::_builtins.find(name) != Builtin::_builtins.end();
}

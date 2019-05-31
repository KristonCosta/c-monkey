#include "builtin.hpp"
#include "bag.hpp"
#include "eval_errors.hpp"

std::shared_ptr<Eval::Bag> evalLenBuiltin(
    const std::string& name,
    const std::list<std::shared_ptr<Eval::Bag>>& arguments) {
  if (arguments.size() != 1) {
    return makeBuiltinInvalidNumberOfArguments(name, 1, arguments.size());
  }
  auto arg = arguments.begin()->get();
  if (arg->type() != Eval::Type::STRING_OBJ) {
    return makeBuiltinInvalidArgument(name, arg->type());
  }
  return std::make_shared<Eval::IntegerBag>(
      ((Eval::StringBag*)arg)->value().size());
}

std::map<std::string, std::shared_ptr<Eval::BuiltinBag>> Builtin::_builtins = {
    {"len", std::make_shared<Eval::BuiltinBag>("len", evalLenBuiltin)}};

std::shared_ptr<Eval::BuiltinBag> Builtin::get(const std::string& name) {
  if (Builtin::_builtins.find(name) != Builtin::_builtins.end()) {
    return Builtin::_builtins.at(name);
  }
  return nullptr;
}

bool Builtin::contains(const std::string& name) {
  return Builtin::_builtins.find(name) != Builtin::_builtins.end();
}
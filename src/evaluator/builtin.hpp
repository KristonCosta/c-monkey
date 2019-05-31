#pragma once
#include <map>
#include "bag.hpp"
class Builtin {
 private:
  static std::map<std::string, std::shared_ptr<Eval::BuiltinBag>> _builtins;

 public:
  static std::shared_ptr<Eval::BuiltinBag> get(const std::string& name);
  static bool contains(const std::string& name);
};
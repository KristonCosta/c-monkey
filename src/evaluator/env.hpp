#pragma once
#include <iostream>
#include <map>
#include <string>
namespace Eval {
class Bag;
}
namespace Env {
class Environment {
 private:
  std::map<std::string, std::shared_ptr<Eval::Bag>> _table;
  std::shared_ptr<Environment> _env;

 public:
  Environment() : _env(nullptr){};
  Environment(std::shared_ptr<Environment> env) : _env(env){};
  Environment(std::shared_ptr<Environment> env,
              std::map<std::string, std::shared_ptr<Eval::Bag>> &refs)
      : _env(env) {
    _table.insert(refs.begin(), refs.end());
  };
  void set(std::string identifier, std::shared_ptr<Eval::Bag> bag);
  std::shared_ptr<Eval::Bag> get(std::string identifier);
};
};  // namespace Env
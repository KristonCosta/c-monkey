#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cstdlib>
#include <fmt/format.h>

namespace Eval {
class Bag;
}
namespace Env {
class Environment {
 private:
  std::map<std::string, std::shared_ptr<Eval::Bag>> _table;
  std::shared_ptr<Environment> _env;
  
  void clearInnerIfShadowed();

 public:
  Environment() : _env(nullptr){};
  explicit Environment(std::shared_ptr<Environment> env) : _env(std::move(env)){
      this->clearInnerIfShadowed();
  };
  Environment(std::shared_ptr<Environment> env,
              std::map<std::string, std::shared_ptr<Eval::Bag>> &refs)
      : _env(std::move(env)) {
    _table.insert(refs.begin(), refs.end());
    this->clearInnerIfShadowed();
  };
  void set(const std::string& identifier, std::shared_ptr<Eval::Bag> bag);
  std::shared_ptr<Eval::Bag> get(const std::string& identifier);
  std::vector<std::string> getAllNames() {
    std::vector<std::string> names;
    for(auto & it : _table) {
        names.push_back(it.first);
    }
    return names;
  }
};
}  // namespace Env
#include "env.hpp"

#include <utility>
using namespace Env;

void Environment::set(const std::string& identifier, std::shared_ptr<Eval::Bag> bag) {
  this->_table[identifier] = std::move(bag);
  this->clearInnerIfShadowed();
}

void Environment::clearInnerIfShadowed(){
  if (this->_env) {
    auto inner_names = _env->getAllNames();
    bool hasOtherNames = false;
    for (auto &name: inner_names) {
      if (this->_table.find(name) == this->_table.end()) {
        hasOtherNames = true;
        break;
      }
    }
    if (!hasOtherNames) {
      _env = nullptr;
    }
  }
}

std::shared_ptr<Eval::Bag> Environment::get(const std::string& identifier) {
  if (this->_table.find(identifier) != this->_table.end()) {
    return this->_table.at(identifier);
  }
  if (this->_env) {
    return _env->get(identifier);
  }
  return nullptr;
}

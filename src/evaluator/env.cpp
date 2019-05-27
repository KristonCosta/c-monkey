#include "env.hpp"
using namespace Env;

void Environment::set(std::string identifier, std::shared_ptr<Eval::Bag> bag) {
  this->_table[identifier] = bag;
}
std::shared_ptr<Eval::Bag> Environment::get(std::string identifier) {
  if (this->_table.find(identifier) != this->_table.end()) {
    return this->_table.at(identifier);
  }
  if (this->_env) {
    return _env->get(identifier);
  }
  return nullptr;
}

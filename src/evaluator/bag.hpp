#pragma once
#include <spdlog/spdlog.h>
#include <string>

namespace Eval {

enum class Type {
  BASE_OBJ,
  INTEGER_OBJ,
  BOOLEAN_OBJ,
  NULL_OBJ,
};

class Bag {
 public:
  virtual std::string inspect() const = 0;
  virtual Type type() const = 0;
};

class BaseBag : public Bag {
 private:
  std::string _value;

 public:
  BaseBag(std::string value) : _value(value){};
  virtual std::string inspect() const override {
    return fmt::format("{}", _value);
  };
  virtual Type type() const override { return Type::BASE_OBJ; };
};

class IntegerBag : public Bag {
 private:
  int64_t _value;

 public:
  IntegerBag(int64_t value) : _value(value){};
  virtual std::string inspect() const override {
    return fmt::format("{}", _value);
  };
  virtual Type type() const override { return Type::INTEGER_OBJ; };
  int64_t value() const { return this->_value; }
};

class BooleanBag : public Bag {
 private:
  bool _value;

 public:
  BooleanBag(bool value) : _value(value){};
  virtual std::string inspect() const override {
    return fmt::format("{}", _value);
  };
  virtual Type type() const override { return Type::BOOLEAN_OBJ; };
  int value() const { return this->_value; }
};

class NullBag : public Bag {
 public:
  NullBag();
  virtual std::string inspect() const override { return "null"; };
  virtual Type type() const override { return Type::NULL_OBJ; };
};
}  // namespace Eval
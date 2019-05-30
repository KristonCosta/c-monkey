#pragma once
#include <spdlog/spdlog.h>
#include <ast.hpp>
#include <env.hpp>
#include <print_dispatcher.hpp>
#include <sstream>
#include <string>

namespace Eval {
/*

  Bag types

*/
enum class Type {
  BASE_OBJ,
  // basic
  INTEGER_OBJ,
  STRING_OBJ,
  BOOLEAN_OBJ,
  NULL_OBJ,
  ERROR_OBJ,
  // complex
  FUNC_OBJ,
  RETURN_OBJ,
};

inline std::string typeToString(Type type) {
  switch (type) {
    case Type::BASE_OBJ:
      return "BASE";
    case Type::INTEGER_OBJ:
      return "INTEGER";
    case Type::STRING_OBJ:
      return "STRING";
    case Type::BOOLEAN_OBJ:
      return "BOOLEAN";
    case Type::NULL_OBJ:
      return "NULL";
    case Type::RETURN_OBJ:
      return "RETURN";
    case Type::ERROR_OBJ:
      return "ERROR";
    case Type::FUNC_OBJ:
      return "FUNCTION";
  }
}

/*

  Base bag

*/
class Bag {
 public:
  virtual std::string inspect() const = 0;
  virtual Type type() const = 0;
};

/*

  Basic bag classes

*/
class IntegerBag : public Bag {
 private:
  int64_t _value;

 public:
  IntegerBag(int64_t value) : _value(value){};
  virtual std::string inspect() const override {
    return fmt::format("{}", _value);
  };
  virtual Type type() const override { return Type::INTEGER_OBJ; };
  int64_t value() const { return _value; }
};

class StringBag : public Bag {
 private:
  std::string _value;

 public:
  StringBag(std::string value) : _value(value){};
  virtual std::string inspect() const override {
    return fmt::format("{}", _value);
  };
  virtual Type type() const override { return Type::STRING_OBJ; };
  std::string value() const { return _value; }
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
  bool value() const { return _value; }
};

class ErrorBag : public Bag {
 private:
  std::string _message;

 public:
  ErrorBag(std::string message) : _message(message){};
  virtual std::string inspect() const override {
    return fmt::format("error: {}", _message);
  };
  virtual Type type() const override { return Type::ERROR_OBJ; };
  std::string message() const { return _message; }
};

class NullBag : public Bag {
 public:
  NullBag(){};
  virtual std::string inspect() const override { return "null"; };
  virtual Type type() const override { return Type::NULL_OBJ; };
};

/*

  Complex bag classes

*/
class ReturnBag : public Bag {
 private:
  std::shared_ptr<Bag> _value;

 public:
  ReturnBag(std::shared_ptr<Bag> value) : _value(value){};
  virtual std::string inspect() const override {
    return fmt::format("{}", _value->inspect());
  };
  virtual Type type() const override { return Type::RETURN_OBJ; };
  std::shared_ptr<Bag> value() const { return _value; }
};

class FunctionBag : public Bag {
 private:
  std::shared_ptr<Env::Environment> _env;
  std::list<std::shared_ptr<AST::Identifier>> _arguments;
  std::shared_ptr<AST::BlockStatement> _body;

 public:
  FunctionBag(std::shared_ptr<Env::Environment> env,
              std::list<std::shared_ptr<AST::Identifier>> arguments,
              std::shared_ptr<AST::BlockStatement> body)
      : _env(env), _arguments(arguments), _body(body){};
  virtual std::string inspect() const override {
    std::stringstream ss;
    ss << "fn(";
    for (auto arg = _arguments.begin(); arg != _arguments.end(); ++arg) {
      if (arg == _arguments.begin()) {
        ASTPrinter::write([&](std::string message) { ss << message; },
                          *arg->get());
      } else {
        ss << ", ";
        ASTPrinter::write([&](std::string message) { ss << message; },
                          *arg->get());
      }
    }
    ss << ") {\n";
    ASTPrinter::write([&](std::string message) { ss << message; }, *_body);
    ss << "\n}";
    return ss.str();
  };
  virtual Type type() const override { return Type::FUNC_OBJ; };
  std::list<std::shared_ptr<AST::Identifier>>& arguments() {
    return _arguments;
  }
  std::shared_ptr<AST::BlockStatement> body() { return _body; }
  std::shared_ptr<Env::Environment> env() { return _env; }
};

/*

  Bag conversion helpers

*/
template <class T>
std::shared_ptr<T> convertType(std::shared_ptr<Bag> bag, Type type) {
  if (bag->type() == type) {
    return std::static_pointer_cast<T>(bag);
  }
  return nullptr;
}

inline std::shared_ptr<BooleanBag> convertToBoolean(std::shared_ptr<Bag> bag) {
  return convertType<BooleanBag>(bag, Type::BOOLEAN_OBJ);
}

inline std::shared_ptr<IntegerBag> convertToInteger(std::shared_ptr<Bag> bag) {
  return convertType<IntegerBag>(bag, Type::INTEGER_OBJ);
}

inline std::shared_ptr<NullBag> convertToNull(std::shared_ptr<Bag> bag) {
  return convertType<NullBag>(bag, Type::NULL_OBJ);
}

inline std::shared_ptr<ReturnBag> convertToReturn(std::shared_ptr<Bag> bag) {
  return convertType<ReturnBag>(bag, Type::RETURN_OBJ);
}

inline std::shared_ptr<ErrorBag> convertToError(std::shared_ptr<Bag> bag) {
  return convertType<ErrorBag>(bag, Type::ERROR_OBJ);
}

inline std::shared_ptr<FunctionBag> convertToFunction(
    std::shared_ptr<Bag> bag) {
  return convertType<FunctionBag>(bag, Type::FUNC_OBJ);
}
}  // namespace Eval
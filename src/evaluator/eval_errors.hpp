#pragma once
#include <spdlog/spdlog.h>
#include <bag.hpp>

inline std::shared_ptr<Eval::ErrorBag> makeErrorWithMessage(
    std::string message) {
  return std::make_shared<Eval::ErrorBag>(message);
}

inline std::shared_ptr<Eval::ErrorBag> makeInfixTypeMismatchError(
    Eval::Type leftType, Eval::Type rightType, std::string op) {
  return makeErrorWithMessage(fmt::format("type mismatch: {} {} {}",
                                          Eval::typeToString(leftType), op,
                                          Eval::typeToString(rightType)));
}

inline std::shared_ptr<Eval::ErrorBag> makeInfixUnknownOperatorError(
    Eval::Type leftType, Eval::Type rightType, std::string op) {
  return makeErrorWithMessage(fmt::format("unknown operator: {} {} {}",
                                          Eval::typeToString(leftType), op,
                                          Eval::typeToString(rightType)));
}

inline std::shared_ptr<Eval::ErrorBag> makePrefixOperatorError(
    Eval::Type leftType, std::string op) {
  return makeErrorWithMessage(
      fmt::format("unknown operator: {}{}", op, Eval::typeToString(leftType)));
}

inline std::shared_ptr<Eval::ErrorBag> makeIdentifierNotFoundError(
    std::string identifier) {
  return makeErrorWithMessage(
      fmt::format("identifier not found: {}", identifier));
}

inline std::shared_ptr<Eval::ErrorBag> makeNotAFunctionError(
    std::string identifier) {
  return makeErrorWithMessage(fmt::format("not a function: {}", identifier));
}

inline std::shared_ptr<Eval::ErrorBag> makeBuiltinWithNameExists(
    std::string identifier) {
  return makeErrorWithMessage(
      fmt::format("builtin name collision: {}", identifier));
}

inline std::shared_ptr<Eval::ErrorBag> makeBuiltinInvalidArgument(
    std::string identifier, Eval::Type type) {
  return makeErrorWithMessage(
      fmt::format("argument to `{}` not supported, got {}", identifier,
                  Eval::typeToString(type)));
}

inline std::shared_ptr<Eval::ErrorBag> makeBuiltinInvalidNumberOfArguments(
    std::string identifier, int expected, int actual) {
  return makeErrorWithMessage(
      fmt::format("wrong number of arguments to `{}`: expected {}, found {}",
                  identifier, expected, actual));
}

inline std::shared_ptr<Eval::ErrorBag> makeInvalidIndexException(
    Eval::Type leftType, Eval::Type indexType) {
  return makeErrorWithMessage(fmt::format(
      "index operator not supported: {} doesn't support index type {}",
      Eval::typeToString(leftType), Eval::typeToString(indexType)));
}

inline std::shared_ptr<Eval::ErrorBag> makeInvalidHashKeyType(
    Eval::Type keyType) {
  return makeErrorWithMessage(fmt::format("hash key type is not supported: {}",
                                          Eval::typeToString(keyType)));
}

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
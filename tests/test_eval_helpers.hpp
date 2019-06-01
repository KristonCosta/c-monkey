#pragma once
#include <bag.hpp>
#include <catch2/catch.hpp>

template <class T>
struct Pair {
  std::string input;
  T expected;
};

inline Eval::IntegerBag *testIntegerBag(Eval::Bag *bag, int64_t value) {
  REQUIRE(bag);
  REQUIRE(bag->type() == Eval::Type::INTEGER_OBJ);
  const auto ret = dynamic_cast<Eval::IntegerBag *>(bag);
  REQUIRE(ret);
  REQUIRE(ret->value() == value);
  return ret;
}

inline Eval::IntegerBag *testIntegerBag(std::shared_ptr<Eval::Bag> bag,
                                        int length) {
  REQUIRE(bag);
  return testIntegerBag(bag.get(), length);
}

inline Eval::StringBag *testStringBag(std::shared_ptr<Eval::Bag> bag,
                                      const std::string &value) {
  REQUIRE(bag);
  REQUIRE(bag->type() == Eval::Type::STRING_OBJ);
  auto ret = std::static_pointer_cast<Eval::StringBag>(bag);
  REQUIRE(ret->value() == value);
  return ret.get();
}

inline Eval::BooleanBag *testBooleanBag(std::shared_ptr<Eval::Bag> bag,
                                        bool value) {
  REQUIRE(bag);
  REQUIRE(bag->type() == Eval::Type::BOOLEAN_OBJ);
  auto ret = std::static_pointer_cast<Eval::BooleanBag>(bag);
  REQUIRE(ret->value() == value);
  return ret.get();
}

inline Eval::ArrayBag *testArrayBag(Eval::Bag *bag, int length) {
  REQUIRE(bag);
  REQUIRE(bag->type() == Eval::Type::ARRAY_OBJ);
  const auto ret = dynamic_cast<Eval::ArrayBag *>(bag);
  REQUIRE(ret);
  REQUIRE(ret->values().size() == length);
  return ret;
}

inline Eval::ArrayBag *testArrayBag(std::shared_ptr<Eval::Bag> bag,
                                    int length) {
  REQUIRE(bag);
  return testArrayBag(bag.get(), length);
}

inline void testNullBag(std::shared_ptr<Eval::Bag> bag) {
  REQUIRE(bag);
  REQUIRE(bag->type() == Eval::Type::NULL_OBJ);
}

inline void testErrorBag(std::shared_ptr<Eval::Bag> bag,
                         const std::string &message) {
  REQUIRE(bag);
  REQUIRE(bag->type() == Eval::Type::ERROR_OBJ);
  auto ret = std::static_pointer_cast<Eval::ErrorBag>(bag);
  REQUIRE(ret->message() == message);
}
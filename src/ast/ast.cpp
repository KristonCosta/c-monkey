#include "ast.hpp"

using namespace AST;

std::string Statement::toDebugString() const {
  return fmt::format("[statement literal={}]", this->tokenLiteral());
};

std::string Expression::toDebugString() const {
  return fmt::format("[expression literal={}]", this->tokenLiteral());
};

/*

  High-level node types

*/

const std::list<std::shared_ptr<Statement>> &Program::getStatements() const {
  return this->statements;
}

const uint64_t Program::size() { return this->statements.size(); };

void Program::addStatement(std::shared_ptr<Statement> statement) {
  this->statements.push_back(statement);
};

std::string Program::tokenLiteral() const {
  if (this->statements.size() > 0) {
    return this->statements.front()->tokenLiteral();
  }
  return "";
};

std::string Program::toDebugString() const {
  std::stringstream ss;
  ss << "[program" << std::endl;
  for (const auto &statement : statements) {
    ss << statement->toDebugString() << std::endl;
  }
  ss << "]";
  return ss.str();
};

/*

  Expression Types

*/

const std::string Identifier::getValue() { return this->value; };

std::string Identifier::toDebugString() const {
  std::stringstream ss;
  ss << "[identifier";
  if (this->token) {
    ss << " token=" << *this->token;
  }
  ss << " value=" << this->value << "]";
  return ss.str();
};

const int64_t IntegerLiteral::getValue() { return this->value; };

std::string IntegerLiteral::toDebugString() const {
  std::stringstream ss;
  ss << "[integer";
  if (this->token) {
    ss << " token=" << *this->token;
  }
  ss << " value=" << this->value << "]";
  return ss.str();
};

const bool Boolean::getValue() { return this->value; };

std::string Boolean::toDebugString() const {
  std::stringstream ss;
  ss << "[bool value=" << this->value << "]";
  return ss.str();
};

/*

  Expression Types - operators

*/

const std::shared_ptr<Expression> PrefixExpression::getRight() {
  return this->right;
};

const Operator PrefixExpression::getOp() { return this->op; };

std::string PrefixExpression::toDebugString() const {
  std::stringstream ss;
  ss << "[prefix";
  if (this->token) {
    ss << " token=" << *this->token;
  }
  if (this->right) {
    ss << " right=" << this->right->toDebugString();
  }
  ss << " op=" << this->op << "]";
  return ss.str();
};

const std::shared_ptr<Expression> InfixExpression::getLeft() {
  return this->left;
};

const std::shared_ptr<Expression> InfixExpression::getRight() {
  return this->right;
};

const Operator InfixExpression::getOp() { return this->op; };

std::string InfixExpression::toDebugString() const {
  std::stringstream ss;
  ss << "[infix"
     << " token=" << *this->token << " left=" << this->left->toDebugString();
  if (this->right) {
    ss << " right=" << this->right->toDebugString();
  }
  ss << " op=" << this->op << "]";
  return ss.str();
};

/*

  Expression Types - complex

*/

std::shared_ptr<Expression> &IfExpression::getCondition() {
  return this->condition;
};
std::shared_ptr<BlockStatement> &IfExpression::getWhenTrue() {
  return this->whenTrue;
};
std::shared_ptr<BlockStatement> &IfExpression::getWhenFalse() {
  return this->whenFalse;
};

std::string IfExpression::toDebugString() const {
  std::stringstream ss;
  ss << "[if token=" << *this->token
     << " condition=" << this->condition->toDebugString()
     << " whenTrue=" << this->whenTrue->toDebugString();
  if (this->whenFalse) {
    ss << " whenFalse=" << this->whenFalse->toDebugString();
  }
  ss << "]";
  return ss.str();
};

const std::list<std::shared_ptr<Identifier>> &FunctionLiteral::getArguments()
    const {
  return this->arguments;
};

const uint64_t FunctionLiteral::size() { return this->arguments.size(); };

void FunctionLiteral::addArgument(std::shared_ptr<Identifier> identifier) {
  this->arguments.push_back(identifier);
};

std::shared_ptr<BlockStatement> &FunctionLiteral::getBody() {
  return this->body;
};

std::string FunctionLiteral::toDebugString() const {
  std::stringstream ss;
  ss << "[function token=" << *this->token << " arguments=[";
  for (const auto &ident : arguments) {
    ss << ident->toDebugString() << ", ";
  }
  ss << "] body=" << this->body->toDebugString();
  return ss.str();
};

const std::list<std::shared_ptr<Expression>> &CallExpression::getArguments()
    const {
  return this->arguments;
};

const uint64_t CallExpression::size() { return this->arguments.size(); };

void CallExpression::addArgument(std::shared_ptr<Expression> expr) {
  this->arguments.push_back(expr);
};

std::shared_ptr<Expression> CallExpression::getFunction() {
  return this->func;
};

std::string CallExpression::toDebugString() const {
  std::stringstream ss;
  ss << "[call token=" << *this->token
     << " func=" << this->func->toDebugString() << " arguments=[";
  for (const auto &arg : arguments) {
    ss << arg->toDebugString() << ", ";
  }
  ss << "]";
  return ss.str();
};

/*

  Statement Types

*/

std::shared_ptr<Expression> ReturnStatement::getReturnValue() const {
  return this->returnValue;
}

std::string ReturnStatement::toDebugString() const {
  std::stringstream ss;
  ss << "[return token=" << *this->token;
  if (this->returnValue) {
    ss << " returnValue=" << this->returnValue->toDebugString();
  }
  ss << "]";
  return ss.str();
};

std::shared_ptr<Expression> ExpressionStatement::getExpression() const {
  return this->expression;
};

std::string ExpressionStatement::toDebugString() const {
  std::stringstream ss;
  ss << "[expression token=" << *this->token;
  if (this->expression) {
    ss << " expression=" << this->expression->toDebugString();
  }
  ss << "]";
  return ss.str();
};

const std::shared_ptr<Identifier> LetStatement::getName() {
  return this->name;
};

const std::shared_ptr<Expression> LetStatement::getValue() {
  return this->value;
};

std::string LetStatement::toDebugString() const {
  std::stringstream ss;
  ss << "[letstatement";
  if (this->token) {
    ss << " token=" << *this->token;
  }
  if (this->name) {
    ss << " name=" << this->name->toDebugString();
  }
  if (this->value) {
    ss << " value=" << this->value->toDebugString();
  }
  ss << "]";
  return ss.str();
};

const std::list<std::shared_ptr<Statement>> &BlockStatement::getStatements()
    const {
  return this->statements;
};

const uint64_t BlockStatement::size() { return this->statements.size(); };

void BlockStatement::addStatement(std::shared_ptr<Statement> statement) {
  this->statements.push_back(statement);
};

std::string BlockStatement::toDebugString() const {
  std::stringstream ss;
  ss << "[block statements=[";
  for (const auto &statement : statements) {
    ss << statement->toDebugString() << ", ";
  };
  ss << "]";
  return ss.str();
};

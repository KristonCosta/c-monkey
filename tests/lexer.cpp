#include <catch2/catch.hpp>
#include <lexer.hpp>

struct Pair {
  TokenType type;
  std::string literal;
};
TEST_CASE("Basic token parsing", "[lexer]") {
  std::string input =
      "let five = 5; \
  let ten = 10; \
  \
  let add = fn(x, y) {\
    x + y;\
  };\
  let result = add(five, ten);";

  Pair testPairs[] = {
      Pair{TokenType::LET, "let"},
      Pair{TokenType::IDENT, "five"},
      Pair{TokenType::ASSIGN, "="},
      Pair{TokenType::INTEGER, "5"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::LET, "let"},
      Pair{TokenType::IDENT, "ten"},
      Pair{TokenType::ASSIGN, "="},
      Pair{TokenType::INTEGER, "10"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::LET, "let"},
      Pair{TokenType::IDENT, "add"},
      Pair{TokenType::ASSIGN, "="},
      Pair{TokenType::FUNCTION, "fn"},
      Pair{TokenType::LPAREN, "("},
      Pair{TokenType::IDENT, "x"},
      Pair{TokenType::COMMA, ","},
      Pair{TokenType::IDENT, "y"},
      Pair{TokenType::RPAREN, ")"},
      Pair{TokenType::LBRACE, "{"},
      Pair{TokenType::IDENT, "x"},
      Pair{TokenType::PLUS, "+"},
      Pair{TokenType::IDENT, "y"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::RBRACE, "}"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::LET, "let"},
      Pair{TokenType::IDENT, "result"},
      Pair{TokenType::ASSIGN, "="},
      Pair{TokenType::IDENT, "add"},
      Pair{TokenType::LPAREN, "("},
      Pair{TokenType::IDENT, "five"},
      Pair{TokenType::COMMA, ","},
      Pair{TokenType::IDENT, "ten"},
      Pair{TokenType::RPAREN, ")"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::END_OF_FILE, std::string(1, '\0')},
  };
  auto lexer = Lexer::from(input);
  for (const Pair &pair : testPairs) {
    auto tok = lexer->nextToken();
    REQUIRE(tok->type == pair.type);
    REQUIRE(tok->literal == pair.literal);
  }
};
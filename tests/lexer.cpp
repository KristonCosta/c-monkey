#include <catch2/catch.hpp>
#include <lexer.hpp>

struct Pair {
  TokenType type;
  std::string literal;
};
TEST_CASE("Basic token parsing", "[lexer]") {
  std::string input = R"V0G0N(let five = 5; 
let ten = 10;

let add = fn(x, y) { x + y; };
let result = add(five, ten);
let x = "test 123 five!";
[1, 2];
{"test":"map"};
while(true);
)V0G0N";

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
      Pair{TokenType::LET, "let"},
      Pair{TokenType::IDENT, "x"},
      Pair{TokenType::ASSIGN, "="},
      Pair{TokenType::STRING, "test 123 five!"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::LBRACKET, "["},
      Pair{TokenType::INTEGER, "1"},
      Pair{TokenType::COMMA, ","},
      Pair{TokenType::INTEGER, "2"},
      Pair{TokenType::RBRACKET, "]"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::LBRACE, "{"},
      Pair{TokenType::STRING, "test"},
      Pair{TokenType::COLON, ":"},
      Pair{TokenType::STRING, "map"},
      Pair{TokenType::RBRACE, "}"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::WHILE, "while"},
      Pair{TokenType::LPAREN, "("},
      Pair{TokenType::TRUE, "true"},
      Pair{TokenType::RPAREN, ")"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::END_OF_FILE, std::string(1, '\0')},
  };
  auto lexer = Lexer(input);
  for (const Pair &pair : testPairs) {
    auto tok = lexer.nextToken();
    REQUIRE(tokenTypeToString(tok->type) == tokenTypeToString(pair.type));
    REQUIRE(tok->literal == pair.literal);
  }
};

TEST_CASE("Advanced token parsing", "[lexer]") {
  std::string input =
      R"V0G0N(!-/*5;
5 < 10 > 5;
  
if (5 < 10) {
  return true; 
} else { 
  return false;
})V0G0N";

  Pair testPairs[] = {
      Pair{TokenType::BANG, "!"},
      Pair{TokenType::MINUS, "-"},
      Pair{TokenType::SLASH, "/"},
      Pair{TokenType::ASTERISK, "*"},
      Pair{TokenType::INTEGER, "5"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::INTEGER, "5"},
      Pair{TokenType::LT, "<"},
      Pair{TokenType::INTEGER, "10"},
      Pair{TokenType::GT, ">"},
      Pair{TokenType::INTEGER, "5"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::IF, "if"},
      Pair{TokenType::LPAREN, "("},
      Pair{TokenType::INTEGER, "5"},
      Pair{TokenType::LT, "<"},
      Pair{TokenType::INTEGER, "10"},
      Pair{TokenType::RPAREN, ")"},
      Pair{TokenType::LBRACE, "{"},
      Pair{TokenType::RETURN, "return"},
      Pair{TokenType::TRUE, "true"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::RBRACE, "}"},
      Pair{TokenType::ELSE, "else"},
      Pair{TokenType::LBRACE, "{"},
      Pair{TokenType::RETURN, "return"},
      Pair{TokenType::FALSE, "false"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::RBRACE, "}"},
      Pair{TokenType::END_OF_FILE, std::string(1, '\0')},
  };
  auto lexer = Lexer(input);
  for (const Pair &pair : testPairs) {
    auto tok = lexer.nextToken();
    REQUIRE(tokenTypeToString(tok->type) == tokenTypeToString(pair.type));
    REQUIRE(tok->literal == pair.literal);
  }
}
TEST_CASE("Multichar token parsing", "[lexer]") {
  std::string input =
      R"V0G0N(
let seven = 7;
10 == seven;
10 != 9;)V0G0N";

  Pair testPairs[] = {
      Pair{TokenType::LET, "let"},
      Pair{TokenType::IDENT, "seven"},
      Pair{TokenType::ASSIGN, "="},
      Pair{TokenType::INTEGER, "7"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::INTEGER, "10"},
      Pair{TokenType::EQ, "=="},
      Pair{TokenType::IDENT, "seven"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::INTEGER, "10"},
      Pair{TokenType::NE, "!="},
      Pair{TokenType::INTEGER, "9"},
      Pair{TokenType::SEMICOLON, ";"},
      Pair{TokenType::END_OF_FILE, std::string(1, '\0')},
  };
  auto lexer = Lexer(input);
  for (const Pair &pair : testPairs) {
    auto tok = lexer.nextToken();
    REQUIRE(tokenTypeToString(tok->type) == tokenTypeToString(pair.type));
    REQUIRE(tok->literal == pair.literal);
  }
};

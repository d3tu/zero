#include <iostream>

#include "lexer.hh"
#include "parser.hh"

int main() {
  try {
    Core::Parser::parse(Core::Lexer::lex(""));
  } catch (Core::Util::Exception &error) {
    std::cout << error.what() << std::endl;
  }

  return 0;
}
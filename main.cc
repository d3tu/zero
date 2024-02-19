#include <iostream>

#include "lexer.hh"
#include "parser.hh"

int main() {
  try {
    DJ::Parser::parse(DJ::Lexer::tokenize("jump label"));

    return 0;
  } catch (const char *error) {
    std::cout << error << std::endl;
  } catch (DJ::Util::Exception &error) {
    std::cout << error.what() << std::endl;
  }

  return 1;
}
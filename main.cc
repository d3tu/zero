#include <iostream>

#include "lexer.hh"
#include "parser.hh"

int main() {
  try {
    Core::Parser::parse(Core::Lexer::tokenize("jump label"));

    return 0;
  } catch (const char *error) {
    std::cout << error << std::endl;
  } catch (Core::Util::Exception &error) {
    std::cout << error.what() << std::endl;
  }

  return 1;
}
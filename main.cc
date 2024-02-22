#include <iostream>

#include "lexer.hh"
#include "parser.hh"
#include "generator.hh"
#include "runtime.hh"

int main() {
  try {
    Core::Runtime::exec(
      Core::Generator::gen(
        Core::Parser::parse(
          Core::Lexer::lex(
            "label:continue label"
          )
        )
      )
    );
  } catch (Core::Util::Exception &error) {
    std::cout << error.what() << std::endl;
  }

  return 0;
}
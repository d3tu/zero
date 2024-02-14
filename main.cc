#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "util.hh"
#include "lexer.hh"
#include "parser.hh"

int main() {
  using namespace DJ::Util;
  using namespace DJ::Lexer;
  using namespace DJ::Parser;

  using Node = DJ::Parser::Node;

  try {
    std::ifstream file("./main.dj");
    
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    std::string str = ss.str();

    auto tokens = tokenize(str.c_str());
    auto nodes = parse(tokens);

    if ((Function *) *nodes.begin()) {
      return 0;
    } else {
      return 1;
    }
  } catch (Exception &err) {
    std::cout << err.what() << std::endl;
  }

  return 0;
}
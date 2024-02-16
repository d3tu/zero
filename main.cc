#define DJ_VERSION "djLang version: beat (0.0.0) [alpha]"
#include <iostream>
#include "parser.hh"
#include "runtime.hh"
bool cmp(const char *str0, const char *str1) {
  auto p0 = str0, p1 = str1;
  while (*p0 && (*p0 == *p1)) { ++p0; ++p1; }
  return *p0 == *p1;
}
int len(const char *str) {
  auto p = str; int l = 0;
  while (*p) { ++l; ++p; }
  return l;
}
const char *tab(int size) {
  auto r = new char[size]; auto p = r;
  while (p - r < size) *p++ = ' ';
  *p = '\0'; return r;
}
int main(int argc, char **argv) {
  using namespace std;
  using namespace DJ::Util;
  using namespace DJ::Lexer;
  using namespace DJ::Parser;
  using namespace DJ::Bytecode;
  using namespace DJ::Runtime;
  using namespace DJ::Embed;
  try {
    if (isEmbed(argv[0])) return exec(decodeBinary(argv[0]));
    else if (argc < 2 || cmp("--help", argv[1]) || cmp("-h", argv[1])) {
      auto air = tab(len(argv[0]));
      cout << argv[0] << " run ./main.dj" << " (run code directly)" << endl;
      cout << air << " embed ./main.dj ./main.exe" << " (embed code in executable)" << endl;
      cout << air << " --help (or -h)" << " (show command's usage)" << endl;
      cout << air << " --version (or -v)" << " (show dj version)" << endl;
    } else if (cmp("--version", argv[1]) || cmp("-v", argv[1])) {
      cout << DJ_VERSION << endl;
    } else if (cmp("run", argv[1])) {
      if (argc > 2) {
        auto source = readBinary(argv[2]);
        auto tokens = tokenize(source);
        auto nodes = parse(tokens);
        auto byteCode = compile(nodes);
        return exec(byteCode);
      } else throw Exception("usage: dj run ./main.dj");
    } else if (cmp("embed", argv[1])) {
      if (argc > 2) {
        #ifdef _WIN32
          static auto out = "main.exe";
        #else
          static auto out = "main";
        #endif
        auto source = readBinary(argv[2]);
        auto tokens = tokenize(source);
        auto nodes = parse(tokens);
        auto byteCode = compile(nodes);
        encodeBinary(argv[0], out, byteCode, byteCode.size);
        #ifdef __unix__
          #include <sys/stat.h>
          chmod(out, 0777);
        #endif
      } else throw Exception("usage: dj embed ./main.dj");
    }
  } catch (Exception &err) {
    cerr << err.what() << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
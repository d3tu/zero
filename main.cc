#define DJ_VERSION "djLang version: beat (0.0.0) [alpha]"

#include <iostream>

#include "embed.hh"

bool strcmp(const char *str0, const char *str1) {
  auto p0 = str0, p1 = str1;

  while (*p0 && (*p0 == *p1)) {
    ++p0;
    ++p1;
  }

  return *p0 == *p1;
}

int strlen(const char *str) {
  auto p = str;
  int l = 0;

  while (*p) {
    ++l;
    ++p;
  }

  return l;
}

const char *strair(int length) {
  auto r = new char[length];
  auto p = r;

  while (p - r < length) {
    *p++ = ' ';
  }

  *p = '\0';

  return r;
}

const char *strcpy(const char *str, int length) {
  auto value = new char[length];
  auto p = value;
  while (p - value < length) {
    *p = str[p - value];
    ++p;
  }
  *p = '\0';
  return value;
}

int main(int argc, char **argv) {
  if (DJ::Embed::isEmbed(argv[0])) {
    auto data = DJ::Embed::decodeBinary(argv[0]);
    std::cout << strcpy(data.data, data.size) << std::endl;
    return 0;
  }

  if (argc < 2 || strcmp("--help", argv[1]) || strcmp("-h", argv[1])) {
    auto air = strair(strlen(argv[0]));
    std::cout << argv[0] << " run ./main.dj" << std::endl;
    std::cout << air << " embed ./main.dj ./main.exe" << std::endl;
    std::cout << air << " --help (or -h)" << std::endl;
    std::cout << air << " --version (or -v)" << std::endl;
  }

  if (strcmp("embed", argv[1])) {
    if (argc > 2) {
      #ifdef _WIN32
        static auto out = "main.exe";
      #else
        static auto out = "main";
      #endif

      DJ::Embed::encodeBinary(argv[0], out, "embedded", 9);
      
      #ifdef __unix__
        #include <sys/stat.h>
        chmod(out, 0777);
      #endif
    }
  }

  if (strcmp("--version", argv[1]) || strcmp("-v", argv[1])) {
    std::cout << DJ_VERSION << std::endl;
  }

  return 0;
}
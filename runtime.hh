#pragma once

#include <iostream>
#include <iomanip>

#define DEBUG_INIT "[DEBUG/VM]"
#define DEBUG(msg) std::cout << "\033[34m" << DEBUG_INIT << "\033[0m " << msg << std::endl

#define HEX(x) std::setw(2) << std::setfill('0') << std::hex << (int) x

namespace Core {
  namespace Runtime {
    enum Instr {
      HLT, JMP
    };

    int readInt(const char *bytes) {
      return (*bytes << 24) | (*(bytes + 1) << 16) | (*(bytes + 2) << 8) | (*(bytes + 3));
    }

    void exec(const char *bytes) {
      auto p = bytes;
      
      std::cout << std::endl;

      p = bytes;

      each: switch (*p) {
        case HLT: {
          auto init = p - bytes;
          DEBUG(init << ": HLT");
          return;
        }

        case JMP: {
          auto init = p - bytes;

          auto to = readInt(++p);

          p = bytes + to;

          // DEBUG(init << ": JMP " << to);

          goto each;
        }
      }
    }
  }
}
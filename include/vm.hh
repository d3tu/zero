#pragma once

namespace DJ {
  namespace VM {
    enum Instr { NOP, HLT, JMP };

    int readInt(const char *src) {
      return (*src << 24) | (*(src + 1) << 16) | (*(src + 2) << 8) | *(src + 3);
    }

    void writeInt(char *src, int value) {
      *src = (value >> 24) & 0xff;
      *(src + 1) = (value >> 16) & 0xff;
      *(src + 2) = (value >> 8) & 0xff;
      *(src + 3) = value & 0xff;
    }

    void exec(const char *src) {
      auto p = src;
      
      loop: switch (*p) {
        case NOP:
          ++p;
          goto loop;

        case HLT:
          return;

        case JMP:
          p = src + readInt(++p);
          goto loop;
      }
    }
  }
}
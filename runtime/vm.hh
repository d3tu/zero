#pragma once

#include "stack.hh"

namespace DJ {
  namespace VM {
    enum Instr { HLT, JMP, RET, CALL };


    // cmp value, value
    // cmp value, ptr
    // cmp ptr, value
    // cmp ptr, ptr
    // jmp label
    // hlt
    
    Stack<int> stack;

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
        case HLT:
          return;

        case JMP:
          p = src + readInt(++p);
          goto loop;

        case RET:
          p = src + stack.pop();
          goto loop;

        case CALL:
          p = src + readInt(++p);
          stack.push(readInt(p += 4));
          goto loop;
      }
    }
  }
}
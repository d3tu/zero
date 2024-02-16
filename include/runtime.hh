#pragma once
#include <iostream>
#include "bytecode.hh"
namespace DJ {
  namespace Runtime {
    short readShort(const char *bytes) { return (*bytes << 8) | *(bytes + 1); }
    int readInt(const char *bytes) { return (*bytes << 24) | (*(bytes + 1) << 16) | (*(bytes + 2) << 8) | (*(bytes + 3)); }
    int exec(const char *bytes) {
      using namespace Bytecode;
      auto p = bytes;
      Util::LinkedList<int> stack;
      while (*p) {
        switch (*p) {
          case OpCode::JMP: {
            p = bytes + *++p;
            continue;
          }
          case OpCode::CALL: {
            p = bytes + *++p;
            stack.push_back(*++p);
            continue;
          }
          case OpCode::RET: {
            stack.pop_back();
            p = bytes + stack.last();
            break;
          }
          case OpCode::HLT: return 0;
          default: throw Util::Exception("UnknownInstruction");
        }
        ++p;
      }
      return 0;
    }
  }
}
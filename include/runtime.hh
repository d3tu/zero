#pragma once

#include <iostream>

#include "util.hh"
#include "embed.hh"
#include "bytecode.hh"

namespace DJ {
  namespace Runtime {
    int exec(Embed::Data bytecode) {
      using namespace Bytecode;

      auto i = bytecode.data;

      auto setPos = [&](int pos) {
        i = bytecode.data + pos;
      };

      Util::LinkedList<void *> stack;

      std::cout << "bin: ";
      while (i - bytecode.data < bytecode.size) std::cout << std::hex << (int) *i++ << " ";
      std::cout << std::endl << std::endl;

      i = bytecode.data;

      while (i - bytecode.data < bytecode.size) {
        std::cout << "op: " << (int) *i << ", i: " << i - bytecode.data << std::endl;

        switch (*i) {
          case OpCode::CALL: {
            auto org = i++;
            auto to = *i;

            stack.push_back(org);
            setPos(to);

            continue;
          }

          case OpCode::RET:
            break;

          default:
            throw Util::Exception("UnknownInstruction");
        }

        ++i;
      }

      return 0;
    }
  }
}
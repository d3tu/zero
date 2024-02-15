#pragma once

#include <iostream>

#include "util.hh"
#include "parser.hh"
#include "embed.hh"

int fib(int n) { return n <= 1 ? n : fib(n - 1) + fib(n - 2); }

#define DEBUG(msg) std::cout << "DEBUG: " << (msg) << std::endl;

namespace DJ {
  namespace Bytecode {
    class str {
      const char *value;

      public:
        str(const char *value): value(value) {}

        operator const char *() const {
          return value;
        }

        operator const char *() {
          return value;
        }

        bool operator==(str &other) {
          const char *p0 = value, *p1 = other.value;

          while (*p0 && (*p0 == *p1)) {
            ++p0;
            ++p1;
          }

          return *p0 == *p1;
        }
    };

    template <typename K, typename V> struct KV { K key; V value; };
    template <typename K, typename V> class LinkedMap {

      Util::LinkedList<KV<K, V>> list;

      public:
        void set(K key, V value) {
          list.push_back({ key, value });
        }

        V *get(K key) {
          for (KV<K, V> &kv : list) {
            if (kv.key == key) {
              return &kv.value;
            }
          }

          return nullptr;
        }
    };

    enum OpCode {
      HLT,
      RET,
      JMP,
      CALL,
      CMP,
      JLE,
      PUSH,
      POP,
      ADD,
      SUB,
      MUL,
      DIV,
      MOV
    };

    // 0  jmp %end ; 15=36 0-1
    // 1  fib:
    // 1   cmp %1, 1 ; 2-4
    // 2   jle %end ; 15=36 5-6

    // 3   push %2 ; 7-8
    // 4   push %3 ; 9-10

    // 5   sub %1, 2 ; 11-13
    // 6   call %fib ; 1=2 14-15
    // 7   mov %3, %1 ; 16-18

    // 8   pop %3 ; 19-20
    // 9   add %3, 1 ; 21-23
    // 10  call %fib ; 1=2 24-25
    // 11  mov %3, %1 ; 26-28

    // 12  add %1, %3 ; 29-31

    // 13  pop %2 ; 32-33
    // 14  pop %3 ; 34-35

    // 15  end:
    // 15   ret ; 36

    enum REG { C = -3, B, A };

    static const char TEST[] = {
      JMP, 36,

      CMP, A, B,
      JLE, 36,
      
      PUSH, A,
      PUSH, B,
      
      SUB, A, 2,
      CALL, 2,
      MOV, C, A,

      POP, C,
      ADD, C, 1,
      CALL, 2,
      MOV, C, A,

      ADD, A, C,

      POP, B,
      POP, C,

      RET
    };
    
    Embed::Data compile(Util::LinkedList<Parser::Node *> nodes) {
      using namespace Parser;

      static Util::LinkedList<char> byteCode;
      static long long int p = 0;
      static LinkedMap<str, decltype(p)> locals;
      static struct {
        static bool handleStmt(Node *node) {
          if (!node) return false;
          return handleFn((Fn *) *node)
            || handleCall((Call *) *node)
            || handleVar((Var *) *node)
            || handleValue(node);
        }
        
        static bool handleIf(If *node) {
          if (!node) return false;
          return true;
        }

        static bool handleCall(Call *node) {
          if (!node) return false;
          byteCode.push_back(OpCode::CALL);
          
          // for (auto n : node->args) handleValue(n);
          return true;
        }

        static bool handleFn(Fn *node) {
          if (!node) return false;
          locals.set(node->name, p++);
          for (auto n : node->body) handleStmt(n);
          byteCode.push_back(OpCode::RET);
          return true;
        }

        static bool handleVar(Var *node) {
          if (!node) return false;
          handleValue(node->value);
          locals.set(node->name, p++);
          return true;
        }

        static bool handleValue(Node *node) {
          if (!node) return false;
          return handleBool((Bool *) *node)
            || handleInt((Int *) *node)
            || handleFloat((Float *) *node)
            || handleString((String *) *node)
            || handleId((Id *) *node);
        }

        static bool handleId(Id *node) {
          if (!node) return false;
          auto value = locals.get(node->value);
          if (!value) return false;
          auto data = reinterpret_cast<char *>((int) *value);
          long long size = sizeof(node->value);
          auto p = data;
          while (p - data < size) byteCode.push_back(*p++);
          return true;
        }

        static bool handleBool(Bool *node) {
          if (!node) return false;
          byteCode.push_back(node->value ? 1 : 0);
          return true;
        }

        static bool handleChar(Char *node) {
          if (!node) return false;
          auto p = node->value;
          while (*p) byteCode.push_back(*p++);
          byteCode.push_back('\0');
          return true;
        }

        static bool handleInt(Int *node) {
          if (!node) return false;
          auto data = reinterpret_cast<char *>(&node->value);
          long long size = sizeof(node->value);
          auto p = data;
          while (p - data < size) byteCode.push_back(*p++);
          return true;
        }

        static bool handleFloat(Float *node) {
          if (!node) return false;
          auto data = reinterpret_cast<char *>(&node->value);
          long long size = sizeof(node->value);
          auto p = data;
          while (p - data < size) byteCode.push_back(*p++);
          return true;
        }

        static bool handleString(String *node) {
          if (!node) return false;
          auto p = node->value;
          while (*p) byteCode.push_back(*p++);
          byteCode.push_back('\0');
          return true;
        }
      } fns;

      for (auto node : nodes) {
        fns.handleStmt(node);
      }
      
      auto len = [](Util::LinkedList<char> &list) {
        long long int size = 0;
        for (auto item = list.begin(); item != list.end(); ++item) ++size;
        return size;
      };

      auto toa = [](Util::LinkedList<char> &list, long long int size) {
        auto data = new char[size];
        auto item = list.begin();
        auto p = data;
        while (p - data < size) {
          *p = *item;
          ++p;
          ++item;
        }
        return data;
      };

      auto size = len(byteCode);

      return { size, toa(byteCode, size) };
    }
  }
}
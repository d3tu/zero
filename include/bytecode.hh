#pragma once

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
        operator const char *() const { return value; }
        operator const char *() { return value; }
        bool operator==(str &other) {
          const char *p0 = value, *p1 = other.value;
          while (*p0 && (*p0 == *p1)) { ++p0; ++p1; }
          return *p0 == *p1;
        }
    };

    template <typename K, typename V> struct KV { K key; V value; };
    template <typename K, typename V> class LinkedMap : Util::LinkedList<KV<K, V>> {
      public:
        void set(K key, V value) { push_back({ key, value }); }
        V *get(K key) {
          for (KV<K, V> &kv : list) if (kv.key == key) return &kv.value;
          return nullptr;
        }
    };

    enum OpCode { HLT, RET, JMP, CALL, CMP, JLE, PUSH, POP, ADD, SUB, MUL, DIV, MOV };

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

    Embed::Data compile(Util::LinkedList<Parser::Node *> nodes) {
      using namespace Parser;

      // if 1 > 0 {}
      //
      // cmp 1, 0
      // jle _if_0_0
      // ...
      // jmp _if_0_1
      // _if_0_0:
      // ...
      // _if_0_1:

      // If;   Fn;     Var;
      // Expr; Assign; Call; Id;
      // Bool; Char;   Int;  Float; String;

      static Util::LinkedList<char> target;
      static long long p = 0;
      static LinkedMap<str, decltype(p)> locals;
      static struct {
        static bool handleStmt(Node *node) {
          return !!node
            && handleIf((If *) *node)
            || handleFn((Fn *) *node)
            || handleVar((Var *) *node);
        }
        
        static bool handleIf(If *node) {
          if (!node) return false;
          return true;
        }

        static bool handleFn(Fn *node) {
          if (!node) return false;
          locals.set(node->name, p++);
          for (auto n : node->body) handleStmt(n);
          target.push_back(OpCode::RET);
          return true;
        }

        static bool handleVar(Var *node) {
          if (!node) return false;
          handleValue(node->value);
          locals.set(node->name, p++);
          return true;
        }

        static bool handleExpr(Expr *node) {

        }

        static bool handleAssign(Assign *node) {
        }

        static bool handleCall(Call *node) {
          if (!node) return false;
          target.push_back(OpCode::CALL);
          
          // for (auto n : node->args) handleValue(n);
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
          while (p - data < size) target.push_back(*p++);
          return true;
        }

        static bool handleBool(Bool *node) {
          if (!node) return false;
          target.push_back(node->value ? 1 : 0);
          return true;
        }

        static bool handleChar(Char *node) {
          if (!node) return false;
          auto p = node->value;
          while (*p) target.push_back(*p++);
          target.push_back('\0');
          return true;
        }

        static bool handleInt(Int *node) {
          if (!node) return false;
          auto value = node->value;
          target.push_back(value & 0xff);
          target.push_back((value >> 8) & 0xff);
          target.push_back((value >> 16) & 0xff);
          target.push_back((value >> 24) & 0xff);
          return true;
        }

        static bool handleFloat(Float *node) {
          if (!node) return false;
          auto data = reinterpret_cast<char *>(&node->value);
          long long size = sizeof(node->value);
          auto p = data;
          while (p - data < size) target.push_back(*p++);
          return true;
        }

        static bool handleString(String *node) {
          if (!node) return false;
          auto p = node->value;
          while (*p) target.push_back(*p++);
          target.push_back('\0');
          return true;
        }
      } fns;

      for (auto node : nodes) fns.handleStmt(node);
      
      auto len = [](Util::LinkedList<char> &list) {
        long long size = 0;
        for (auto item = list.begin(); item != list.end(); ++item) ++size;
        return size;
      };

      auto toa = [](Util::LinkedList<char> &list, long long size) {
        auto data = new char[size]; auto item = list.begin(); auto p = data;
        while (p - data < size) { *p = *item; ++p; ++item; }
        return data;
      };

      auto size = len(target);
      return { size, toa(target, size) };
    }
  }
}
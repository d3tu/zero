#pragma once
#include "parser.hh"
#include "embed.hh"
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
        void set(K key, V value) { this->push_back({ key, value }); }
        V *get(K key) {
          for (KV<K, V> &kv : *this) if (kv.key == key) return &kv.value;
          return nullptr;
        }
    };
    enum OpCode { HLT, RET, JMP, CALL, CMP, JLE, PUSH, POP, ADD, SUB, MUL, DIV, MOV };
    Embed::Data compile(Util::LinkedList<Parser::Node *> nodes) {
      using namespace Parser;
      static Util::LinkedList<char> target;
      static long long p = 0;
      static LinkedMap<str, decltype(p)> locals;
      static struct {
        static bool handleStmt(Node *node) {
          return !!node && (
            handleIf((If *) *node) ||
            handleFn((Fn *) *node) ||
            handleVar((Var *) *node)
          );
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
          if (!node) return false;
          return true;
        }
        static bool handleAssign(Assign *node) {
          if (!node) return false;
          return true;
        }
        static bool handleCall(Call *node) {
          if (!node) return false;
          target.push_back(OpCode::CALL);
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
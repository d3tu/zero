#pragma once

#include "util.hh"

namespace Core {
  namespace Parser {
    class Stmt {
      public:
        virtual ~Stmt() = default;

        template <typename T> operator T *() {
          return dynamic_cast<T *>(this);
        }
    };

    class Var : public Stmt {
      public:
        bool mut;

        const char *type;
        const char *name;

        Stmt *value;

        Var(bool mut, const char *type, const char *name, Stmt *value):
          mut(mut), type(type), name(name), value(value) {}
    };

    class Assign : public Stmt {
      public:
        const char *name;

        Stmt *value;

        Assign(const char *name, Stmt *value):
          name(name), value(value) {}
    };

    class If : public Stmt {
      public:
        Stmt *expr;

        Util::LinkedList<Stmt *> scope;
        Util::LinkedList<Stmt *> other;

        If(Stmt *expr, Util::LinkedList<Stmt *> scope, Util::LinkedList<Stmt *> other):
          expr(expr), scope(scope), other(other) {}
    };
    
    class Method : public Stmt {
      public:
        const char *type;
        const char *name;

        Util::LinkedList<Stmt *> args;
        Util::LinkedList<Stmt *> scope;

        Method(const char *type, const char *name, Util::LinkedList<Stmt *> args, Util::LinkedList<Stmt *> scope):
          type(type), name(name), args(args), scope(scope) {}
    };

    class Call : public Stmt {
      public:
        const char *name;

        Util::LinkedList<Stmt *> args;

        Call(const char *name, Util::LinkedList<Stmt *> args):
          name(name), args(args) {}
    };
    
    class While : public Stmt {
      public:
        Stmt *expr;
        
        Util::LinkedList<Stmt *> scope;

        While(Stmt *expr, Util::LinkedList<Stmt *> scope):
          expr(expr), scope(scope) {}
    };

    class Return : public Stmt {
      public:
        Stmt *value;

        Return(Stmt *value):
          value(value) {}
    };

    class Label : public Stmt {
      public:
        const char *name;

        Label(const char *name):
          name(name) {}
    };

    class Jump : public Stmt {
      public:
        const char *name;

        Jump(const char *name):
          name(name) {}
    };

    class Continue : public Stmt {};

    class Break : public Stmt {};

    class Expr : public Stmt {
      public:
        enum Op {
          Not, Plus, Minus, Add, Sub, Mul, Div
        };

        Op op;

        Stmt *left;
        Stmt *right;

        Expr(Op op, Stmt *left, Stmt *right):
          op(op), left(left), right(right) {}
    };

    class Value : public Stmt {
      public:
        enum Type {
          Id, Bool, Char, Short, Int, Long, Float, Double, String
        };

        Type type;

        const char *value;

        Value(Type type, const char *value):
          type(type), value(value) {}
    };
  }
}
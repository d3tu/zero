#pragma once

#include "util.hh"

namespace DJ {
  namespace Parser {
    class Stmt {
      public:
        virtual ~Stmt() = default;

        template <typename T> operator T *() {
          return dynamic_cast<T *>(this);
        }
    };

    using str = const char *;

    using list = Util::LinkedList<Stmt *>;

    class Var : public Stmt {
      public:
        bool mut;

        str type;
        str name;

        Stmt *value;

        Var(bool mut, str type, str name, Stmt *value):
          mut(mut), type(type), name(name), value(value) {}
    };

    class Assign : public Stmt {
      public:
        str name;

        Stmt *value;

        Assign(str name, Stmt *value):
          name(name), value(value) {}
    };

    class If : public Stmt {
      public:
        Stmt *expr;

        list scope;
        list other;

        If(Stmt *expr, list scope, list other):
          expr(expr), scope(scope), other(other) {}
    };
    
    class Method : public Stmt {
      public:
        str type;
        str name;

        list args;
        list scope;

        Method(str type, str name, list args, list scope):
          type(type), name(name), args(args), scope(scope) {}
    };

    class Call : public Stmt {
      public:
        str name;

        list args;

        Call(str name, list args):
          name(name), args(args) {}
    };
    
    class While : public Stmt {
      public:
        Stmt *expr;
        
        list scope;

        While(Stmt *expr, list scope):
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
        str name;

        Label(str name):
          name(name) {}
    };

    class Jump : public Stmt {
      public:
        str name;

        Jump(str name):
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

        str value;

        Value(Type type, str value):
          type(type), value(value) {}
    };
  }
}
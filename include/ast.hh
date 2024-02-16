#pragma once

namespace DJ {
  namespace Parser {
    class Node {
      public:
        virtual ~Node() = default;

        template <typename T> operator T *() {
          return dynamic_cast<T *>(this);
        }
    };

    class Id : public Node {
      public:
        const char *value;

        Id(decltype(value) value):
          value(value) {}
    };

    class Bool : public Node {
      public:
        bool value;

        Bool(decltype(value) value):
          value(value) {}
    };

    class Char : public Node {
      public:
        const char *value;

        Char(decltype(value) value):
          value(value) {}
    };

    class Int : public Node {
      public:
        int value;
        
        Int(decltype(value) value):
          value(value) {}
    };

    class Float : public Node {
      public:
        float value;

        Float(decltype(value) value):
          value(value) {}
    };

    class String : public Node {
      public:
        const char *value;

        String(decltype(value) value):
          value(value) {}
    };

    class Var : public Node {
      public:
        const char *type, *name;
        Node *value;

        Var(decltype(type) type, decltype(name) name, decltype(value) value): 
          type(type), name(name), value(value) {}
    };

    class Assign : public Node {
      public:
        const char *name;
        Node *value;

        Assign(decltype(name) name, decltype(value) value):
          name(name), value(value) {}
    };

    class Fn : public Node {
      public:
        const char *type, *name;
        Util::LinkedList<Node *> vars, body;

        Fn(decltype(type) type, decltype(name) name, decltype(vars) vars, decltype(body) body):
          type(type), name(name), vars(vars), body(body) {}
    };

    class Call : public Node {
      public:
        const char *name;
        Util::LinkedList<Node *> args;

        Call(decltype(name) name, decltype(args) args):
          name(name), args(args) {}
    };

    class If : public Node {
      public:
        Node *expr;
        Util::LinkedList<Node *> body, elses, other;

        If(decltype(expr) expr, decltype(body) body, decltype(elses) elses, decltype(other) other):
          expr(expr), body(body), elses(elses), other(other) {}
    };

    enum class Op { Not, Plus, Minus, And, Or, Equals, NotEquals, Major, Minor, MajorEquals, MinorEquals, Add, Sub, Mul, Div };

    class Expr : public Node {
      public:
        Op op;
        Node *left, *right;

        Expr(decltype(op) op, decltype(left) left, decltype(right) right):
          op(op), left(left), right(right) {}
    };
  }
}
#pragma once

#include "util.hh"
#include "lexer.hh"

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

    class Int : public Node {
      public:
        int value;
        
        Int(decltype(value) value):
          value(value) {}
    };

    class Bool : public Node {
      public:
        bool value;

        Bool(decltype(value)  value):
          value(value) {}
    };

    class Float : public Node {
      public:
        float value;

        Float(decltype(value)  value):
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

    class Function : public Node {
      public:
        const char *type, *name;
        Util::LinkedList<Node *> vars, body;

        Function(decltype(type) type, decltype(name) name, decltype(vars) vars, decltype(body) body):
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
      
    Util::LinkedList<Node *> parse(Util::LinkedList<Lexer::Token> tokens) {
      using namespace Lexer;
      Util::LinkedList<Node *> nodes;
      auto tok = tokens.begin();
      using Iter = decltype(tok);

      static auto cmpstr = [](Token token, const char *value) {
        auto p0 = value, p1 = token.start;
        while (*p0 != '\0') if (*p0++ != *p1++) return false;
        return true;
      };

      static auto cmp = [](Token token, Type type, const char *value = "") {
        return token.type == type && cmpstr(token, value);
      };
      
      static struct {
        Node *parseStmt(Iter &tok) {
          Node *node;

          if (node = parseIf(tok)) return node;
          else if (node = parseFn(tok)) return node;
          else if (node = parseVar(tok)) return node;
          else if (node = parseExpr(tok)) return node;
          else return node;
        }

        Node *parseIf(Iter &tok) {
          if (!!tok && cmp(*tok, Type::Identifier, "if")) {
            auto expr = parseExpr(++tok);

            if (!!tok && cmp(*tok++, Type::Symbol, "{")) {
              Util::LinkedList<Node *> body, elses, other;

              while (true) {
                auto stmt = parseStmt(tok);
                if (stmt) body.push_back(stmt);
                else break;
              }

              if (!tok || !cmp(*tok++, Type::Symbol, "}")) throw Util::Exception("escopo nao fechado...");
              else return new If(expr, body, elses, other);
            } else throw Util::Exception("if sem escopo...");
          }

          return nullptr;
        }
        
        Node *parseFn(Iter &tok) {
          auto _tok = tok;

          if (!!tok && (*tok).type == Type::Identifier) {
            auto type = *tok++;

            if (!!tok && (*tok).type == Type::Identifier) {
              auto name = *tok++;

              if (!!tok && cmp(*tok++, Type::Symbol, "(")) {
                Util::LinkedList<Node *> vars;

                while (true) {
                  auto var = parseVar(tok);

                  if (!!tok && cmp(*tok, Type::Symbol, ",")) {
                    ++tok;

                    if (!var) throw Util::Exception("e esse `,`?");
                  }

                  if (var) vars.push_back(var);
                  else break;
                }
                
                if (!tok || !cmp(*tok++, Type::Symbol, ")")) throw Util::Exception("lista de variaveis nao fechada...");
                else if (!!tok && cmp(*tok++, Type::Symbol, "{")) {
                  Util::LinkedList<Node *> body;

                  while (true) {
                    auto stmt = parseStmt(tok);
                    if (stmt) body.push_back(stmt);
                    else break;
                  }

                  if (!tok || !cmp(*tok++, Type::Symbol, "}")) throw Util::Exception("escopo nao fechado...");
                  else return new Function(toa(type), toa(name), vars, body);
                } else Util::Exception("funcao sem escopo...");
              }
            }
          }

          tok = _tok;
          return nullptr;
        }

        Node *parseVar(Iter &tok) {
          auto _tok = tok;

          if (!!tok && (*tok).type == Type::Identifier) {
            auto type = *tok++;

            if (!!tok && (*tok).type == Type::Identifier) {
              auto name = *tok++;
              Node *value = nullptr;

              if (!!tok && cmp(*tok, Type::Symbol, "=")) {
                if (!(value = parseExpr(++tok))) throw Util::Exception("declaracao sem valor...");
              }

              return new Var(toa(type), toa(name), value);
            }
          }

          tok = _tok;
          return nullptr;
        }
        
        Node *parseExpr(Iter &tok) {
          if (!tok) return nullptr;

          Node *node;

          if (node = parseAssign(tok)) return node;
          else if (node = parseCall(tok)) return node;
          else if (node = parseValue(tok)) return node;
          else if ((*tok).type == Type::Symbol) {
            Node *expr;

            if (cmpstr(*tok, "!")) {
              expr = parseExpr(++tok);
              if (!expr) throw Util::Exception("sem expressao... `!`");
              else return new Expr(Op::Not, expr, nullptr);
            } else if (cmpstr(*tok, "+")) {
              expr = parseExpr(++tok);
              if (!expr) throw Util::Exception("sem expressao... `+`");
              else return new Expr(Op::Major, expr, nullptr);
            } else if (cmpstr(*tok, "-")) {
              expr = parseExpr(++tok);
              if (!expr) throw Util::Exception("sem expressao... `-`");
              else return new Expr(Op::Minus, expr, nullptr);
            } else if (cmpstr(*tok, "(")) {
              expr = parseExpr(++tok);
              if (!expr) throw Util::Exception("sem expressao...");
              else if (!tok || !cmp(*tok++, Type::Symbol, ")")) throw Util::Exception("expressao nao fechada");
              else return expr;
            }
          }
          
          return node;
        }
        
        Node *parseAssign(Iter &tok) {
          auto _tok = tok;

          if (!!tok && (*tok).type == Type::Identifier) {
            auto name = *tok++;

            if (!!tok && cmp(*tok, Type::Symbol, "=")) {
              auto value = parseExpr(++tok);
              if (!value) throw Util::Exception("atribuicao sem valor...");
              else return new Assign(toa(name), value);
            }
          }

          tok = _tok;
          return nullptr;
        }
        
        Node *parseCall(Iter &tok) {
          auto _tok = tok;

          if (!!tok && (*tok).type == Type::Identifier) {
            auto name = *tok++;

            if (!!tok && cmp(*tok++, Type::Symbol, "(")) {
              Util::LinkedList<Node *> args;

              while (true) {
                auto value = parseExpr(tok);

                if (!!tok && cmp(*tok, Type::Symbol, ",")) {
                  ++tok;

                  if (!value) throw Util::Exception("e esse `,`?");
                }

                if (value) args.push_back(value);
                else break;
              }
              
              if (!tok || !cmp(*tok++, Type::Symbol, ")")) throw Util::Exception("lista de args n fechada...");
            }
          }
          
          tok = _tok;
          return nullptr;
        }
        
        Node *parseValue(Iter &tok) {
          if (!tok) return nullptr;

          auto _tok = tok++;

          switch ((*_tok).type) {
            case Type::Integer: return new Int(toi(*_tok));
            case Type::Decimal: return new Float(tof(*_tok));
            case Type::String: return new String(toa(*_tok));
            case Type::Identifier:
              if (cmpstr(*_tok, "true")) return new Bool(true);
              else if (cmpstr(*_tok, "false")) return new Bool(false);
              else return new Id(toa(*_tok));
            default:
              tok = _tok;
              return nullptr;
          }
        }
      } fns;

      Node *node;

      each: if ((node = fns.parseStmt(tok))) {
        nodes.push_back(node);
        goto each;
      }

      return nodes;
    }
  }
}
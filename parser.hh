#pragma once

#include "util.hh"
#include "lexer.hh"
#include "types.hh"

namespace Core {
  namespace Parser {
    bool cmp(Lexer::Token token, const char *value) {
      auto p0 = value;
      auto p1 = token.start;

      while (*p0 && (*p0 == *p1)) {
        ++p0;
        ++p1;
      }
      
      return !*p0 && (!*p1 || p1 - 1 == token.end);
    };

    Util::LinkedList<Stmt *> parse(Util::LinkedList<Lexer::Token> tokens) {
      using namespace Util;
      using namespace Lexer;

      LinkedList<Stmt *> nodes;

      auto tok = tokens.begin();
      
      Stack<void *> locals;
      Stack<void *> backup;
      Stack<Stmt *> values;

      #define LOCALS locals
      #define BACKUP backup
      #define VALUES values

      each: {
        Stmt *node;

        CALL(parseStmt)

        if ((node = POP)) {
          nodes.push(node);
          goto each;
        }
      }

      return nodes;

      #define RET_STMT(NAME) CALL(NAME) if ((node = POP)) { PUSH(node) RET }

      parseStmt: {
        Stmt *node;

        RET_STMT(parseIf)
        RET_STMT(parseFor)
        RET_STMT(parseReturn)
        RET_STMT(parseContinue)
        RET_STMT(parseBreak)
        RET_STMT(parseLabel)
        RET_STMT(parseMethod)
        RET_STMT(parseVar)
        RET_STMT(parseExpr)
          
        PUSH(nullptr)
        RET
      }
      // if expr { ... } else { ... } OK
      parseIf: {
        if (!tok || (*tok).type != Type::Id || !cmp(*tok, "if")) {
          PUSH(nullptr)
          RET
        }

        ++tok;

        CALL(parseExpr)
        auto expr = POP;

        if (!expr) {
          throw Exception("IfExpressionExpected");
        }

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "{")) {
          throw Exception("IfScopeExpected");
        }

        ++tok;
        
        LinkedList<Stmt *> scope;
        
        SAVE(&expr)

        while (true) {
          SAVE(&scope)

          CALL(parseStmt)
          auto stmt = POP;
          
          RESTORE(scope);
          
          if (!stmt) {
            break;
          }
          
          scope.push(stmt);
        }

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "}")) {
          throw Exception("IfScopeNotClosed");
        }

        ++tok;

        LinkedList<Stmt *> other;

        if (!!tok && (*tok).type == Type::Id && cmp(*tok, "else")) {
          ++tok;

          if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "{")) {
            throw Exception("ElseScopeExpected");
          }

          ++tok;

          while (true) {
            SAVE(&other)

            CALL(parseStmt)
            auto stmt = POP;
            
            RESTORE(other);
            
            if (!stmt) {
              break;
            }

            other.push(stmt);
          }

          if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "}")) {
            throw Exception("IfScopeNotClosed");
          }

          ++tok;
        }

        RESTORE(expr);
        
        PUSH(new If(expr, scope, other))
        RET
      }
      // for { ... }; for expr { ... } OK
      parseFor: {
        if (!tok || (*tok).type != Type::Id || !cmp(*tok, "for")) {
          PUSH(nullptr)
          RET
        }

        ++tok;

        CALL(parseExpr)
        auto expr = POP;

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "{")) {
          throw Exception("ForScopeExpected");
        }

        ++tok;

        LinkedList<Stmt *> scope;

        SAVE(&expr)

        while (true) {
          SAVE(&scope)

          CALL(parseStmt)
          auto stmt = POP;

          RESTORE(scope);
          
          if (!stmt) {
            break;
          }
          
          scope.push(stmt);
        }
        
        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "}")) {
          throw Exception("ForScopeNotClosed");
        }

        RESTORE(expr);

        PUSH(new For(expr, scope))
        RET
      }
      // return; return expr; OK
      parseReturn: {
        if (!tok || (*tok).type != Type::Id || !cmp(*tok, "return")) {
          PUSH(nullptr)
          RET
        }

        auto t = tok;

        ++tok;

        if (*((*t).end + 1) != '\n' || *((*t).end + 1) != '\r') {
          if (!!tok && (*tok).type == Type::Id) {
            PUSH(new Return(POP))
            RET
          }
        }

        PUSH(new Return(nullptr))
        RET
      }
      // continue; continue expr; +/-
      parseContinue: {
        if (!tok || (*tok).type != Type::Id || !cmp(*tok, "continue")) {
          PUSH(nullptr)
          RET
        }

        auto t = tok;

        ++tok;

        if (*((*t).end + 1) != '\n' || *((*t).end + 1) != '\r') {
          if (!!tok && (*tok).type == Type::Id) {
            CALL(parseValue)
            PUSH(new Continue(POP))
            RET
          }
        }

        PUSH(new Continue(nullptr))
        RET
      }
      // OK ~ break;
      parseBreak: {
        if (!tok || (*tok).type != Type::Id || !cmp(*tok, "break")) {
          PUSH(nullptr)
          RET
        }

        ++tok;

        PUSH(new Break())
        RET
      }
      // OK ~ label:
      parseLabel: {
        if (!tok || (*tok).type != Type::Id) {
          PUSH(nullptr)
          RET
        }
        
        auto name = tok;

        ++tok;

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, ":")) {
          tok = name;

          PUSH(nullptr)
          RET
        }

        ++tok;

        PUSH(new Label(toa(*name)))
        RET
      }
      // OK ~ type name(...) { ... }
      parseMethod: {
        if (!tok || (*tok).type != Type::Id) {
          PUSH(nullptr)
          RET
        }

        auto type = tok;

        ++tok;

        if (!tok || (*tok).type != Type::Id) {
          tok = type;

          PUSH(nullptr)
          RET
        }

        auto name = tok;

        ++tok;

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "(")) {
          tok = type;

          PUSH(nullptr)
          RET
        }

        ++tok;

        LinkedList<Stmt *> args;

        SAVE(&type)
        SAVE(&name)
        
        bool flag = false;

        while (true) {
          SAVE(&flag)
          SAVE(&args)

          CALL(parseStmt)
          auto arg = POP;

          RESTORE(args);
          RESTORE(flag);

          if (!!tok && (*tok).type == Type::Symbol && cmp(*tok, ",")) {
            if (!arg) {
              throw Exception("MethodInvalidCommaUsage");
            }

            ++tok;

            flag = true;
          }
          
          if (!arg) {
            if (!flag) {
              throw Exception("MethodInvalidCommaUsage");
            }

            break;
          }

          args.push(arg);

          flag = false;
        }

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, ")")) {
          throw Exception("MethodArgsNotClosed");
        }

        ++tok;

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "{")) {
          throw Exception("MethodScopeExpected");
        }

        ++tok;

        LinkedList<Stmt *> scope;

        while (true) {
          SAVE(&flag)
          SAVE(&scope)

          CALL(parseStmt)
          auto arg = POP;

          RESTORE(scope);
          RESTORE(flag);

          if (!!tok && (*tok).type == Type::Symbol && cmp(*tok, ",")) {
            if (!arg) {
              throw Exception("MethodInvalidCommaUsage");
            }
            
            ++tok;

            flag = true;
          }
          
          if (!arg) {
            if (flag) {
              throw Exception("MethodInvalidCommaUsage");
            }

            break;
          }

          args.push(arg);
          
          flag = false;
        }

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "}")) {
          throw Exception("MethodScopeExpected");
        }

        ++tok;

        RESTORE(name);
        RESTORE(type);

        PUSH(new Method(toa(*type), toa(*name), args, scope))
        RET
      }
      // +- ~ type name; type name = expr; const type name = value; const name = value; let name = value
      parseVar: {
        if (!tok || (*tok).type != Type::Id) {
          PUSH(nullptr)
          RET
        }

        auto type = tok;

        ++tok;

        if (!tok || (*tok).type != Type::Id) {
          tok = type;

          PUSH(nullptr)
          RET
        }

        auto name = tok;

        ++tok;

        Stmt *value = nullptr;

        if (!!tok && (*tok).type == Type::Symbol && cmp(*tok, "=")) {
          SAVE(&type)
          SAVE(&name)
          
          ++tok;
          
          CALL(parseExpr)
          value = POP;

          if (!value) {
            throw Exception("VarValueExpected");
          }

          RESTORE(name);
          RESTORE(type);
        }

        PUSH(new Var(true, toa(*type), toa(*name), value))
        RET
      }
      // +-
      parseExpr: {
        if (!tok) {
          PUSH(nullptr)
          RET
        }

        Expr::Op op;

        if ((*tok).type == Type::Symbol) {
          if (cmp(*tok, "+")) op = Expr::Plus;
          else if (cmp(*tok, "-")) op = Expr::Minus;
          else if (cmp(*tok, "!")) op = Expr::Not;
          else if (cmp(*tok, "(")) {
            ++tok;

            CALL(parseExpr)
            auto expr = POP;

            if (!expr) {
              throw Exception("ExpressionExpected");
            }

            if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, ")")) {
              throw Exception("ExpressionEndExpected");
            }

            ++tok;

            PUSH(expr)
            RET
          } else {
            PUSH(nullptr)
            RET
          }

          ++tok;

          SAVE(&op)

          CALL(parseExpr)
          auto expr = POP;

          RESTORE(op);

          PUSH(new Expr(op, expr, nullptr))
          RET
        }

        Stmt *left;

        SAVE(&left)

        CALL(parseIf)
        if (!(left = POP)) {
          RESTORE(left);
          SAVE(&left)

          CALL(parseAssign)
          if (!(left = POP)) {
            RESTORE(left);
            SAVE(&left)

            CALL(parseCall)
            if (!(left = POP)) {
              RESTORE(left);
              SAVE(&left)

              CALL(parseValue)
              if (!(left = POP)) {
                PUSH(nullptr)
                RET
              }
            }
          }
        }
        
        if ((If *) *left) {
          if (!((If *) *left)->scope.begin()) {
            throw Exception("IfValueExpected");
          }

          if (!((If *) *left)->other.begin()) {
            throw Exception("ElseValueExpected");
          }
        }

        auto _left = left;
        
        RESTORE(left);
        
        left = _left;

        if (!!tok && (*tok).type == Type::Symbol) {
          if (cmp(*tok, "+")) op = Expr::Add;
          else if (cmp(*tok, "-")) op = Expr::Sub;
          else if (cmp(*tok, "*")) op = Expr::Mul;
          else if (cmp(*tok, "/")) op = Expr::Div;
          else {
            PUSH(left)
            RET
          }

          SAVE(&op)
          SAVE(&left)

          CALL(parseExpr)
          auto right = POP;

          if (!right) {
            throw Exception("ExpressionMissingRightSide");
          }

          RESTORE(left);
          RESTORE(op);

          PUSH(new Expr(op, left, right))
          RET
        }

        PUSH(left)
        RET
      }
      // name = expr OK
      parseAssign: {
        if (!tok || (*tok).type != Type::Id) {
          PUSH(nullptr)
          RET
        }

        auto name = tok;

        ++tok;

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "=")) {
          tok = name;

          PUSH(nullptr)
          RET
        }

        ++tok;

        SAVE(&name)

        CALL(parseExpr)
        auto value = POP;

        if (!value) {
          throw Exception("AssignValueExpected");
        }

        RESTORE(name);

        PUSH(new Assign(toa(*name), value))
        RET
      }
      // +- name(...)
      parseCall: {
        if (!tok || (*tok).type != Type::Id) {
          PUSH(nullptr)
          RET
        }
        
        auto name = tok;

        ++tok;

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "(")) {
          tok = name;

          PUSH(nullptr)
          RET
        }

        ++tok;

        SAVE(&name)
        
        LinkedList<Stmt *> args;

        bool flag = false;

        while (true) {
          SAVE(&flag)
          SAVE(&args)

          CALL(parseStmt)
          auto arg = POP;

          RESTORE(args);
          RESTORE(flag);

          if (!!tok && (*tok).type == Type::Symbol && cmp(*tok, ",")) {
            if (!arg) {
              throw Exception("CallInvalidCommaUsage");
            }

            ++tok;

            flag = true;
          }

          if (!arg) {
            if (flag) {
              throw Exception("CallInvalidCommaUsage");
            }

            break;
          }

          args.push(arg);

          flag = false;
        }

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, ")")) {
          throw Exception("CallArgListNotClosed");
        }

        RESTORE(name);

        PUSH(new Call(toa(*name), args))
        RET
      }
      // +-
      parseValue: {
        if (!tok) {
          PUSH(nullptr)
          RET
        }

        switch ((*tok).type) {
          case Type::Id:
            if (cmp(*tok, "true") || cmp(*tok, "false")) {
              PUSH(new Value(Value::Bool, toa(*tok)))

              ++tok;
              
              RET
            }

            PUSH(new Value(Value::Id, toa(*tok)))
            
            ++tok;
            
            
            RET
          
          case Type::Char:
            PUSH(new Value(Value::Char, toa(*tok)))

            ++tok;
            
            RET

          case Type::Int:
            PUSH(new Value(Value::Int, toa(*tok)))

            ++tok;
            
            RET

          case Type::Float:
            PUSH(new Value(Value::Float, toa(*tok)))

            ++tok;
            
            RET

          case Type::String:
            PUSH(new Value(Value::String, toa(*tok)))

            ++tok;
            
            RET

          default:
            PUSH(nullptr)
            RET
        }
      }
    }
  }
}
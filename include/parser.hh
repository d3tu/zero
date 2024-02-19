#pragma once

#include "util.hh"
#include "lexer.hh"
#include "types.hh"
#include "stack.hh"
#include "header.hh"

namespace DJ {
  namespace Parser {
    Util::LinkedList<Stmt *> parse(Util::LinkedList<Lexer::Token> tokens) {
      using namespace Util;
      using namespace Lexer;

      Util::LinkedList<Stmt *> nodes;

      auto tok = tokens.begin();
      
      using Iter = decltype(tok);
      
      static auto cmp = [](Token token, const char *value) {
        auto p0 = value;
        auto p1 = token.start;

        while (*p0 && (*p0 == *p1)) {
          ++p0;
          ++p1;
        }
        
        return !*p0 && (!*p1 || p1 - 1 == token.end);
      };
      
      Stack<void *> locals;
      Stack<void *> backup;
      Stack<Stmt *> values;

      #define LOCALS locals
      #define BACKUP backup
      #define VALUES values

      Stmt *node;

      each:
        CALL(parseStmt)

        if ((node = POP)) {
          nodes.push(node);
          goto each;
        }

      return nodes;

      #define RET_STMT(NAME) CALL(NAME) if ((node = POP)) { PUSH(node) RET }

      parseStmt: {
        Stmt *node;

        RET_STMT(parseIf)
        RET_STMT(parseWhile)
        RET_STMT(parseReturn)
        RET_STMT(parseContinue)
        RET_STMT(parseBreak)
        RET_STMT(parseJump)
        RET_STMT(parseLabel)
        RET_STMT(parseMethod)
        RET_STMT(parseVar)
        RET_STMT(parseExpr)
          
        PUSH(nullptr)
        RET
      }

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
        
        list scope;
        
        SAVE(&expr)

        while (true) {
          SAVE(&scope)
          CALL(parseStmt)
          auto stmt = POP;
          RESTORE(scope);
          if (!stmt) break;
          scope.push(stmt);
        }

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "}")) {
          throw Exception("IfScopeNotClosed");
        }

        ++tok;

        list other;

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
            if (!stmt) break;
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
        }

        RESTORE(name);
        RESTORE(type);
        PUSH(new Var(true, toa(*type), toa(*name), value))
        RET
      }

      parseLabel: {
        if (!tok || (*tok).type != Type::Id) {
          PUSH(nullptr)
          RET
        }
        
        auto name = tok;

        ++tok;

        if (!tok || (*tok).type != Type::Symbol || cmp(*tok, ":")) {
          tok = name;

          PUSH(nullptr)
          RET
        }

        ++tok;

        PUSH(new Label(toa(*name)))
        RET
      }
      
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

        list args;

        SAVE(&type);
        SAVE(&name);

        while (true) {
          SAVE(&args);
          CALL(parseStmt)
          auto arg = POP;
          RESTORE(args);
          if (!arg) break;
          args.push(arg);
        }

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, ")")) {
          throw Exception("MethodArgsNotClosed");
        }

        ++tok;

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "{")) {
          throw Exception("MethodScopeExpected");
        }

        ++tok;

        list scope;

        while (true) {
          SAVE(&scope)
          CALL(parseStmt)
          RESTORE(scope);
          auto arg = POP;
          if (!arg) break;
          args.push(arg);
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

      parseWhile: {
        if (!tok || (*tok).type != Type::Id || !cmp(*tok, "while")) {
          PUSH(nullptr)
          RET
        }

        ++tok;

        CALL(parseExpr)
        auto expr = POP;

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "{")) {
          throw Exception("WhileScopeExpected");
        }

        ++tok;

        list scope;

        while (true) {
          SAVE(&scope)
          CALL(parseStmt)
          RESTORE(scope);
          auto stmt = POP;
          if (!stmt) break;
          scope.push(stmt);
        }
        
        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, "}")) {
          throw Exception("WhileScopeNotClosed");
        }

        PUSH(new While(expr, scope))
        RET
      }

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
        
        list args;

        while (true) {
          SAVE(&args)
          CALL(parseStmt)
          auto arg = POP;
          RESTORE(args);
          if (!arg) break;
          args.push(arg);
        }

        if (!tok || (*tok).type != Type::Symbol || !cmp(*tok, ")")) {
          throw Exception("CallArgListNotClosed");
        }

        RESTORE(name);

        PUSH(new Call(toa(*name), args))
        RET
      }

      parseReturn: {
        if (!tok || (*tok).type != Type::Id || !cmp(*tok, "return")) {
          PUSH(nullptr)
          RET
        }

        ++tok;

        CALL(parseExpr)
        auto value = POP;

        PUSH(new Return(value))
        RET
      }

      parseJump: {
        if (!tok || (*tok).type != Type::Id || !cmp(*tok, "jump")) {
          PUSH(nullptr)
          RET
        }

        ++tok;

        if (!tok || (*tok).type != Type::Id) {
          throw Exception("JumpLabelExpected");
        }

        auto name = tok;

        ++tok;

        PUSH(new Label(toa(*name)))
        RET
      }

      parseContinue: {
        if (!tok || (*tok).type != Type::Id || !cmp(*tok, "continue")) {
          PUSH(nullptr)
          RET
        }

        ++tok;

        PUSH(new Continue())
        RET
      }

      parseBreak: {
        if (!tok || (*tok).type != Type::Id || !cmp(*tok, "break")) {
          PUSH(nullptr)
          RET
        }

        ++tok;

        PUSH(new Break())
        RET
      }

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
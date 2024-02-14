#pragma once

#include "util.hh"

namespace DJ {
  namespace Lexer {
    enum class Type {
      Identifier,
      Integer,
      Decimal,
      String,
      Char,
      Symbol,

      excl = '!',
      num = '#',
      percnt = '%',
      amp = '&',
      lpar = '(',
      rpar = ')',
      ast = '*',
      plus = '+',
      comma = ',',
      minus = '-',
      period = '.',
      sol = '/',
      colon = ':',
      semi = ';',
      lt = '<',
      equals = '=',
      gt = '>',
      quest = '?',
      commat = '@',
      lsqb = '[',
      rsqb = ']',
      circ = '^',
      lcub = '{',
      verbar = '|',
      rcub = '}',
      tilde = '~',
    };
    
    Type type(char c) {
      switch (c) {
        case '!': return Type::excl;
        case '#': return Type::num;
        case '%': return Type::percnt;
        case '&': return Type::amp;
        case '(': return Type::lpar;
        case ')': return Type::rpar;
        case '*': return Type::ast;
        case '+': return Type::plus;
        case ',': return Type::comma;
        case '-': return Type::minus;
        case '.': return Type::period;
        case '/': return Type::sol;
        case ':': return Type::colon;
        case ';': return Type::semi;
        case '<': return Type::lt;
        case '=': return Type::equals;
        case '>': return Type::gt;
        case '?': return Type::quest;
        case '@': return Type::commat;
        case '[': return Type::lsqb;
        case ']': return Type::rsqb;
        case '^': return Type::circ;
        case '{': return Type::lcub;
        case '|': return Type::verbar;
        case '}': return Type::rcub;
        case '~': return Type::tilde;
        default: throw "NotValidSymbol";
      }
    }

    struct Token {
      Type type;
      const char *start;
      const char *end;
    };

    const char *toa(Token token) {
      auto value(new char[token.end - token.start + 1]);
      auto p0(token.start);
      auto p1(value);

      while (p0 != token.end + 1) {
        *p1 = *p0;

        ++p0;
        ++p1;
      }
      
      *p1 = '\0';

      return value;
    }

    int toi(Token token) {
      int value(0);

      auto p(token.start);

      while (p != token.end + 1) {
        value = value * 10 + (*p - '0');
        ++p;
      }

      return value;
    }

    float tof(Token token) {
      float value(.0), depth(.0);

      auto p(token.start);

      while (p != token.end + 1) {
        if (depth > .0) {
          value += (*p - '0') / (depth *= 10.);
        } else if (*p == '.') {
          depth = 1.;
        } else {
          value = value * 10. + (*p - '0');
        }

        ++p;
      }

      return value;
    }

    Util::LinkedList<Token> tokenize(const char *source) {
      Util::LinkedList<Token> tokens;

      auto isAIR = [](char c) {
        return c == ' ' || c == '\n' || c == '\r';
      };

      auto isNUM = [](char c) {
        return c >= '0' && c <= '9';
      };

      auto isVAL = [](char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '$' || c == '_';
      };

      auto isSYM = [](char c) {
        static const char *SYMBOLS = "!#%&()*+,-./:;<=>?@[]^{|}~";

        auto p = SYMBOLS;

        while (*p != '\0') {
          if (*p++ == c) {
            return true;
          }
        }

        return false;
      };

      const char *p(source);

      while (*p != '\0') {
        if (isNUM(*p)) {
          Token token { Type::Integer, p++ };

          bool flag(false);
          
          while (true) {
            if (*p == '.') {
              if (flag) {
                throw Util::Exception("DotAlreadyUsed");
              }

              flag = true;
            } else if (!isNUM(*p)) {
              break;
            }
            
            ++p;
          }

          token.end = p - 1;

          if (flag) {
            token.type = Type::Decimal;
          }

          tokens.push_back(token);
        } else if (isVAL(*p)) {
          Token token { Type::Identifier, p++ };

          while (isVAL(*p) || isNUM(*p)) {
            ++p;
          }

          token.end = p - 1;

          tokens.push_back(token);
        } else if (*p == '"') {
          Token token { Type::String, ++p };

          while (*p != '"' || *p != '\n' || *p != '\0') {
            if (*p == '\\') {
              ++p;
            }

            ++p;
          }

          token.end = p - 1;

          if (*p++ != '"') {
            throw Util::Exception("StringNotClosed");
          }

          tokens.push_back(token);
        } else if (*p == '\'') {
          Token token { Type::Char, ++p };
          
          if (*p == '\\') {
            ++p;
          }

          token.end = p - 1;

          if (*++p != '\'') {
            throw Util::Exception("CharNotClosed");
          }

          tokens.push_back(token);
        } else if (isSYM(*p)) {
          Token token { Type::Symbol, p, p++ };
          tokens.push_back(token);
        } else if (isAIR(*p)) {
          ++p;
        } else {
          throw Util::Exception("UnknownToken");
        }
      }

      return tokens;
    }
  }
}
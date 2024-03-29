#pragma once

#include "util.hh"

namespace Core {
  namespace Lexer {
    enum class Type { Id, Int, Char, Float, String, Symbol };
    
    struct Token {
      Type type;
      const char *start;
      const char *end;
    };

    const char *toa(Token token) {
      auto value = new char[token.end - token.start + 1];

      auto p0 = token.start;
      auto p1 = value;

      while (p0 <= token.end) {
        *p1++ = *p0++;
      }

      *p1 = '\0';

      return value;
    }

    int toi(Token token) {
      int value = 0;

      auto p = token.start;

      while (p <= token.end) {
        value = value * 10 + (*p++ - '0');
      }

      return value;
    }

    float tof(Token token) {
      float value = .0;
      float depth = .0;

      auto p = token.start;

      while (p <= token.end) {
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

    bool isAIR(char c) {
      return c == ' ' || c == '\n' || c == '\r' || '\t';
    };

    bool isNUM(char c) {
      return c >= '0' && c <= '9';
    };

    bool isVAL(char c) {
      return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '$' || c == '_';
    };
    
    bool isSYM(char c) {
      static auto SYMBOLS = "!#%&()*+,-./:;<=>?@[]^{|}~";

      auto p = SYMBOLS;
      
      while (*p) {
        if (*p++ == c) {
          return true;
        }
      }

      return false;
    };

    Util::LinkedList<Token> lex(const char *source) {
      Util::LinkedList<Token> tokens;

      auto p = source;

      while (*p) {
        if (isNUM(*p)) {
          Token token { Type::Int, p++, nullptr };

          while (true) {
            if (*p == '.') {
              if (token.type == Type::Float) {
                throw Util::Exception("DotAlreadyUsed");
              } else {
                token.type = Type::Float;
              }
            } else if (!isNUM(*p)) {
              break;
            }

            ++p;
          }
          
          if (*(p - 1) == '.') {
            throw Util::Exception("DotNotAllowed");
          }

          token.end = p - 1;

          tokens.push(token);
        } else if (isVAL(*p)) {
          Token token { Type::Id, p++, nullptr };

          while (isVAL(*p) || isNUM(*p)) {
            ++p;
          }

          token.end = p - 1;

          tokens.push(token);
        } else if (*p == '\'') {
          Token token { Type::Char, ++p, nullptr };
          
          if (*p == '\\') {
            ++p;
          }

          token.end = p - 1;

          if (*++p != '\'') {
            throw Util::Exception("CharNotClosed");
          }

          tokens.push(token);
        } else if (*p == '"') {
          Token token { Type::String, ++p, nullptr };
          
          while (*p && *p != '"' && *p != '\n') {
            if (*p++ == '\\') {
              ++p;
            }
          }

          if (*p != '"') {
            throw Util::Exception("StringNotClosed");
          }

          token.end = p++ - 1;

          tokens.push(token);
        } else if (isSYM(*p)) {
          if (*p == '/' && (*p + 1) == '/') {
            p += 2;

            while (*p && *p != '\n') {
              ++p;
            }

            continue;
          }

          tokens.push({ Type::Symbol, p, p++ });
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
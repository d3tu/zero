#pragma once

#include "util.hh"

namespace DJ {
  namespace Lexer {
    enum class Type { Identifier, Integer, Decimal, String, Char, Symbol };

    struct Token {
      Type type;
      const char *start;
      const char *end;
    };

    const char *toa(Token token) {
      auto value = new char[token.end - token.start + 1];
      auto p0 = token.start;
      auto p1 = value;
      while (p0 <= token.end) *p1++ = *p0++;
      *p1 = '\0';
      return value;
    }

    int toi(Token token) {
      int value = 0;
      auto p = token.start;
      while (p <= token.end) value = value * 10 + (*p++ - '0');
      return value;
    }

    float tof(Token token) {
      float value = .0, depth = .0;
      auto p = token.start;
      while (p <= token.end) {
        if (depth > .0) value += (*p - '0') / (depth *= 10.);
        else if (*p == '.') depth = 1.;
        else value = value * 10. + (*p - '0');
        ++p;
      }
      return value;
    }

    Util::LinkedList<Token> tokenize(const char *source) {
      Util::LinkedList<Token> tokens;

      static auto isAIR = [](char c) {
        return c == ' ' || c == '\n' || c == '\r' || '\t';
      };

      static auto isNUM = [](char c) {
        return c >= '0' && c <= '9';
      };

      static auto isVAL = [](char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '$' || c == '_';
      };

      static auto isSYM = [](char c) {
        static const char *SYMBOLS = "!#%&()*+,-./:;<=>?@[]^{|}~";
        auto p = SYMBOLS;
        while (*p != '\0') if (*p++ == c) return true;
        return false;
      };

      const char *p = source;

      while (*p != '\0') if (isNUM(*p)) {
        Token token { Type::Integer, p++, nullptr };
        bool flag = false;

        while (true) {
          if (*p == '.') {
            if (flag) throw Util::Exception("DotAlreadyUsed");
            flag = true;
            token.type = Type::Decimal;
          } else if (!isNUM(*p)) break;
          ++p;
        }

        token.end = p - 1;
        tokens.push_back(token);
      } else if (isVAL(*p)) {
        Token token { Type::Identifier, p++, nullptr };
        while (isVAL(*p) || isNUM(*p)) ++p;
        token.end = p - 1;
        tokens.push_back(token);
      } else if (*p == '"') {
        Token token { Type::String, ++p, nullptr };
        while (*p && *p != '"' && *p != '\n') if (*p++ == '\\') ++p;
        if (*p != '"') throw Util::Exception("StringNotClosed");
        token.end = p++ - 1;
        tokens.push_back(token);
      } else if (*p == '\'') {
        Token token { Type::Char, ++p, nullptr };
        if (*p == '\\') ++p;
        token.end = p - 1;
        if (*++p != '\'') throw Util::Exception("CharNotClosed");
        tokens.push_back(token);
      } else if (isSYM(*p)) tokens.push_back({ Type::Symbol, p, p++ });
      else if (isAIR(*p)) ++p;
      else throw Util::Exception("UnknownToken");

      return tokens;
    }
  }
}
#pragma once

#include "util.hh"
#include "parser.hh"
#include "runtime.hh"

namespace Core {
  namespace Generator {
    bool cmp(const char *a, const char *b) {
      auto p0 = a;
      auto p1 = b;

      while (*p0 && (*p0 == *p1)) {
        ++p0;
        ++p1;
      }

      return *p0 == *p1;
    }
    
    void writeInt(char *src, int value) {
      *src = (value >> 24) & 0xff;
      *(src + 1) = (value >> 16) & 0xff;
      *(src + 2) = (value >> 8) & 0xff;
      *(src + 3) = value & 0xff;
    }

    struct Buffer {
      const char *data;
      int size;
    };

    Buffer gen(Util::LinkedList<Parser::Stmt *> nodes) {
      using namespace Util;
      using namespace Parser;
      using namespace Runtime;
      
      auto it = nodes.begin();
      
      int size = 0;

      Stack<void *> locals;
      Stack<void *> backup;
      Stack<bool> values;

      #define LOCALS locals
      #define BACKUP backup
      #define VALUES values

      struct _Label {
        const char *name;
        int pos;
      };

      struct _Out {
        char data;
        int pos;
      };

      Stack<_Out> out;

      LinkedList<_Label> labels;
      LinkedList<_Label> waiting_labels;

      char *data;

      each: {
        if (!!it) {
          CALL(genStmt)
          goto each;
        }
      }

      data = new char[size];
      
      while (!out.empty()) {
        auto o = out.pop();
        data[o.pos] = o.data;
      }

      for (auto label : labels) {
        for (auto waiting_label : waiting_labels) {
          if (cmp(label.name, waiting_label.name)) {
            writeInt(data + waiting_label.pos, label.pos);
          }
        }
      }

      return { data, size };

      genStmt: {
        CALL(genLabel)
        if (POP) {
          PUSH(true)
          RET
        }

        CALL(genGoto)
        if (POP) {
          PUSH(true)
          RET
        }

        CALL(genReturn)
        if (POP) {
          PUSH(true)
          RET
        }

        PUSH(false)
        RET
      }
      
      genIf: {
        PUSH(false)
        RET
      }
      
      genVar: {
        PUSH(false)
        RET
      }
      
      genLabel: {
        auto stmt = (Label *) **it;

        if (!stmt) {
          PUSH(false)
          RET
        }

        labels.push({ stmt->name, size });

        ++it;

        PUSH(true)
        RET
      }
      
      genMethod: {
        auto stmt = (Method *) **it;

        if (!stmt) {
          PUSH(false)
          RET
        }

        ++it;

        stmt->name;

        labels.push({ stmt->name, size++ });

        PUSH(false)
        RET
      }
      
      genWhile: {
        auto stmt = (For *) **it;

        if (!stmt) {
          PUSH(false)
          RET
        }

        

        PUSH(false)
        RET
      }
      
      genAssign: {
        PUSH(false)
        RET
      }
      
      genExpr: {
        PUSH(false)
        RET
      }
      
      genCall: {
        PUSH(false)
        RET
      }
      
      genReturn: {
        auto stmt = (Return *) **it;

        if (!stmt) {
          PUSH(false)
          RET
        }

        out.push({ HLT, size++ });

        ++it;

        PUSH(true)
        RET
      }

      genGoto: {
        auto stmt = (Goto *) **it;

        if (!stmt) {
          PUSH(false)
          RET
        }

        out.push({ JMP, size++ });
        waiting_labels.push({ stmt->name, size });

        size += sizeof(int);

        ++it;

        PUSH(true)
        RET
      }
      
      genContinue: {
        PUSH(false)
        RET
      }
      
      genBreak: {
        PUSH(false)
        RET
      }
      
      genValue: {
        PUSH(false)
        RET
      }
    }
  }
}
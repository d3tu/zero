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

    const char *toa(int n) {
      int temp = n;
      int size = 0;

      while (temp > 0) {
        ++size;
        temp /= 10;
      }

      auto res = new char[size + 1];

      for (int i = size - 1; i >= 0; i--) {
        res[i] = n % 10 + '0';
        n /= 10;
      }
      
      res[size] = '\0';

      return res;
    }

    struct Buffer {
      char *data;
      int size;

      operator char *() const {
        return data;
      }
    };

    Buffer gen(Util::LinkedList<Parser::Stmt *> nodes) {
      using namespace Util;
      using namespace Parser;
      using namespace Runtime;
      
      auto it = nodes.begin();
      
      int size = 0;
      int idCounter = 0;

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

      while (true) {
        CALL(genStmt)
        if (!POP) break;
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

        CALL(genContinue)
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

      // cmp expr, true
      // jne _if_id_next
      // ; ...
      // _if_id_next:
      // ; ...
      
      genIf: {
        PUSH(false)
        RET
      }
      
      genVar: {
        PUSH(false)
        RET
      }

      // name:
      // ; ...
      
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

      // name:
      // ; ...
      // ret
      
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

      // _for_id_start:
      // ; ...
      // jmp _for_id_start

      // _for_id_start:
      // cmp expr, true
      // jne _for_id_end
      // ; ...
      // jmp _for_id_start
      // _for_id_end:
      // ; ...
      
      genFor: {
        auto stmt = (For *) **it;

        if (!stmt) {
          PUSH(false)
          RET
        }

        SAVE(&it);

        it = stmt->scope.begin();

        while (true) {
          CALL(genStmt)
          if (!POP) break;
        }

        RESTORE(it);

        PUSH(false)
        RET
      }

      // mov name, value
      
      genAssign: {
        PUSH(false)
        RET
      }
      
      genExpr: {
        PUSH(false)
        RET
      }

      // push value
      // call name
      
      genCall: {
        PUSH(false)
        RET
      }
      
      // push value
      // ret
      
      genReturn: {
        auto stmt = (Return *) **it;

        if (!stmt) {
          PUSH(false)
          RET
        }

        out.push({ INSTR_HLT, size++ });

        ++it;

        PUSH(true)
        RET
      }

      // jmp name

      // jmp _stmt_id_abc

      genContinue: {
        auto stmt = (Continue *) **it;
        
        if (!stmt) {
          PUSH(false)
          RET
        }
        
        if (stmt->expr) {
          auto name = (Value *) *stmt->expr;
          
          out.push({ INSTR_JMP_M, size++ });
          waiting_labels.push({ name->value, size });

          size += sizeof(int);

          ++it;

          PUSH(true)
          RET
        } else {
          ++it;
        }

        PUSH(false)
        RET
      }

      // jmp _stmt_id_end
      
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
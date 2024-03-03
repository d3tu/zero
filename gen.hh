#pragma once

#include "util.hh"
#include "parser.hh"
#include "vm.hh"

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
    
    int toi(const char *str) {
      int value = 0;

      auto p = str;

      while (*p) {
        value = value * 10 + (*p++ - '0');
      }

      return value;
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

    auto strlen(const char *str) {
      auto p = str;
      while (*p) ++p;
      return p - str;
    }

    const char *concat(const char *a, const char *b) {
      auto value = new char[strlen(a) + strlen(b) + 1];

      auto p0 = value;
      auto p1 = a;
      auto p2 = b;

      while (*p1) *p0++ = *p1++;
      while (*p2) *p0++ = *p2++;
      
      *p0 = '\0';

      return value;
    }

    struct Buffer {
      const char *data;
      int size;

      operator const char *() const {
        return data;
      }
    };

    Buffer gen(Util::LinkedList<Parser::Stmt *> nodes) {
      using namespace Util;
      using namespace Parser;
      using namespace Runtime;
      
      int id = 0;

      auto genId = [&](const char *name = "l") {
        return concat(name, toa(id));
      };
      
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

      // auto _genCMP = [&](Expr *stmt, int pos) {
      //   out.push({ INSTR_CMP_R_I, size++ });
      //   out.push({ 0x01, size++ });
      // };

      // if 1 == 2 { ... }
      //
      // MOV R, 1
      // CMP R, 2
      // JNE END
      // END:

      // 0x02 0x01 0x00 0x00 0x00 0x01 0x25 0x01 0x00 0x00 0x00 0x02 0x2c 0xff

      // instr = 1 byte, reg = 1 byte, imm = 4 bytes

      genIf: {
        auto stmt = (If *) **it;

        if (!stmt) {
          PUSH(false)
          RET
        }

        auto expr = (Expr *) *stmt->expr;
        
        int value;

        out.push({ INSTR_MOV_R_I, size++ });
        out.push({ 0x01, size++ });

        value = toi(((Value *) *(expr->left))->value);
        out.push({ (char) ((value >> 24) & 0xff), size++ });
        out.push({ (char) ((value >> 16) & 0xff), size++ });
        out.push({ (char) ((value >> 8) & 0xff), size++ });
        out.push({ (char) (value & 0xff), size++ });
        
        out.push({ INSTR_CMP_R_I, size++ });
        out.push({ 0x01, size++ });

        value = toi(((Value *) *(expr->right))->value);
        out.push({ (char) ((value >> 24) & 0xff), size++ });
        out.push({ (char) ((value >> 16) & 0xff), size++ });
        out.push({ (char) ((value >> 8) & 0xff), size++ });
        out.push({ (char) (value & 0xff), size++ });

        auto _end = genId("end");

        switch (expr->op) {
          case Expr::E: {
            out.push({ INSTR_JNE_M, size++ });
            waiting_labels.push({ _end, size });
            size += sizeof(int);
            break;
          }
        }

        SAVE(&it);

        it = stmt->scope.begin();

        while (true) {
          CALL(genStmt)
          if (!POP) break;
        }

        RESTORE(it);

        labels.push({ _end, size });
        
        ++it;

        PUSH(true)
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

        labels.push({ concat("_", stmt->name), size });

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

        labels.push({ concat("_", stmt->name), size++ });
        
        ++it;

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

        ++it;

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
        }

        ++it;

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
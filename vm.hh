#pragma once

#include "util.hh"

namespace Core {
  namespace Runtime {
    enum Instr {
      INSTR_MOV_R_R,
      INSTR_MOV_R_M,
      INSTR_MOV_R_I,
      INSTR_MOV_M_R,
      INSTR_MOV_M_I,
      
      INSTR_ADD_R_R,
      INSTR_ADD_R_M,
      INSTR_ADD_R_I,

      INSTR_SUB_R_R,
      INSTR_SUB_R_M,
      INSTR_SUB_R_I,

      INSTR_MUL_R_R,
      INSTR_MUL_R_M,
      INSTR_MUL_R_I,

      INSTR_DIV_R_R,
      INSTR_DIV_R_M,
      INSTR_DIV_R_I,
      
      INSTR_INC_R,

      INSTR_DEC_R,
      
      INSTR_AND_R_R,
      INSTR_AND_R_M,
      INSTR_AND_R_I,

      INSTR_OR_R_R,
      INSTR_OR_R_M,
      INSTR_OR_R_I,

      INSTR_XOR_R_R,
      INSTR_XOR_R_M,
      INSTR_XOR_R_I,

      INSTR_NOT_R,

      INSTR_SHL_R_R,
      INSTR_SHL_R_M,
      INSTR_SHL_R_I,
      
      INSTR_SHR_R_R,
      INSTR_SHR_R_M,
      INSTR_SHR_R_I,

      INSTR_CMP_R_R,
      INSTR_CMP_R_M,
      INSTR_CMP_R_I,
      
      INSTR_JMP_M,

      INSTR_JC_M,
      INSTR_JNC_M,

      INSTR_JZ_M,
      INSTR_JNZ_M,

      INSTR_JE_M,
      INSTR_JNE_M,

      INSTR_JL_M,
      INSTR_JLE_M,

      INSTR_JG_M,
      INSTR_JGE_M,

      INSTR_CALL_M,

      INSTR_RET,
      
      INSTR_PUSH_R,
      INSTR_PUSH_M,
      INSTR_PUSH_I,

      INSTR_POP_R,

      INSTR_HLT
    };

    int readInt(const char *bytes) {
      return (*bytes << 24) | (*(bytes + 1) << 16) | (*(bytes + 2) << 8) | (*(bytes + 3));
    }

    void exec(const char *bytes) {
      auto p = bytes;

      p = bytes;
      
      bool zf = false;
      bool cf = false;

      auto regs = new int[256];
      auto stack = new int[1024];

      each: switch (*p) {
        case INSTR_HLT: {
          auto reg0 = regs[0];
          return;
        }
        
        case INSTR_PUSH_R: {
          *stack++ = regs[*++p];

          ++p;
          
          goto each;
        }

        case INSTR_POP_R: {
          regs[*++p] = *--stack;

          ++p;
          
          goto each;
        }

        case INSTR_ADD_R_R: {
          auto regA = *++p;
          auto regB = *++p;

          ++p;

          regs[regA] = regs[regA] + regs[regB];

          goto each;
        }

        case INSTR_SUB_R_I: {
          auto reg = *++p;
          auto imm = readInt(++p);

          p += sizeof(int);

          regs[reg] = regs[reg] - imm;
          
          goto each;
        }

        case INSTR_JZ_M:
        case INSTR_JE_M: {
          if (zf) {
            auto mem = readInt(++p);

            p = bytes + mem;
          } else {
            p += sizeof(int) + 1;
          }
          
          goto each;
        }

        case INSTR_JNZ_M:
        case INSTR_JNE_M: {
          if (!zf) {
            auto mem = readInt(++p);

            p = bytes + mem;
          } else {
            p += sizeof(int) + 1;
          }
          
          goto each;
        }

        case INSTR_JG_M: {
          if (!zf && !cf) {
            auto mem = readInt(++p);

            p = bytes + mem;
          } else {
            p += sizeof(int) + 1;
          }
          
          goto each;
        }

        case INSTR_JL_M: {
          if (cf) {
            auto mem = readInt(++p);

            p = bytes + mem;
          } else {
            p += sizeof(int) + 1;
          }
          
          goto each;
        }

        case INSTR_JGE_M: {
          if (!cf) {
            auto mem = readInt(++p);

            p = bytes + mem;
          } else {
            p += sizeof(int) + 1;
          }
          
          goto each;
        }

        case INSTR_JLE_M: {
          if (zf && cf) {
            auto mem = readInt(++p);

            p = bytes + mem;
          } else {
            p += sizeof(int) + 1;
          }
          
          goto each;
        }

        case INSTR_CMP_R_I: {
          auto reg = *++p;
          auto imm = readInt(++p);

          p += sizeof(int);
          
          zf = regs[reg] == imm;
          cf = regs[reg] < imm;
          
          goto each;
        }

        case INSTR_JMP_M: {
          auto mem = readInt(++p);
          
          p = bytes + mem;
          
          goto each;
        }

        case INSTR_MOV_R_I: {
          auto reg = *++p;
          auto imm = readInt(++p);

          p += sizeof(int);
          
          regs[reg] = imm;
          
          goto each;
        }

        case INSTR_CALL_M: {
          auto mem = readInt(++p);

          *stack++ = p - bytes + sizeof(int);

          p = bytes + mem;

          goto each;
        }
        
        case INSTR_RET: {
          p = bytes + *--stack;

          goto each;
        }

        default:
          throw Util::Exception("UnknownInstruction");
      }
    }
  }
}
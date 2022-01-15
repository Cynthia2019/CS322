#pragma once

#include <vector>
#include <string>

using namespace std;

namespace L1 {

  enum Register {rdi, rax, rbx, rbp, r10, r11, r12, r13, r14, r15, rsi, rdx, rcx, r8, r9, rsp};
  class Item {
    public:
      string labelName;
      int64_t num; 
      Register r;
      bool isARegister;
      bool isAConstant; 
  };

  /*
   * Instruction interface.
   */
  class Instruction{
  };

  /*
   * Instructions.
   */
  class Instruction_ret : public Instruction{
  };

  class Instruction_assignment : public Instruction{
    public:
      Item src,dst;
  };
  
  //load instruction
  class Instruction_load : public Instruction{
    public: 
    Item src; 
    Item dst; 
    Item constant;
  };
  
  class Instruction_shift : public Instruction{
    public: 
    Item src; 
    Item dst; 
    Item constant;
  };


  //store instruction
  class Instruction_store : public Instruction{
    public: 
    Item src; 
    Item dst; 
    Item constant;
  };

  /*
   * Function.
   */
  class Function{
    public:
      std::string name;
      int64_t arguments;
      int64_t locals;
      std::vector<Instruction *> instructions;
  };

  /*
   * Program.
   */
  class Program{
    public:
      std::string entryPointLabel;
      std::vector<Function *> functions;
  };

}

#pragma once

#include <vector>
#include <string>

using namespace std;

namespace L1 {

  enum Register {rdi, rax, rbx, rbp, r10, r11, r12, r13, r14, r15, rsi, rdx, rcx, r8, r9, rsp};
  class Item {
    public:
      string labelName;
      string op; 
      int64_t num; 
      Register r;
      bool isARegister = false;
      bool isAConstant = false; 
      bool isAnOp = false; 
      bool isALabel = false;
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
  
  //shift instruction
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

  //aop instruction 
  class Instruction_aop : public Instruction {
    public: 
    Item src; 
    Item dst; 
    Item op; 
  };

  //store aop instruction 
  class Instruction_store_aop : public Instruction {
    public: 
    Item src; 
    Item constant; 
    Item dst; 
    Item op; 
  };

  //load aop instruction 
  class Instruction_load_aop : public Instruction {
    public: 
    Item src; 
    Item constant; 
    Item dst; 
    Item op; 
  };

  //call u N instruction 
  class Instruction_call : public Instruction {
    public: 
    Item constant; 
    Item label; 
    Item dst; 
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

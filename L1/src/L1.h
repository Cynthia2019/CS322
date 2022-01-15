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
      std::string tostring() {
        return "";
      }
  };

  /*
   * Instruction interface.
   */
  class Instruction{
    public: 
    string instructionName; 
    virtual std::string tostring() { return ""; };
  };

  /*
   * Instructions.
   */
  class Instruction_ret : public Instruction{
    public:
    std::string tostring() {
      return "return";
    }
  };

  class Instruction_assignment : public Instruction{
    public:
      Item src,dst;
    std::string tostring() {
      return "";
    }
  };
  
  //load instruction
  class Instruction_load : public Instruction{
    public: 
    Item src; 
    Item dst; 
    Item constant;
    std::string tostring() {
      return "";
    }
  };
  
  //shift instruction
  class Instruction_shift : public Instruction{
    public: 
    Item dst; 
    Item op;
    Item src; 
    std::string tostring() {
      return dst.tostring() + " " + src.tostring();
    }
  };


  //store instruction
  class Instruction_store : public Instruction{
    public: 
    Item src; 
    Item dst; 
    Item constant;
    std::string tostring() {
      return "";
    }
  };

  //aop instruction 
  class Instruction_aop : public Instruction {
    public: 
    Item src; 
    Item dst; 
    Item op; 
    std::string tostring() {
      return "";
    }
  };

  //store aop instruction 
  class Instruction_store_aop : public Instruction {
    public: 
    Item src; 
    Item constant; 
    Item dst; 
    Item op; 
    std::string tostring() {
      return "";
    }
  };

  //load aop instruction 
  class Instruction_load_aop : public Instruction {
    public: 
    Item src; 
    Item constant; 
    Item dst; 
    Item op; 
    std::string tostring() {
      return "";
    }
  };

  class Instruction_compare: public Instruction {
    public:
    Item dst;
    Item oprand1;
    Item op;
    Item oprand2;
    std::string tostring() {
      return "";
    }
  };

  class Instruction_cjump: public Instruction {
    public:
    Item oprand1;
    Item op;
    Item oprand2;
    Item label;
    std::string tostring() {
      return "";
    }
  };

  //call u N instruction 
  class Instruction_call : public Instruction {
    public: 
    Item constant; 
    Item label; 
    Item dst; 
    std::string tostring() {
      return "";
    }
  }; 

  class Instruction_call_print : public Instruction {
    std::string tostring() {
      return "";
    }
  };
  class Instruction_call_input : public Instruction {
    std::string tostring() {
      return "";
    }
  };
  class Instruction_call_allocate : public Instruction {
    std::string tostring() {
      return "";
    }
  };
  class Instruction_call_error : public Instruction {
    public: 
    Item constant; 
    std::string tostring() {
      return "";
    }
  };

  /*
  misc instruction 
  */
  class Instruction_increment : public Instruction {}; 
  class Instruction_decrement : public Instruction {}; 
  class Instruction_at : public Instruction {
    public: 
    Item constant; 
    Item src_mult; 
    Item src_add; 
    Item dst; 
  };

  //goto instruction 
  class Instruction_goto : public Instruction {
    public:
    Item label; 
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

#pragma once

#include <vector>
#include <string>

using namespace std;

namespace L2
{

  class Spiller;

  enum Register
  {
    rdi,rax,rbx,rbp,
    r10,r11,r12,r13,
    r14,r15,rsi,rdx,
    rcx,r8,r9,rsp
  };

  enum ItemType
  {
    item_op,
    item_label,
    item_number,
    item_register,
    item_variable
  };

  class Item
  {
  public:
    // virtual std::string tostring() = 0;
    virtual std::string get_content() = 0;
    virtual ItemType get_type() = 0;
  };

  class Item_op : public Item
  {
  public:
    std::string op;
    std::string get_content()
    {
      return op;
    }
    ItemType get_type()
    {
      return item_op;
    }
  };

  class Item_register : public Item
  {
  public:
    std::string register_name;
    std::string get_content()
    {
      return register_name;
    }

    ItemType get_type()
    {
      return item_register;
    }
  };

  class Item_number : public Item
  {
  public:
    int64_t num;
    std::string get_content()
    {
      return to_string(num);
    }
    ItemType get_type()
    {
      return item_number;
    }
    int64_t get_nb()
    {
      return num;
    }
  };

  class Item_label : public Item
  {
  public:
    std::string labelName;
    std::string get_content()
    {
      return labelName;
    }
    ItemType get_type()
    {
      return item_label;
    }
  };

  class Item_variable : public Item
  {
  public:
    std::string variable_name;
    std::string get_content()
    {
      return variable_name;
    }
    ItemType get_type()
    {
      return item_variable;
    }
  };

  /*
   * Instruction interface.
   */
  // enum InstructionType {inst_return, inst_assign, inst_load, inst_shift, inst_store,
  //     inst_aop, inst_store_aop, inst_load_aop, inst_comare, inst_cjump, inst_call,
  //     inst_call_print, inst_call_input, inst_call_allocate, inst_call_error, inst_label,
  //     inst_increment, inst_decrement, inst_at, inst_goto};
  class Instruction
  {
  public:
    // std::string instructionName;
    virtual std::string tostring() { return ""; };
    // virtual std::string toL1() = 0;
    virtual vector<Item *> get_gen_set() = 0;
    virtual vector<Item *> get_kill_set() = 0;
    virtual void spill(Spiller &s) = 0;
  };

  /*
   * Instructions.
   */
  class Instruction_ret : public Instruction
  {
    public:
      vector<Item *> get_gen_set();
      vector<Item *> get_kill_set();
      std::string tostring() { return "return"; };
      void spill(Spiller &s) override;
  };

  class Instruction_assignment : public Instruction
  {
  public:
    Item *src, *dst;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return dst->get_content() + " <- " + src->get_content(); }
    void spill(Spiller &s) override;
  };

  // load instruction
  class Instruction_load : public Instruction
  {
  public:
    Item *src;
    Item *dst;
    Item *constant;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { 
      return dst->get_content() + " <- mem "
          + dst->get_content() + " " + constant->get_content();
    }
    void spill(Spiller &s) override;
  };

  // shift instruction
  class Instruction_shift : public Instruction
  {
  public:
    Item *dst;
    Item *op;
    Item *src;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return dst->get_content() +" "+ op->get_content() + " " + src->get_content(); }
    void spill(Spiller &s) override;
  };

  // store instruction
  class Instruction_store : public Instruction
  {
  public:
    Item *src;
    Item *dst;
    Item *constant;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();

    std::string tostring() { return "mem " + dst->get_content() + " " + constant->get_content() + " <- " + src->get_content(); }
  
    void spill(Spiller &s) override;
  };

  // stack arg instruction
  class Instruction_stack : public Instruction
  {
  public:
    Item *dst;
    Item *src;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return dst->get_content() + " stack-arg " + src->get_content(); }
    void spill(Spiller &s) override;
  };

  // aop instruction
  class Instruction_aop : public Instruction
  {
  public:
    Item *src;
    Item *dst;
    Item *op;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return dst->get_content() + op->get_content() + src->get_content(); }
    void spill(Spiller &s) override;
  };

  // store aop instruction
  class Instruction_store_aop : public Instruction
  {
  public:
    Item *src;
    Item *constant;
    Item *dst;
    Item *op;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return dst->get_content() + constant->get_content() + op->get_content() + src->get_content(); }
    void spill(Spiller &s) override;
  };

  // load aop instruction
  class Instruction_load_aop : public Instruction
  {
  public:
    Item *src;
    Item *constant;
    Item *dst;
    Item *op;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return dst->get_content() + op->get_content() + src->get_content() + constant->get_content(); }
    void spill(Spiller &s) override;
  };

  class Instruction_compare : public Instruction
  {
  public:
    Item *dst;
    Item *oprand1;
    Item *op;
    Item *oprand2;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return dst->get_content() + oprand1->get_content() + op->get_content() + oprand2->get_content(); }
    void spill(Spiller &s) override;
  };

  class Instruction_cjump : public Instruction
  {
  public:
    Item *oprand1;
    Item *op;
    Item *oprand2;
    Item *label;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return "cjump " + oprand1->get_content() + op->get_content() + oprand2->get_content() + label->get_content(); }
    void spill(Spiller &s) override;
  };

  // call u N instruction
  class Instruction_call : public Instruction
  {
  public:
    Item *constant;
    Item *dst;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return "call " + dst->get_content() +" "+ constant->get_content(); }
    void spill(Spiller &s) override;
  };

  class Instruction_call_print : public Instruction
  {
    public:
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return "call print 1"; }
    void spill(Spiller &s) override;
  };
  class Instruction_call_input : public Instruction
  {
    public:
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return "call input 0"; }
    void spill(Spiller &s) override;
  };
  class Instruction_call_allocate : public Instruction
  {
    public:
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return "allocate"; }
    void spill(Spiller &s) override;
  };
  class Instruction_call_error : public Instruction
  {
  public:
    Item *constant;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return "error"; }
    void spill(Spiller &s) override;
  };

  // label instruction
  class Instruction_label : public Instruction
  {
  public:
    Item *label;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return label->get_content(); }
    void spill(Spiller &s) override;
  };

  /*
  misc instruction
  */
  class Instruction_increment : public Instruction
  {
  public:
    Item *src;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return src->get_content() + "++"; }
    void spill(Spiller &s) override;
  };
  class Instruction_decrement : public Instruction
  {
  public:
    Item *src;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return src->get_content() + "--"; }
    void spill(Spiller &s) override;
  };
  class Instruction_at : public Instruction
  {
  public:
    Item *constant;
    Item *src_mult;
    Item *src_add;
    Item *dst;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return dst->get_content() + src_add->get_content() + src_mult->get_content() + constant->get_content(); }
    void spill(Spiller &s) override;
  };

  // goto instruction
  class Instruction_goto : public Instruction
  {
  public:
    Item *label;
    vector<Item *> get_gen_set();
    vector<Item *> get_kill_set();
    std::string tostring() { return "goto " + label->get_content(); }
    void spill(Spiller &s) override;
  };
  /*
   * Function.
   */
  class Function
  {
  public:
    std::string name;
    int64_t arguments;
    std::vector<Instruction *> instructions;
  };

  /*
   * Program.
   */
  class Program
  {
  public:
    std::string entryPointLabel;
    std::vector<Function *> functions;
    std::string spill_prefix;
    std::string spill_variable;
  };

  class Spiller {
    public: 
      virtual void visit(Instruction_ret *i) = 0;
      virtual void visit(Instruction_assignment *i) = 0;
      virtual void visit(Instruction_load *i) = 0;
      virtual void visit(Instruction_shift *i) = 0;
      virtual void visit(Instruction_store *i) = 0;
      virtual void visit(Instruction_stack *i) = 0;
      virtual void visit(Instruction_aop *i) = 0;
      virtual void visit(Instruction_store_aop *i) = 0;
      virtual void visit(Instruction_load_aop *i) = 0;
      virtual void visit(Instruction_compare *i) = 0;
      virtual void visit(Instruction_cjump *i) = 0;
      virtual void visit(Instruction_call *i) = 0;
      virtual void visit(Instruction_call_print *i) = 0;
      virtual void visit(Instruction_call_input *i) = 0;
      virtual void visit(Instruction_call_allocate *i) = 0;
      virtual void visit(Instruction_call_error *i) = 0;
      virtual void visit(Instruction_label *i) = 0;
      virtual void visit(Instruction_increment *i) = 0;
      virtual void visit(Instruction_decrement *i) = 0;
      virtual void visit(Instruction_at *i) = 0;
      virtual void visit(Instruction_goto *i) = 0;
  };

}

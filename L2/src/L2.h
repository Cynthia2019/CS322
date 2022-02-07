#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>

#include <architecture.h>

using namespace std;

namespace L2
{

  class Spiller;

  // enum Operation
  // {
  //   op_nope,op_plus,op_minus,op_mul,
  //   op_and, op_left_shift,op_right_shift,op_at,
  //   op_inc,op_dec
  // };

  // enum CompareOperation
  // {
  //   cmp_nope,cmp_l, cmp_le, cmp_eq, cmp_ge, cmp_g};
  // };

  class Visitor;

  enum ItemType
  {
    item_label,
    item_number,
    item_register,
    item_variable
  };

  //Item = Register, Variable, Number, Label, Memory reference

  class Item
  {
  public:
    virtual string toString(void) = 0;
  };

  class Number : public Item
  {
  public:
    Number(int64_t n); 
    int64_t get (void); 
    bool operator == (const Number &other); 
    std::string toString(void) override; 
  private: 
    int64_t num;
  };

  class Label : public Item
  {
  public:
    Label(string l); 
    string get(); 
    bool operator== (const Label &other); 
    string toString(void) override; 
  private: 
    string labelname;
  };

  class Variable : public Item
  {
  public:
    Variable(string varName); 
    string get (void); 
    bool operator == (const Variable &other); 
    string toString(void) override;
  private: 
    string variableName;
  };

class Register : public Variable
  {
  public:
    Register(Architecture::RegisterID rid); 
    Architecture::RegisterID get (void); 
    bool operator == (const Register &other); 
    std::string toString(void) override;
  private: 
    Architecture::RegisterID rid;
  };
class Operation : public Item
  {
  public:
    Operation(string op); 
    string get (void); 
    // bool operator == (const Operation &other) const; 
    string toString(void) override;
  private: 
    string op;
  };
class Memory : public Item
  {
  public:
    Memory(Variable *s, Number *o); 
    Variable* getStartAddress(); 
    // Item** getStartAddressPtr(); 
    int64_t getOffset(); 
    bool operator == (const Memory &other); 
    string toString(void) override;
  protected: 
    Variable* startAddress; 
    Number* offset; 
  };

class StackArgument : public Memory {
  public: 
    StackArgument(Register* rsp, Number* o);
    string toString(void) override;
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
    virtual vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) = 0;
    virtual vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) = 0;
    virtual void spill(Spiller &s) = 0;

    virtual void accept(Visitor* visitor) = 0; 
    virtual std::string toString() = 0; //for debug
  };

  /*
   * Instructions.
   */
  class Instruction_ret : public Instruction
  {
    public:
      vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
      vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
      std::string toString() override { return "return"; }
      void spill(Spiller &s) override;
      void accept(Visitor *v) override; 
  };

  class Instruction_assignment : public Instruction
  {
  public:
    Item* dst;
    Item* src;
    std::string toString() override { return this->dst->toString() + " <- " + this->src->toString(); }
    void accept(Visitor *v) override; 
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    void spill(Spiller &s) override;
  };

  // load instruction
  class Instruction_load : public Instruction
  {
    // dst <- mem src constant
  public:
    Instruction_load();
    Instruction_load(Memory *, Item *);
    Item* dst;
    Item* src;
    Item* constant;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { 
      return this->dst->toString() + " <- mem "
          + this->src->toString() + " " + this->constant->toString();
    }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  // shift instruction
  class Instruction_shift : public Instruction
  {
  public:
    Item *dst;
    Item *op;
    Item *src;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return dst->toString() +" "+ op->toString() + " " + src->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  // store instruction
  class Instruction_store : public Instruction
  {
    // mem dst constant <- src
  public:
    Instruction_store();
    Instruction_store(Memory *, Item *);
    Item *src;
    Item *dst;
    Item *constant;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;

    std::string toString() override { return "mem " + dst->toString() + " " + constant->toString() + " <- " + src->toString(); }
  
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  // stack arg instruction
  class Instruction_stack : public Instruction
  {
  public:
    Item *dst;
    Item *src;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return dst->toString() + " stack-arg " + src->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  // aop instruction
  class Instruction_aop : public Instruction
  {
  public:
    Item *src;
    Item *dst;
    Item *op;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return dst->toString() + " "+ op->toString() + " " + src->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  // store aop instruction
  class Instruction_store_aop : public Instruction
  {
  public:
    Item *src;
    Item *constant;
    Item *dst;
    Item *op;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return "mem " + dst->toString() + " " + constant->toString() + " " + op->toString() + " " + src->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  // load aop instruction
  class Instruction_load_aop : public Instruction
  {
  public:
    Item *src;
    Item *constant;
    Item *dst;
    Item *op;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return dst->toString() + op->toString() + src->toString() + constant->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  class Instruction_compare : public Instruction
  {
  public:
    Item *dst;
    Item *oprand1;
    Item *op;
    Item *oprand2;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return dst->toString() + oprand1->toString() + op->toString() + oprand2->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  class Instruction_cjump : public Instruction
  {
  public:
    Item *oprand1;
    Item *op;
    Item *oprand2;
    Item *label;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return "cjump " + oprand1->toString() + " " + op->toString()+ " " + oprand2->toString() + " " + label->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  // call u N instruction
  class Instruction_call : public Instruction
  {
  public:
    Item *constant;
    Item *dst;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return "call " + dst->toString() +" "+ constant->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  class Instruction_call_print : public Instruction
  {
    public:
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return "call print 1"; }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };
  class Instruction_call_input : public Instruction
  {
    public:
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return "call input 0"; }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };
  class Instruction_call_allocate : public Instruction
  {
    public:
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return "allocate"; }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };
  class Instruction_call_error : public Instruction
  {
  public:
    Item *constant;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return "error"; }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  // label instruction
  class Instruction_label : public Instruction
  {
  public:
    Item *label;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return label->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  /*
  misc instruction
  */
  class Instruction_increment : public Instruction
  {
  public:
    Item *src;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return src->toString() + "++"; }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };
  class Instruction_decrement : public Instruction
  {
  public:
    Item *src;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return src->toString() + "--"; }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };
  class Instruction_at : public Instruction
  {
  public:
    Item *constant;
    Item *src_mult;
    Item *src_add;
    Item *dst;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return dst->toString() + src_add->toString() + src_mult->toString() + constant->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };

  // goto instruction
  class Instruction_goto : public Instruction
  {
  public:
    Item *label;
    vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
    std::string toString() override { return "goto " + label->toString(); }
    void spill(Spiller &s) override;
    void accept(Visitor *v) override; 
  };
  /*
   * Function.
   */
  class Function
  {
  public:
  std::string name;
    int64_t arguments;
    int64_t locals = 0;
    std::vector<Instruction *> instructions;
    std::map<std::string, Variable*> variables; 
    Variable* newVariable(std::string variableName);
    Function(); 
    void format_function();
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
    Program();
    std::map<Architecture::RegisterID, Register*> registers; 
    Register* getRegister(Architecture::RegisterID rid); 
  };

  class Visitor {
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

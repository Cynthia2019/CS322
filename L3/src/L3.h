#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>

#include <architecture.h>

using namespace std;

namespace L3
{

  class Spiller;

  class Visitor;

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
  
  class String : public Item 
  {
    public: 
    String(string sName);
    string get(); 
    string toString() override; 
    private: 
    string sName;
  }

// class Register : public Variable
//   {
//   public:
//     Register(Architecture::RegisterID rid); 
//     Architecture::RegisterID get (void); 
//     bool operator == (const Register &other); 
//     std::string toString(void) override;
//   private: 
//     Architecture::RegisterID rid;
//   };
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
  class Instruction
  {
  public:
    virtual vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) = 0;
    virtual vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) = 0;
    virtual void spill(Spiller &s) = 0;

    virtual void accept(Visitor* visitor) = 0; 
    virtual std::string toString() = 0; //for debug
    virtual Instruction *clone() = 0;
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
      Instruction_ret *clone() override; 
  };

  class Instruction_assignment : public Instruction
  {
  public:
    Variable* dst;
    Item* src;
    std::string toString() override { return this->dst->toString() + " <- " + this->src->toString(); }
    void accept(Visitor *v) override; 
    Instruction_assignment *clone() override; 
  };

  // load instruction
  class Instruction_load : public Instruction
  {
    // dst <- mem src constant
  public:
    Instruction_load();
    Instruction_load(Memory *, Item *);
    Variable* dst;
    Variable* src;
    std::string toString() override { 
      return this->dst->toString() + " <- load "
          + this->src->toString();
    }
    void accept(Visitor *v) override; 
    Instruction_load *clone() override; 
  };

  // shift instruction
  class Instruction_math : public Instruction
  {
  public:
    Item *dst;
    Item *op;
    Item *oprand1;
    Item *oprand2; 
    std::string toString() override { return dst->toString() +" <- "+ oprand1->toString() + " " + op->toString() + " " + oprand2->toString(); }
    void accept(Visitor *v) override; 
    Instruction_shift *clone() override; 
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
    std::string toString() override { return "store " + dst->toString() + " <- " + src->toString(); }
    void accept(Visitor *v) override; 
    Instruction_store *clone() override; 
  };

//   // stack arg instruction
//   class Instruction_stack : public Instruction
//   {
//   public:
//     Item *dst;
//     Item *src;
//     vector<Item *> get_gen_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
//     vector<Item *> get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  override;
//     std::string toString() override { return dst->toString() + " stack-arg " + src->toString(); }
//     void spill(Spiller &s) override;
//     void accept(Visitor *v) override; 
//     Instruction_stack *clone() override; 
//   };

  class Instruction_compare : public Instruction
  {
  public:
    Variable *dst;
    Item *oprand1;
    Item *op;
    Item *oprand2;
    std::string toString() override { return dst->toString() + " " + oprand1->toString()+ " " + op->toString()+ " " + oprand2->toString(); }
    void accept(Visitor *v) override; 
    Instruction_compare *clone() override; 
  };

  class Instruction_br : public Instruction
  {
  public:
    Label *label;
    std::string toString() override { return "br " + label->toString(); }
    void accept(Visitor *v) override; 
    Instruction_br *clone() override; 
  };

class Instruction_br_t : public Instruction
  {
  public:
    Label *label;
    Item *condition;
    std::string toString() override { return "br " + condition->toString() " " +label->toString(); }
    void accept(Visitor *v) override; 
    Instruction_br_ts *clone() override; 
  };
  // call u N instruction
  class Instruction_call : public Instruction
  {
  public:
    Item *dst;
    vector<Item*> args; 
    std::string toString() override { 
        string s = "call " + dst->toString() + " "; 
        for(Item* i : args) s += i->toString() + " ";
        return s; 
    }
    void accept(Visitor *v) override; 
    Instruction_call *clone() override; 
  };

  // label instruction
  class Instruction_label : public Instruction
  {
  public:
    Label *label;
    std::string toString() override { return label->toString(); }
    void accept(Visitor *v) override; 
    Instruction_label *clone() override; 
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
    Instruction_increment *clone() override; 
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
    Instruction_decrement *clone() override; 
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
    Instruction_at *clone() override; 
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
    Instruction_goto *clone() override; 
  };
  /*
   * Function.
   */
  class Function
  {
  public:
    std::string name;
    int64_t arguments;
//    int64_t locals = 0;
    std::vector<Instruction *> instructions;
    std::map<std::string, Variable*> variables; 
    Variable* newVariable(std::string variableName);
    Function(); 
    Function(const Function &f);
    void format_function();
  };

  /*
   * Program.
   */
  class Program
  {
  public:
  //  std::string entryPointLabel;
    std::vector<Function *> functions;
    std::string spill_prefix;
    std::string spill_variable;
    Program();
  //  std::map<Architecture::RegisterID, Register*> registers; 
  //  Register* getRegister(Architecture::RegisterID rid); 
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

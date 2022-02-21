#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>

using namespace std;

namespace IR
{

  class Visitor;

  enum ItemType {
    item_number,
    item_label,
    item_variable,
    item_string,
    item_operation,
    item_type
  };

  enum VarTypes {
    var_int64, 
    var_int64_multi, 
    var_tuple, 
    var_code
  };

  class Item
  {
  public:
    virtual string toString(void) = 0;
    virtual ItemType getType(void) = 0;
  };

  class Number : public Item
  {
  public:
    Number(int64_t n); 
    int64_t get (void); 
    bool operator == (const Number &other); 
    std::string toString(void) override; 
    ItemType getType(void) override { return item_number; }
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
    ItemType getType(void) override { return item_label; }
  private: 
    string labelname;
  };

  class Variable : public Item
  {
  public:
    Variable(string varName, VarTypes type); 
    string get (void); 
    VarTypes getVariableType(); 
    bool operator == (const Variable &other); 
    string toString(void) override;
    ItemType getType(void) override { return item_variable; }
  protected: 
    string variableName;
    VarTypes type; 
  };
  
  class String : public Item 
  {
    public: 
    String(string sName);
    string get(); 
    string toString() override; 
    ItemType getType(void) override { return item_string; }
    private: 
    string sName;
  };

class Operation : public Item
  {
  public:
    Operation(string op); 
    string get (void); 
    // bool operator == (const Operation &other) const; 
    string toString(void) override;
    ItemType getType(void) override { return item_operation; }
  private: 
    string op;
  };

class ArrayVar : public Variable {
  public:
  ArrayVar(std::string name, int dim);
  int64_t dimension;
};

// class TupleVar : public Variable {
//   public:
//   TupleVar(std::string name);
// };


  /*
   * Instruction interface.
   */
  class Instruction
  {
  public:
    vector<Item *> uses;
    vector<Item *> define;

    virtual void accept(Visitor* visitor) = 0; 
    virtual std::string toString() = 0; //for debug
  };

  class Terminator {
    public:
    Operation* op;
    virtual std::string toString() = 0;
  };


  /*
   * Instructions.
   */
  class Instruction_ret : public Instruction, public Terminator
  {
    public:
      virtual std::string toString() = 0;
      virtual void accept(Visitor *v) = 0;
  };

  class Instruction_ret_not : public Instruction_ret
  {
    public:
      std::string toString() override { return "return"; }
      void accept(Visitor *v) override; 
  };

  class Instruction_ret_t : public Instruction_ret
  {
    public:
      // Operation* op;
      Item* arg; 
      std::string toString() override { return "return " + arg->toString(); }
      void accept(Visitor *v) override; 
  };

  class Instruction_declare : public Instruction 
  {
    public: 
    VarTypes type;
    Variable* dst; 
    std::string toString() override { return "type  " + dst->toString(); }
    void accept(Visitor *v) override;
  };

  class Instruction_assignment : public Instruction
  {
  public:
    Variable* dst;
    Item* src;
    std::string toString() override { return this->dst->toString() + " <- " + this->src->toString(); }
    void accept(Visitor *v) override; 
  };

  // load from index instruction
  class Instruction_load : public Instruction
  {
  public:
    Variable* dst;
    Variable* src;
    vector<Item*> indices; 
    std::string toString() override { 
      string s = this->dst->toString() + " <- " + this->src->toString(); 
      for(Item* i : indices) {
        s += "[" + i->toString() + "]";
      }
      return s;
    }
    void accept(Visitor *v) override; 
  };

  // math instruction
  class Instruction_op : public Instruction
  {
  public:
    Variable *dst;
    Item *op;
    Item *oprand1;
    Item *oprand2; 
    std::string toString() override { return dst->toString() +" <- "+ oprand1->toString() + " " + op->toString() + " " + oprand2->toString(); }
    void accept(Visitor *v) override; 
  };

  // store instruction
  class Instruction_store : public Instruction
  {
  public:
    Item *src;
    Variable *dst;
    vector<Item*> indices; 
    std::string toString() override { 
      string s = this->dst->toString(); 
      for(Item* i : indices) {
        s += "[" + i->toString() + "]";
      }
      s += + " <- " + this->src->toString();
      return s;
    }
    void accept(Visitor *v) override; 
  };

  class Instruction_br : public Instruction, public Terminator {
    public:
    virtual std::string toString() = 0;
    virtual void accept(Visitor *v) = 0;
  };

  class Instruction_br_label : public Instruction_br
  {
  public:
    Label *label;
    std::string toString() override { return "br " + label->toString(); }
    void accept(Visitor *v) override; 
  };

class Instruction_br_t : public Instruction_br
  {
  public:
    Item *condition;
    Label *label1; 
    Label *label2; 
    std::string toString() override { return "br " + condition->toString() + " " +label1->toString() + " " +label2->toString(); }
    void accept(Visitor *v) override; 
  };

  class Instruction_call : public Instruction {
    public:
    Item *callee;
    vector<Item*> args; 
    virtual std::string toString() = 0;
    virtual void accept(Visitor *v) = 0;
  };

  // call callee (args) instruction
  class Instruction_call_noassign : public Instruction_call
  {
  public:
    std::string toString() override { 
        string s = "call " + callee->toString() + " "; 
        for(Item* i : args) s += i->toString() + " ";
        return s; 
    }
    void accept(Visitor *v) override; 
  };
  // var <- call callee (args) instruction
  class Instruction_call_assignment : public Instruction_call
  {
  public:
    Variable *dst;
    std::string toString() override { 
        string s = dst->toString() + " <- call " + callee->toString() + " "; 
        for(Item* i : args) s += i->toString() + " ";
        return s; 
    }
    void accept(Visitor *v) override; 
  };

  // label instruction
  class Instruction_label : public Instruction
  {
  public:
    Label *label;
    std::string toString() override { return label->toString(); }
    void accept(Visitor *v) override; 
  };

  //length instruction 
  class Instruction_length : public Instruction
  {
    public:
    Variable* dst; 
    Variable* src; 
    Item* dimID; 
    std::string toString() override { 
      return dst->toString() + " <- length " + src->toString() + " " + dimID->toString(); 
    }
    void accept(Visitor *v) override; 
  };

  //Array instruction 
  class Instruction_array : public Instruction
  {
    public:
    Variable* dst; 
    vector<Item*> args;
    std::string toString() override { 
      string s = dst->toString() + " <- new Array("; 
      for(Item* item : args){
        s += item->toString() + " "; 
      }
      s += ")";
      return s; 
    }
    void accept(Visitor *v) override; 
  };

  //Tuple instruction 
  class Instruction_tuple : public Instruction
  {
    public:
    Variable* dst; 
    Item* arg; 
    std::string toString() override { 
      string s = dst->toString() + " <- new Tuple(" + arg->toString() + ")"; 
      return s; 
    }
    void accept(Visitor *v) override; 
  };

  class BasicBlock {
    public:
    Label* label; 
    vector<Instruction*> instructions; 
    Terminator* te; 
  };
  /*
   * Function.
   */
  class Function
  {
  public:
    std::string name;
    string type; 
    bool isMain;
    vector<Variable*> arguments; 
    std::vector<BasicBlock*> basicBlocks; 
    std::map<std::string, Variable*> variables; 
    Variable* newVariable(std::string type, VarTypes variableName, int dim);
    Function(); 
  };

  /*
   * Program.
   */
  class Program
  {
  public:
    bool hasMain; 
    std::vector<Function *> functions;
    Program();
  };

  class Visitor {
    public: 
      virtual void visit(Instruction_ret_not *i) = 0;
      virtual void visit(Instruction_ret_t *i) = 0;
      virtual void visit(Instruction_assignment *i) = 0;
      virtual void visit(Instruction_load *i) = 0;
      virtual void visit(Instruction_op *i) = 0;
      virtual void visit(Instruction_store *i) = 0;
      virtual void visit(Instruction_declare *i) = 0;
      virtual void visit(Instruction_br_label *i) = 0;
      virtual void visit(Instruction_br_t *i) = 0;
      virtual void visit(Instruction_call_noassign *i) = 0;
      virtual void visit(Instruction_call_assignment *i) = 0;
      virtual void visit(Instruction_label *i) = 0;
      virtual void visit(Instruction_length *i) = 0;
      virtual void visit(Instruction_array *i) = 0;
      virtual void visit(Instruction_tuple *i) = 0;
  };

}

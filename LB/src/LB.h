#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <iostream>

using namespace std;

namespace LB
{

  class Visitor;

  enum ItemType {
    item_number,
    item_label,
    item_variable,
    item_string,
    item_operation,
    item_type,
    item_function
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
    void setName(string name) {variableName = name;}
  protected: 
    string variableName;
    VarTypes type; 
  };

  class Scope {
    public:
    Scope(Scope *parent);
    std::unordered_map<Variable *, Variable *> rename;
    std::map<std::string, Variable*> variables; 
    Variable *newVariable(std::string variable, VarTypes type, int dim);
    Variable *getVariable(std::string s);
    Scope *parent;
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

class TupleVar: public Variable {
  public:
  TupleVar(std::string name);
};

class FunctionItem : public Item {
  public: 
  FunctionItem(string name); 
    string get(); 
    string toString() override; 
    ItemType getType(void) override { return item_function; }
  private: 
    string fname; 
};

  /*
   * Instruction interface.
   */
  class Instruction
  {
  public:
    Scope *scope;
    vector<Item *> uses;
    vector<Item *> define;
    int64_t lineno = 0;
    virtual void accept(Visitor* visitor) = 0; 
    virtual std::string toString() = 0; //for debugging
  };

  /*
   * Instructions.
   */
  class Instruction_ret :  public Instruction
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
    std::string type_str;
    //Variable* dst; 
    vector<Variable *> declared;
    std::string toString() override { 
      std::string s = type_str + " ";
      for (auto v : declared) {
        s += v->toString() + " ";
      }
      return s;
    }
    void accept(Visitor *v) override;
  };

  class Instruction_assignment : public Instruction
  {
  public:
    Variable* dst;
    Item* src;
    std::string toString() override { return this->dst->toString() + " <- " + this->src->toString(); }
    bool do_not_encode = false;
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
    std::string toString() override { 
      if (!dst) std::cout << "no dst" << std::endl;
      if (!op) cout << "no op" << endl;
      if (!oprand1) cout << "no op1" << endl;
      if (!oprand2) cout << "no op2" << endl;
      return dst->toString() +" <- "+ oprand1->toString() + " " + op->toString() + " " + oprand2->toString(); }
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

  class Instruction_goto : public Instruction {
    public:
    Label *label;
    std::string toString() { return "goto " + label->toString(); };
    void accept(Visitor *v);
  };

  class Condition {
    public:
    Item *oprand1;
    Item *op;
    Item *oprand2;
    std::string toString() {
      return oprand1->toString() + " " + op->toString() + " " + oprand2->toString();
    }
  };

  class Instruction_branch: public Instruction {
    public:
    Label *true_label;
    Label *false_label;
    Condition *condition;
    virtual std::string toString() = 0;
    virtual void accept(Visitor *v) = 0;
  };

  class Instruction_if: public Instruction_branch {
    std::string toString() {
      return "if " +condition->toString() + " " + true_label->toString() + false_label->toString();
    }
    void accept(Visitor *v);
  };

  class Instruction_while : public Instruction_branch {
    public:
    std::string toString() {
      return "while " +condition->toString() + " " + true_label->toString() + false_label->toString();
    }

    void accept(Visitor *v);
  };

  class Instruction_call : public Instruction {
    public:
    Item *callee;
    vector<Item*> args; 
    virtual std::string toString() = 0;
    virtual void accept(Visitor *v) = 0;
  };

  class Instruction_print : public Instruction {
    public: 
    Item * src; 
    std::string toString() override { return "print (" + src->toString() + ")";};
    void accept(Visitor *v) override; 
  };
  class Instruction_input : public Instruction {
    public: 
    std::string toString() override { return "input ()";};
    void accept(Visitor *v) override; 
  };

  class Instruction_continue :public Instruction {
    public:
    std::string toString() override { return "continue";};
    void accept(Visitor *v) override; 
  };

  class Instruction_break: public Instruction {
    public:
    std::string toString() override { return "break";};
    void accept(Visitor *v) override; 
  };

  class Instruction_input_assignment : public Instruction {
    public: 
    Item* dst;
    std::string toString() override { return dst->toString() + " <- input ()";};
    void accept(Visitor *v) override; 
  };
  // call callee (args) instruction
  class Instruction_call_noassign : public Instruction_call
  {
  public:
    std::string toString() override { 
        string s = callee->toString() + "("; 
        for (int i = 0; i < args.size(); i++) {
          s += args[i]->toString();
          if (i != args.size() - 1) {
            s += ", ";
          }
        }
        s += ")";
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
        string s = dst->toString() + " <- " + callee->toString() + "("; 
        for (int i = 0; i < args.size(); i++) {
          s += args[i]->toString();
          if (i != args.size() - 1) {
            s += ", ";
          }
        }
        s += ")";
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

  /*
   * Function.
   */
  class Function
  {
  public:
    std::string name;
    std::string entry_label;
    std::string longest_name;
    string type; 
    bool isMain;
    vector<Variable*> arguments; 
    vector<Instruction*> instructions; 
    std::map<std::string, Variable*> variables; 
    Variable* newVariable(std::string type, VarTypes variableName, int dim);
    Variable* getVariable(std::string name);
    Function(); 
  };

  /*
   * Program.
   */
  class Program
  {
  public:
    bool hasMain; 
    std::map<std::string, Function*> name_to_functions; 
    std::vector<Function *> functions;
    Function* getFunction(string name); 
    std::string getLongestLabel();
    void printProgram(void);
    Program();
  private:
    std::string LL;
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
      virtual void visit(Instruction_while *i) = 0;
      virtual void visit(Instruction_if *i) = 0;
      virtual void visit(Instruction_goto *i) = 0;
      virtual void visit(Instruction_print *i) = 0;
      virtual void visit(Instruction_input *i) = 0;
      virtual void visit(Instruction_break *i) = 0;
      virtual void visit(Instruction_continue *i) = 0;
      virtual void visit(Instruction_input_assignment *i) = 0;
      virtual void visit(Instruction_call_noassign *i) = 0;
      virtual void visit(Instruction_call_assignment *i) = 0;
      virtual void visit(Instruction_label *i) = 0;
      virtual void visit(Instruction_length *i) = 0;
      virtual void visit(Instruction_array *i) = 0;
      virtual void visit(Instruction_tuple *i) = 0;
  };

}

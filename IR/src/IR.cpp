#include "IR.h"
#include <iostream>
#include <string>

using namespace std;

namespace IR
{
  //Program 
  Program::Program() {}
  //Function 
  Function::Function(void) {}

  Variable* Function::newVariable(std::string variable, std::string type){
    // std::string variableName = "%var_" + variable.substr(1);
    // if(Function::variables.find(variableName) != Function::variables.end()){
    //   return Function::variables[variableName];
    // }
    Function::variables[variable] = new Variable(variable, type);
    return Function::variables[variable];
  }

  // Label Item
  Label::Label(string l){
      this->labelname = l;
  } 
  string Label::get()
  {
    return this->labelname;
  }
  bool Label::operator==(const Label &other)
  {
    return *this == other;
  }
  string Label::toString()
  {
    return this->labelname;
  }

  // Number Item
  Number::Number(int64_t n)
  {
    this->num = n;
  }
  int64_t Number::get()
  {
    return this->num;
  }
  bool Number::operator==(const Number &other)
  {
    return *this == other;
  }
  string Number::toString()
  {
    return to_string(this->num);
  }

  // Variable Item
  Variable::Variable(string v, string t)
  {
    this->variableName = v;
    this->type = t; 
  }
  string Variable::get()
  {
    return this->variableName;
  }
  string Variable::getVariableType()
  {
    return this->type;
  }
  bool Variable::operator==(const Variable &other)
  {
    return *this == other;
  }
  string Variable::toString()
  {
    return this->variableName;
  }
  // Operation Item
  Operation::Operation(string op)
  {
    this->op = op;
  }
  string Operation::get()
  {
    return this->op;
  }
  string Operation::toString()
  {
    return this->op;
  }
  String::String(std::string sName){
      this->sName = sName; 
  }
  string String::get() {
      return this->sName;
  }
  string String::toString() {
      return this->sName;
  }

//assignment 
void Instruction_assignment::accept(Visitor* v) {
  v->visit(this);
}

void Instruction_declare::accept(Visitor* v){
  v->visit(this);
}

void Instruction_load::accept(Visitor* v) {
  v->visit(this);
}

void Instruction_store::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_ret_not::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_ret_t::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_op::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_br_label::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_br_t::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_call_noassign::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_call_assignment::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_label::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_length::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_array::accept(Visitor* v) {
  v->visit(this);
}
void Instruction_tuple::accept(Visitor* v) {
  v->visit(this);
}
  }
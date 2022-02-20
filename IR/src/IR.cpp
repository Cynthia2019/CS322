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

  Variable* Function::newVariable(std::string variable, VarTypes type, int dim){
    std::string variableName = variable;
    if(Function::variables.find(variableName) != Function::variables.end()){
      cerr << "multiple definition of " << variable << endl;
      abort();
      //return Function::variables[variableName];
    }

    switch (type)
    {
    case var_int64_multi:
      /* code */
      Function::variables[variableName] = new ArrayVar(variableName, dim);
      break;
    
    default:
      Function::variables[variableName] = new Variable(variableName, type);
      break;
    }
    return Function::variables[variableName];
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
  Variable::Variable(string v, VarTypes t)
  {
    this->variableName = v;
    this->type = t; 
  }
  string Variable::get()
  {
    return this->variableName;
  }
  VarTypes Variable::getVariableType()
  {
    return this->type;
  }
  bool Variable::operator==(const Variable &other)
  {
    return *this == other;
  }

  ArrayVar::ArrayVar(std::string name, int dim) :Variable(name, var_int64_multi){
    this->dimension = dim;
    // this->variableName = name;
    // this->type = var_int64_multi;
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
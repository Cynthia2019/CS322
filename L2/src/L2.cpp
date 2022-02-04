#include "architecture.h"
#include "L2.h"
#include <iostream>
#include <string>

using namespace std;

namespace L2
{


vector<Architecture::RegisterID> argument_regs = {
    Architecture::rdi, 
    Architecture::rsi, 
    Architecture::rdx,
    Architecture::rcx,
    Architecture::r8,
    Architecture::r9 };
vector<Architecture::RegisterID> caller_saved_regs = {
    Architecture::rdi, 
    Architecture::rsi, 
    Architecture::rdx,
    Architecture::rcx,
    Architecture::r8,
    Architecture::r9,
    Architecture::r10,
    Architecture::r11,
    Architecture::rax
      };
vector<Architecture::RegisterID> callee_saved_regs = {
    Architecture::rbp, 
    Architecture::rbx, 
    Architecture::r12,
    Architecture::r13,
    Architecture::r14,
    Architecture::r15
      };
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
  // Register Item
  Register::Register(Architecture::RegisterID r) : Variable(Architecture::fromRegisterToString(r))
  {
    this->rid = r;
  }
  Architecture::RegisterID Register::get()
  {
    return this->rid;
  }
  bool Register::operator==(const Register &other)
  {
    return *this == other;
  }
  string Register::toString()
  {
    return Architecture::fromRegisterToString(this->rid);
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
  Variable::Variable(string v)
  {
    this->variableName = v;
  }
  string Variable::get()
  {
    return this->variableName;
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
  // Memory Item
  Memory::Memory(Variable *s, Number *n)
  {
    this->startAddress = s;
    this->offset = n;
  }
  Variable* Memory::getStartAddress()
  {
    return this->startAddress;
  }
  // Item** Memory::getStartAddressPtr(){
  //   return this->startAddress; 
  // }
  int64_t Memory::getOffset() {
    return this->offset->get(); 
  }
  bool Memory::operator==(const Memory &other)
  {
    if (true && ((*this->offset) == (*other.offset)) && ((*this->startAddress) == (*other.startAddress)))
    {
      return false;
    }
    return false;
  }

string Memory::toString() {
  return this->startAddress->toString() + " " + this->offset->toString();
}
StackArgument::StackArgument(Register* rsp, Number* o) : Memory(rsp, o)
{};
string StackArgument::toString() {
  return this->startAddress->toString() + " " + this->offset->toString();
}
//assignment 
void Instruction_assignment::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_assignment::get_gen_set()
{
  vector<Item *> v;
  Number* i = dynamic_cast<Number*>(src);
  if(i == nullptr) {
    v.push_back(src);
  }
  return v;
}
vector<Item *> Instruction_assignment::get_kill_set()
{
  return {dst};
}
//load 
void Instruction_load::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_load::get_gen_set()
{
  if(src->toString() != "rsp") return {src};
  return {};
}
vector<Item *> Instruction_load::get_kill_set()
{
  return {dst};
}
void Instruction_store::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_store::get_gen_set()
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if (i != nullptr)
  {
    v.push_back(i);
  }
  if(dst->toString() != "rsp") v.push_back(dst); 
  return v;
}

vector<Item *> Instruction_store::get_kill_set()
{
  return {};
}
void Instruction_ret::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_ret::get_gen_set()
{
  vector<Item*> v;
  auto callee = callee_saved_regs; 
  for(auto i : callee){
    Register* r = new Register(i); 
    v.push_back(r); 
  }
  Register* rax = new Register(Architecture::RegisterID::rax);
  v.push_back(rax);
  return v;
}
vector<Item *> Instruction_ret::get_kill_set() { return {}; }

void Instruction_shift::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_shift::get_gen_set()
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if (i != nullptr)
  {
    v.push_back(i);
  }
  v.push_back(dst);
  return v;
}

vector<Item *> Instruction_shift::get_kill_set()
{
  return {dst};
}
void Instruction_aop::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_aop::get_gen_set()
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if (i != nullptr)
  {
    v.push_back(i);
  }
  v.push_back(dst);
  return v;
}

vector<Item *> Instruction_aop::get_kill_set()
{
  return {dst};
}
void Instruction_store_aop::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_store_aop::get_gen_set()
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if (i != nullptr)
  {
    v.push_back(i);
  }
  if(dst->toString() != "rsp") v.push_back(dst); 
  return v;
}

vector<Item *> Instruction_store_aop::get_kill_set()
{
  return {};
}
void Instruction_stack::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_stack::get_gen_set()
{
  return {};
}

vector<Item *> Instruction_stack::get_kill_set()
{
  return {dst};
}
void Instruction_compare::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_compare::get_gen_set()
  {
    vector<Item *> v;
    Variable* op1 = dynamic_cast<Variable*>(oprand1);
    if (op1 != nullptr)
    {
      v.push_back(oprand1);
    }
    Variable* op2 = dynamic_cast<Variable*>(oprand2);
    if (op2 != nullptr)
    {
      v.push_back((oprand2));
    }
    return v;
  }
vector<Item *> Instruction_compare::get_kill_set()
{
  return {dst};
}
void Instruction_cjump::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_cjump::get_gen_set()
{
    vector<Item *> v;
    Variable* op1 = dynamic_cast<Variable*>(oprand1);
    if (op1 != nullptr)
    {
      v.push_back(oprand1);
    }
    Variable* op2 = dynamic_cast<Variable*>(oprand2);
    if (op2 != nullptr)
    {
      v.push_back((oprand2));
    }
    return v;
}
vector<Item *> Instruction_cjump::get_kill_set()
{
  return {};
}
void Instruction_call::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_call::get_gen_set()
{
  vector<Item*> v;
  Number* n = dynamic_cast<Number*>(constant); 
  Variable* i = dynamic_cast<Variable*>(dst); 
  auto args = argument_regs; 
  for(int i = 0; i < n->get(); i++){
    Register* r = new Register(args[i]); 
    v.push_back(r); 
  }
  if(i != nullptr) v.push_back(i);
  return v;
}
vector<Item *> Instruction_call::get_kill_set()
{
  vector<Item *> v;
  for(auto i : caller_saved_regs){
    Register* r = new Register(i); 
    v.push_back(r); 
  }
  return v;
}
void Instruction_call_print::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_call_print::get_gen_set()
{
  Architecture::RegisterID arg = argument_regs[0]; 
  Register* r = new Register(arg); 
  return {r};
}

vector<Item *> Instruction_call_print::get_kill_set()
{
  vector<Item *> v;
  vector<Architecture::RegisterID> caller = caller_saved_regs; 
  for(auto i : caller){
    Register* r = new Register(i); 
    v.push_back(r); 
  }
  return v;
}
void Instruction_call_allocate::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_call_allocate::get_gen_set()
{
  vector<Item *> v;
  vector<Architecture::RegisterID> args = argument_regs; 
  for(int i = 0; i < 2; i++){
    Register* r = new Register(args[i]); 
    v.push_back(r); 
  }
  return v;
}
vector<Item *> Instruction_call_allocate::get_kill_set()
{
  vector<Item *> v;
  auto caller =  caller_saved_regs; 
  for(auto i : caller){
    Register* r = new Register(i); 
    v.push_back(r); 
  }
  return v;
}
void Instruction_call_error::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_call_error::get_gen_set()
{
  vector<Item *> v;
  Number* n = dynamic_cast<Number*>(constant); 
  auto args = argument_regs; 
  for(int i = 0; i < n->get(); i++){
    Register* r = new Register(args[i]); 
    v.push_back(r); 
  }
  return v;
}

vector<Item *> Instruction_call_error::get_kill_set()
{
  vector<Item *> v;
  auto caller = caller_saved_regs; 
  for(auto i : caller){
    Register* r = new Register(i); 
    v.push_back(r); 
  }
  return v;
}
void Instruction_increment::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_increment::get_gen_set()
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if(i != nullptr) v.push_back(i);
  return v;
}

vector<Item *> Instruction_increment::get_kill_set()
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if(i != nullptr) v.push_back(i);
  return v;
}
void Instruction_decrement::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_decrement::get_gen_set()
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if(i != nullptr) v.push_back(i);
  return v;
}

vector<Item *> Instruction_decrement::get_kill_set()
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if(i != nullptr) v.push_back(i);
  return v;
}
void Instruction_at::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_at::get_gen_set()
{
  vector<Item *> v;
  v.push_back(src_mult);
  v.push_back(src_add);
  return v;
}

vector<Item *> Instruction_at::get_kill_set()
{
  return {dst};
}
void Instruction_load_aop::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_load_aop::get_gen_set()
{
  vector<Item *> v;

  Variable* i = dynamic_cast<Variable*>(src);
  if (i != nullptr)
  {
    if(i->toString() != "rsp") v.push_back(src); 
  }
  v.push_back(dst);
  return v;
}
vector<Item *> Instruction_load_aop::get_kill_set()
{
  return {dst};
}
void Instruction_goto::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_goto::get_gen_set()
{
  return {};
}

vector<Item *> Instruction_goto::get_kill_set()
{
  return {};
}
void Instruction_label::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_label::get_gen_set()
{
  return {};
}

vector<Item *> Instruction_label::get_kill_set()
{
  return {};
}


    void Instruction_ret::spill(Spiller & s) { s.visit(this); }
    void Instruction_assignment::spill(Spiller & s) { s.visit(this); }
    void Instruction_load::spill(Spiller & s) { s.visit(this); }
    void Instruction_shift::spill(Spiller & s) { s.visit(this); }
    void Instruction_store::spill(Spiller & s) { s.visit(this); }
    void Instruction_stack::spill(Spiller & s) { s.visit(this); }
    void Instruction_aop::spill(Spiller & s) { s.visit(this); }
    void Instruction_store_aop::spill(Spiller & s) { s.visit(this); }
    void Instruction_load_aop::spill(Spiller & s) { s.visit(this); }
    void Instruction_compare::spill(Spiller & s) { s.visit(this); }
    void Instruction_cjump::spill(Spiller & s) { s.visit(this); }
    void Instruction_call::spill(Spiller & s) { s.visit(this); }
    void Instruction_call_print::spill(Spiller & s) { s.visit(this); }
    void Instruction_call_input::spill(Spiller & s) { s.visit(this); }
    void Instruction_call_allocate::spill(Spiller & s) { s.visit(this); }
    void Instruction_call_error::spill(Spiller & s) { s.visit(this); }
    void Instruction_label::spill(Spiller & s) { s.visit(this); }
    void Instruction_increment::spill(Spiller & s) { s.visit(this); }
    void Instruction_decrement::spill(Spiller & s) { s.visit(this); }
    void Instruction_at::spill(Spiller & s) { s.visit(this); }
    void Instruction_goto::spill(Spiller & s) { s.visit(this); }
  }
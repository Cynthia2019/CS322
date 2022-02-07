#include "architecture.h"
#include "L2.h"
#include <iostream>
#include <string>

using namespace std;

namespace L2
{
  //Program 
  Program::Program(void) {
      this->registers = {
      {L2::Architecture::rax, new L2::Register(L2::Architecture::rax)},
      {L2::Architecture::rbx, new L2::Register(L2::Architecture::rbx)},
      {L2::Architecture::rcx, new L2::Register(L2::Architecture::rcx)},
      {L2::Architecture::rdx, new L2::Register(L2::Architecture::rdx)},
      {L2::Architecture::rdi, new L2::Register(L2::Architecture::rdi)},
      {L2::Architecture::rsi, new L2::Register(L2::Architecture::rsi)},
      {L2::Architecture::rbp, new L2::Register(L2::Architecture::rbp)},
      {L2::Architecture::r8, new L2::Register(L2::Architecture::r8)},
      {L2::Architecture::r9, new L2::Register(L2::Architecture::r9)},
      {L2::Architecture::r10, new L2::Register(L2::Architecture::r10)},
      {L2::Architecture::r11, new L2::Register(L2::Architecture::r11)},
      {L2::Architecture::r12, new L2::Register(L2::Architecture::r12)},
      {L2::Architecture::r13, new L2::Register(L2::Architecture::r13)},
      {L2::Architecture::r14, new L2::Register(L2::Architecture::r14)},
      {L2::Architecture::r15, new L2::Register(L2::Architecture::r15)},
      {L2::Architecture::rsp, new L2::Register(L2::Architecture::rsp)}
    };
  }
  Register* Program::getRegister(Architecture::RegisterID rid){
    return Program::registers[rid];
  }
  //Function 
  Function::Function(void) {}
  Function::Function(const Function &f) {
    this->name = f.name;
    this->arguments = f.arguments;
    this->locals = f.locals;
    for (auto i : f.instructions) {
      Instruction *inst = i->clone();
      this->instructions.push_back(inst);
    }
    this->variables= f.variables;
  }

  Variable* Function::newVariable(std::string variableName){
    if(Function::variables.find(variableName) != Function::variables.end()){
      return Function::variables[variableName];
    }
    Function::variables[variableName] = new Variable(variableName);
    return Function::variables[variableName];
  }

  void Function::format_function() {
    ::cout << "(" << this->name << endl;
    ::cout << "\t" << this->arguments << endl;
    for (auto i : this->instructions) {
      ::cout << "\t" << i->toString() << endl;
    }

    ::cout << ")" << endl;
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
{}
string StackArgument::toString() {
  return this->startAddress->toString() + " " + this->offset->toString();
}

  Instruction_ret * Instruction_ret::clone() { return new Instruction_ret(*this); }
  Instruction_assignment * Instruction_assignment::clone() { return new Instruction_assignment(*this); }
  Instruction_load * Instruction_load::clone() { return new Instruction_load(*this); }
  Instruction_shift * Instruction_shift::clone() { return new Instruction_shift(*this); }
  Instruction_store * Instruction_store::clone() { return new Instruction_store(*this); }
  Instruction_stack * Instruction_stack::clone() { return new Instruction_stack(*this); }
  Instruction_aop * Instruction_aop::clone() { return new Instruction_aop(*this); }
  Instruction_store_aop * Instruction_store_aop::clone() { return new Instruction_store_aop(*this); }
  Instruction_load_aop * Instruction_load_aop::clone() { return new Instruction_load_aop(*this); }
  Instruction_compare * Instruction_compare::clone() { return new Instruction_compare(*this); }
  Instruction_cjump * Instruction_cjump::clone() { return new Instruction_cjump(*this); }
  Instruction_call * Instruction_call::clone() { return new Instruction_call(*this); }
  Instruction_call_print * Instruction_call_print::clone() { return new Instruction_call_print(*this); }
  Instruction_call_input * Instruction_call_input::clone() { return new Instruction_call_input(*this); }
  Instruction_call_allocate * Instruction_call_allocate::clone() { return new Instruction_call_allocate(*this); }
  Instruction_call_error * Instruction_call_error::clone() { return new Instruction_call_error(*this); }
  Instruction_label * Instruction_label::clone() { return new Instruction_label(*this); }
  Instruction_increment * Instruction_increment::clone() { return new Instruction_increment(*this); }
  Instruction_decrement * Instruction_decrement::clone() { return new Instruction_decrement(*this); }
  Instruction_at * Instruction_at::clone() { return new Instruction_at(*this); }
  Instruction_goto * Instruction_goto::clone() { return new Instruction_goto(*this); }


//assignment 
void Instruction_assignment::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_assignment::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if(i != nullptr) {
    v.push_back(src);
  }
  return v;
}
vector<Item *> Instruction_assignment::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  Variable* i = dynamic_cast<Variable*>(dst); 
  if(i != nullptr) return {dst}; 
  return {};
}
//load 
Instruction_load::Instruction_load(Memory *m, Item *item) {
  this->src = m->getStartAddress();
  this->constant = new Number(m->getOffset());
  this->dst = item;
}

Instruction_load::Instruction_load() {}

void Instruction_load::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_load::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  if(src->toString() != "rsp") return {src};
  return {};
}
vector<Item *> Instruction_load::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  Variable* i = dynamic_cast<Variable*>(dst); 
  if(i != nullptr) return {dst}; 
  return {};
}

Instruction_store::Instruction_store(Memory *m, Item *item) {
  this->dst = m->getStartAddress();
  this->constant = new Number(m->getOffset());
  this->src = item;
}

Instruction_store::Instruction_store() {}

void Instruction_store::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_store::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
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

vector<Item *> Instruction_store::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {};
}
void Instruction_ret::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_ret::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item*> v;
  auto callee = Architecture::get_callee_saved_regs(); 
  for(auto i : callee){
    Register* r = registers[i];
    v.push_back(r); 
  }
  Register* rax = registers[Architecture::rax];
  v.push_back(rax);
  return v;
}
vector<Item *> Instruction_ret::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers)  { return {}; }

void Instruction_shift::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_shift::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
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

vector<Item *> Instruction_shift::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {dst};
}
void Instruction_aop::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_aop::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
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

vector<Item *> Instruction_aop::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {dst};
}
void Instruction_store_aop::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_store_aop::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
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

vector<Item *> Instruction_store_aop::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {};
}
void Instruction_stack::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_stack::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {};
}

vector<Item *> Instruction_stack::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {dst};
}
void Instruction_compare::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_compare::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
  {
    vector<Item *> v;
    Variable* op1 = dynamic_cast<Variable*>(oprand1);
    if (op1 != nullptr)
    {
      v.push_back(op1);
    }
    Variable* op2 = dynamic_cast<Variable*>(oprand2);
    if (op2 != nullptr)
    {
      v.push_back(op2);
    }
    return v;
  }
vector<Item *> Instruction_compare::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {dst};
}
void Instruction_cjump::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_cjump::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
    vector<Item *> v;
    Variable* op1 = dynamic_cast<Variable*>(oprand1);
    if (op1 != nullptr)
    {
      v.push_back(op1);
    }
    Variable* op2 = dynamic_cast<Variable*>(oprand2);
    if (op2 != nullptr)
    {
      v.push_back(op2);
    }
    return v;
}
vector<Item *> Instruction_cjump::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {};
}
void Instruction_call::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_call::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item*> v;
  Number* n = dynamic_cast<Number*>(constant); 
  Variable* i = dynamic_cast<Variable*>(dst); 
  auto args = Architecture::get_argument_regs(); 
  for(int i = 0; i < min((int)n->get(), 6); i++){
    Register* r = registers[args[i]]; 
    v.push_back(r); 
  }
  if(i != nullptr) v.push_back(i);
  return v;
}
vector<Item *> Instruction_call::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  auto caller_regs = Architecture::get_caller_saved_regs();
  for(auto i : caller_regs){
    Register* r = registers[i]; 
    v.push_back(r); 
  }
  return v;
}
void Instruction_call_print::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_call_print::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  Architecture::RegisterID arg = Architecture::get_argument_regs()[0]; 
  Register* r = registers[arg]; 
  return {r};
}

vector<Item *> Instruction_call_print::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  vector<Architecture::RegisterID> caller = Architecture::get_caller_saved_regs(); 
  for(auto i : caller){
    Register* r = registers[i]; 
    v.push_back(r); 
  }
  return v;
}
void Instruction_call_allocate::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_call_allocate::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  vector<Architecture::RegisterID> args = Architecture::get_argument_regs(); 
  for(int i = 0; i < 2; i++){
    Register* r = registers[args[i]]; 
    v.push_back(r); 
  }
  return v;
}
vector<Item *> Instruction_call_allocate::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  auto caller =  Architecture::get_caller_saved_regs(); 
  for(auto i : caller){
    Register* r = registers[i]; 
    v.push_back(r); 
  }
  return v;
}
void Instruction_call_error::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_call_error::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  Number* n = dynamic_cast<Number*>(constant); 
  auto args = Architecture::get_argument_regs(); 
  for(int i = 0; i < n->get(); i++){
    Register* r = registers[args[i]]; 
    v.push_back(r); 
  }
  return v;
}

vector<Item *> Instruction_call_error::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  auto caller = Architecture::get_caller_saved_regs(); 
  for(auto i : caller){
    Register* r = registers[i]; 
    v.push_back(r); 
  }
  return v;
}
void Instruction_increment::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_increment::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if(i != nullptr) v.push_back(i);
  return v;
}

vector<Item *> Instruction_increment::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if(i != nullptr) v.push_back(i);
  return v;
}
void Instruction_decrement::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_decrement::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if(i != nullptr) v.push_back(i);
  return v;
}

vector<Item *> Instruction_decrement::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  Variable* i = dynamic_cast<Variable*>(src);
  if(i != nullptr) v.push_back(i);
  return v;
}
void Instruction_at::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_at::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  v.push_back(src_mult);
  v.push_back(src_add);
  return v;
}

vector<Item *> Instruction_at::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {dst};
}
void Instruction_load_aop::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_load_aop::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;

  Variable* i = dynamic_cast<Variable*>(src);
  if (i != nullptr)
  {
    if(i->toString() != "rsp") v.push_back(i); 
  }
  v.push_back(dst);
  return v;
}
vector<Item *> Instruction_load_aop::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {dst};
}
void Instruction_goto::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_goto::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {};
}

vector<Item *> Instruction_goto::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {};
}
void Instruction_label::accept(Visitor* v) {
  v->visit(this);
}
vector<Item *> Instruction_label::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {};
}

vector<Item *> Instruction_label::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {};
}

vector<Item *> Instruction_call_input::get_gen_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  return {};
}

vector<Item *>  Instruction_call_input::get_kill_set(std::map<Architecture::RegisterID, Register*> &registers) 
{
  vector<Item *> v;
  vector<Architecture::RegisterID> caller = Architecture::get_caller_saved_regs(); 
  for(auto i : caller){
    Register* r = registers[i]; 
    v.push_back(r); 
  }
  return v;
}

void Instruction_call_input::accept(Visitor *v) {
  v->visit(this);
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
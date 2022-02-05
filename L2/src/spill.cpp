#include "spill.h"
#include <iostream>

namespace L2 {

  class Spiller_single : public Visitor {
    public:
    Spiller_single(std::ostream &os, Program &p, Function *f, int64_t &lineno) 
    :outputstream(os), prog(p), prefix(p.spill_prefix), func(f), lineno(lineno){
      this->spill_var = func->newVariable(p.spill_variable);
    }

    void visit(Instruction_ret *i) {
    }

    void visit(Instruction_assignment *i) {
      bool use = should_spill(&i->src);
      
      // cout << lineno << endl;
      bool define = should_spill(&i->dst);
      if (use)
        load();

      string s = i->toString();
      
      if (define)
        store();
      if (use || define)
        counter++;
    }

    void visit(Instruction_load *i) {
      bool define = should_spill(&i->dst);

      if (define) {
        store();
        counter++;
      }
    }

    void visit(Instruction_shift *i) {
    }


    void visit(Instruction_store *i) {
      bool src = should_spill(&i->src);
      bool dst = should_spill(&i->dst);
      bool use = src || dst;
      if (use) load();
      // if (&i->src->get_type() == item_variable) {
      //   load();
      // }
    }

    void visit(Instruction_stack *i) {
    }

    void visit(Instruction_aop *i) {
      bool src = should_spill(&i->src);
      bool dst = should_spill(&i->dst);
      bool use = src || dst;
      bool define = dst;

      if (use) load();
      if (define) store();
      if (use || define) counter++;
    }

    void visit(Instruction_store_aop *i) { 
      bool src = should_spill(&i->src);
      bool dst = should_spill(&i->dst);
      bool use = src || dst;
      if (use) load();
     }
    void visit(Instruction_load_aop *i) {}
    void visit(Instruction_compare *i) {}
    void visit(Instruction_cjump *i) {
      bool label = should_spill(&i->label);
      bool opr1 = should_spill(&i->oprand1);
      bool opr2 = should_spill(&i->oprand2);
      bool use = label || opr1 || opr2;
      if (use) load();
      if (use) counter++;
    }
    void visit(Instruction_call *i) { 
      bool use = should_spill(&i->dst); 
      if(use) load();
     }
    void visit(Instruction_call_print *i) {}
    void visit(Instruction_call_input *i) {}
    void visit(Instruction_call_allocate *i) {}
    void visit(Instruction_call_error *i) {}
    void visit(Instruction_label *i) {}
    void visit(Instruction_increment *i) { 
      bool spill = should_spill(&i->src);
      if (spill) load();
      if (spill) store();
      if (spill) counter++;
      }
    void visit(Instruction_decrement *i) {}
    void visit(Instruction_at *i) {}
    void visit(Instruction_goto *i) {}

    private:
      int counter = 0;
      int64_t offset = 0;
      ::string prefix;
      std::ostream &outputstream;
      // ::string spill_var;
      int64_t &lineno;
      Variable *spill_var;
      Function *func;
      Program &prog;

      void load() {
        Memory *m = new Memory(prog.getRegister(Architecture::rsp), new Number(offset));
        Instruction *ins = new Instruction_load(m, func->newVariable(prefix + to_string(counter)));
        func->instructions.insert(func->instructions.begin() + lineno, ins);
        // outputstream << "\t" << prefix + to_string(counter) << " <- mem rsp 0" << endl;
        lineno++;
      }
      
      void store() {
        Memory *m = new Memory(prog.getRegister(Architecture::rsp), new Number(offset));
        Instruction *ins = new Instruction_store(m, func->newVariable(prefix + to_string(counter)));
        func->instructions.insert(func->instructions.begin() + lineno + 1, ins);
        // outputstream << "\t" << "mem rsp 0 <- " << prefix + to_string(counter) << endl;
        lineno++;
      }

      bool should_spill(Item **item) {
        Variable *v = dynamic_cast<Variable *>(*item);
        if (v == nullptr) return false;
        if (v == spill_var) {
          *item = func->newVariable(prefix + to_string(counter));
          return true;
        }
        return false;
      }

      string replace_all(string s) {
        return s;
      }

  };

  bool checkVariablePresent(string spill_variable, Function* f){
    for(auto i: f->instructions){
      size_t n = i->toString().find(spill_variable); 
      if(n != string::npos) return true;
    }
    return false; 
  }
  void spill(Program &p) {
    int spill_variable_nb = 1;
    Function *f = p.functions[0];
    // cout << p.spill_variable << endl;
    if(!checkVariablePresent(p.spill_variable, f)) spill_variable_nb = 0;
    ostream &os = ::cout;
    int64_t lineno = 0;
    Spiller_single spiller(os, p, f, lineno);

    auto instruction_cp = f->instructions;
    for (auto i: instruction_cp) {
      i->accept(&spiller);
      lineno++;
    }   

    os << "(" << f->name << endl;
    os << "\t" << f->arguments << " " << spill_variable_nb << endl;
    for (auto i: f->instructions) {
      os << "\t" << i->toString() << endl;
    }
    os << ")" << endl;
  }



}
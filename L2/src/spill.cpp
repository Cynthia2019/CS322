#include "spill.h"
#include <iostream>

namespace L2 {

  class Spiller_single : public Visitor {
    public:
    Spiller_single(
      std::ostream &os,
      Program *p,
      Function *f,
      Variable *v,
      ::string prefix,
      int64_t &lineno,
      int64_t start_stack_pos,
      std::unordered_map<Variable *, bool> &newVariables) 
        :outputstream(os), prog(p), prefix(prefix), func(f), spill_var(v),
        lineno(lineno), start_stack_pos(start_stack_pos), newVariables(newVariables){
      offset = start_stack_pos * Architecture::stackWordLen;
    }

    bool if_spilled() {
      return if_spilled_at_all;
    }

    // void initSpill(Variable *spill_var) {
    //   this->spill_var = spill_var;
    //   lineno = 0;
    // } 

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
      int64_t counter = 0;
      int64_t offset = 0;
      ::string prefix;
      std::ostream &outputstream;
      // ::string spill_var;
      int64_t &lineno;
      Variable *spill_var;
      Function *func;
      Program *prog;
      bool if_spilled_at_all = false;
      int64_t start_stack_pos;
      std::unordered_map<Variable *, bool> &newVariables;

      void load() {
        Memory *m = new Memory(prog->getRegister(Architecture::rsp), new Number(offset));
        Variable *nv = func->newVariable(prefix + to_string(counter));
        Instruction *ins = new Instruction_load(m, nv);
        func->instructions.insert(func->instructions.begin() + lineno, ins);
        newVariables[nv] = true;
        // outputstream << "\t" << prefix + to_string(counter) << " <- mem rsp 0" << endl;
        lineno++;
        if_spilled_at_all = true;
      }
      
      void store() {
        Memory *m = new Memory(prog->getRegister(Architecture::rsp), new Number(offset));
        Variable *nv = func->newVariable(prefix + to_string(counter));
        Instruction *ins = new Instruction_store(m, nv);
        func->instructions.insert(func->instructions.begin() + lineno + 1, ins);
        newVariables[nv] = true;
        // outputstream << "\t" << "mem rsp 0 <- " << prefix + to_string(counter) << endl;
        lineno++;
        if_spilled_at_all = true;
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
  };

  // bool checkVariablePresent(string spill_variable, Function* f){
  //   for(auto i: f->instructions){
  //     size_t n = i->toString().find(spill_variable); 
  //     if(n != string::npos) return true;
  //   }
  //   return false; 
  // }

  int64_t spillMultiple(Program *p, Function *f, vector<Variable *> tospill, 
      std::unordered_map<Variable *, bool> &newVariable) {
    ::cout << "spill multiple" << endl;
    ::string prefix = "%S" + to_string(1) + "_";
    int64_t start_pos = newVariable.size();

    // ostream &os = ::cout;
    // int64_t lineno = 0;
    // Spiller_single spiller(os, p, f, , prefix, lineno, start_pos);
    int spill_variable_nb = 0;

    for (auto v : tospill) {
      //TODO: check spilled variable
      bool spilled = spillOne(p, f, v, prefix, newVariable);
      if (spilled) {
        spill_variable_nb++;
        start_pos++;
        // newVariable[f->newVariable()]
      }
    }

  //  os << "(" << f->name << endl;
  //   os << "\t" << f->arguments << " " << spill_variable_nb << endl;
  //   for (auto i: f->instructions) {
  //     os << "\t" << i->toString() << endl;
  //   }
  //   os << ")" << endl;
  cout << "number " << spill_variable_nb << endl;
    return spill_variable_nb;
  }

  void spillAll() {
    //TODO: fill this out
    ::cout << "spilling all" << endl;
  }

  bool spillOne(Program *p, Function *f, Variable *spill_var, std::string prefix, 
      std::unordered_map<Variable *, bool> &newVariable) {
    // int spill_variable_nb = 0;
    ostream &os = ::cout;
    int64_t lineno = 0, start_pos = newVariable.size();
    cout << "variable spilled : " << spill_var->toString() << " prefix: " << prefix << endl;
    Spiller_single spiller(os, p, f, spill_var, prefix, lineno, start_pos, newVariable);
    // spiller.initSpill(spill_var);

    auto instruction_cp = f->instructions;
    for (auto i: instruction_cp) {
      i->accept(&spiller);
      lineno++;
    }   

    // if (spiller.if_spilled()) {
    //   spill_variable_nb = 1;
    // }

    // os << "(" << f->name << endl;
    // os << "\t" << f->arguments << " " << spill_variable_nb << endl;
    // for (auto i: f->instructions) {
    //   os << "\t" << i->toString() << endl;
    // }
    // os << ")" << endl;
    return spiller.if_spilled();
  }



}
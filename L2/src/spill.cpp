#include "spill.h"
#include <iostream>

namespace L2 {

  class Spiller_single : public Spiller {
    public:
    Spiller_single(std::ostream &os, ::string prefix, ::string spill_var) 
    :outputstream(os), prefix(prefix), spill_var(spill_var) {
      // cout << spill_var << endl;
    }

    void visit(Instruction_ret *i) {
      outputstream << " " << i->tostring() << endl;
    }

    void visit(Instruction_assignment *i) {
      bool spilled = false;
      if (i->src->get_type() == item_variable) {
        load();
        spilled = true;
      }
      outputstream << " " << i->tostring() << endl;
      if (i->dst->get_type() == item_variable) {
        store();
        spilled = true;
      }
      if (spilled)
        counter++;
    }

    void visit(Instruction_load *i) {
      outputstream << " " << i->tostring() << endl;
      if (i->dst->get_type() == item_variable) {
        store();
        counter++;
      }
    }

    void visit(Instruction_shift *i) {
      outputstream << " " << i->tostring() << endl;
    }


    void visit(Instruction_store *i) {
      if (i->src->get_type() == item_variable) {
        load();
      }
      outputstream << " " << i->tostring() << endl;
    }

    void visit(Instruction_stack *i) {
      outputstream << " " << i->tostring() << endl;
    }

    void visit(Instruction_aop *i) {
      outputstream << " " << i->tostring() << endl;
    }

    void visit(Instruction_store_aop *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_load_aop *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_compare *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_cjump *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_call *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_call_print *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_call_input *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_call_allocate *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_call_error *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_label *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_increment *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_decrement *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_at *i) { outputstream << " " << i->tostring() << endl; }
    void visit(Instruction_goto *i) { outputstream << " " << i->tostring() << endl; }


    private:
      int counter = 0;
      ::string prefix;
      std::ostream &outputstream;
      ::string spill_var;
      void load() {
        outputstream << " " << prefix + to_string(counter) << "<- mem rsp 0" << endl;
      }
      
      void store() {
        outputstream << " " << "mem rsp 0 <- " << prefix + to_string(counter) << endl;
      }
  };

  const int spill_variable_nb = 1;
  void spill(Program &p) {
    Function *f = p.functions[0];
    // cout << p.spill_variable << endl;
    Spiller_single spiller(::cout, p.spill_prefix, p.spill_variable);
    for (auto i: f->instructions) {
      i->spill(spiller);
    }
  }

}
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
      outputstream << "\t" << i->tostring() << endl;
    }

    void visit(Instruction_assignment *i) {
      bool use = should_spill(i->src), define = should_spill(i->dst);
      if (use)
        load();

      string s = i->tostring();
      outputstream << "\t" << replace_all(s) << endl;
      
      if (define)
        store();
      if (use || define)
        counter++;
    }

    void visit(Instruction_load *i) {
      bool define = should_spill(i->dst);
      outputstream << "\t";
      outputstream << replace_all(i->tostring());

      if (define) {
        store();
        counter++;
      }
    }

    void visit(Instruction_shift *i) {
      outputstream << "\t" << i->tostring() << endl;
    }


    void visit(Instruction_store *i) {
      if (i->src->get_type() == item_variable) {
        load();
      }
      outputstream << "\t" << i->tostring() << endl;
    }

    void visit(Instruction_stack *i) {
      outputstream << "\t" << i->tostring() << endl;
    }

    void visit(Instruction_aop *i) {
      bool use = should_spill(i->src) || should_spill(i->dst);
      bool define = should_spill(i->dst);
      if (use) load();
      outputstream << "\t" << replace_all(i->tostring()) << endl;
      if (define) store();
      if (use || define) counter++;
    }

    void visit(Instruction_store_aop *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_load_aop *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_compare *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_cjump *i) {
      bool use = should_spill(i->label) || should_spill(i->oprand1) || should_spill(i->oprand2);
      if (use) load();
      outputstream << "\t" << replace_all(i->tostring()) << endl;
      if (use) counter++;
    }
    void visit(Instruction_call *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_call_print *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_call_input *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_call_allocate *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_call_error *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_label *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_increment *i) { 
      bool spill = should_spill(i->src);
      if (spill) load();
      outputstream << "\t" << replace_all(i->tostring()) << endl;
      if (spill) store();
      if (spill) counter++;
      }
    void visit(Instruction_decrement *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_at *i) { outputstream << "\t" << i->tostring() << endl; }
    void visit(Instruction_goto *i) { outputstream << "\t" << i->tostring() << endl; }


    private:
      int counter = 0;
      ::string prefix;
      std::ostream &outputstream;
      ::string spill_var;
      void load() {
        outputstream << "\t" << prefix + to_string(counter) << " <- mem rsp 0" << endl;
      }
      
      void store() {
        outputstream << "\t" << "mem rsp 0 <- " << prefix + to_string(counter) << endl;
      }

      bool should_spill(Item *item) {
        return (item->get_type() == item_variable && item->get_content() == spill_var);
      }

      string replace_all(string s) {
        size_t n = 0;
        string t = prefix + to_string(counter);
        while ((n = s.find(spill_var, n)) != string::npos) {
          s.replace(n, spill_var.size(), prefix + to_string(counter));
          n += t.size();
        }
        return s;
      }

  };

  const int spill_variable_nb = 1;
  void spill(Program &p) {
    Function *f = p.functions[0];
    // cout << p.spill_variable << endl;
    ostream &os = ::cout;
    Spiller_single spiller(os, p.spill_prefix, p.spill_variable);
    
    os << "(" << f->name << endl;
    os << "\t" << f->arguments << " " << spill_variable_nb << endl;
    for (auto i: f->instructions) {
      i->spill(spiller);
    }
    os << ")" << endl;
  }



}
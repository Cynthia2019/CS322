// #include "spill.h"
// #include <iostream>

// namespace L2 {

//   class Spiller_single : public Spiller {
//     public:
//     Spiller_single(std::ostream &os, ::string prefix, ::string spill_var) 
//     :outputstream(os), prefix(prefix), spill_var(spill_var) {
//       // cout << spill_var << endl;
//     }

//     void visit(Instruction_ret *i) {
//       outputstream << "\t" << i->toString() << endl;
//     }

//     void visit(Instruction_assignment *i) {
//       bool use = should_spill(i->src), define = should_spill(i->dst);
//       if (use)
//         load();

//       string s = i->toString();
//       outputstream << "\t" << replace_all(s) << endl;
      
//       if (define)
//         store();
//       if (use || define)
//         counter++;
//     }

//     void visit(Instruction_load *i) {
//       bool define = should_spill(i->dst);
//       outputstream << "\t";
//       outputstream << replace_all(i->toString());

//       if (define) {
//         store();
//         counter++;
//       }
//     }

//     void visit(Instruction_shift *i) {
//       outputstream << "\t" << i->toString() << endl;
//     }


//     void visit(Instruction_store *i) {
//       bool use = should_spill(i->src) || should_spill(i->dst);
//       if (use) load();
//       outputstream << "\t" << replace_all(i->toString()) << endl;
//       // if (i->src->get_type() == item_variable) {
//       //   load();
//       // }
//       // outputstream << "\t" << i->toString() << endl;
//     }

//     void visit(Instruction_stack *i) {
//       outputstream << "\t" << i->toString() << endl;
//     }

//     void visit(Instruction_aop *i) {
//       bool use = should_spill(i->src) || should_spill(i->dst);
//       bool define = should_spill(i->dst);
//       if (use) load();
//       outputstream << "\t" << replace_all(i->toString()) << endl;
//       if (define) store();
//       if (use || define) counter++;
//     }

//     void visit(Instruction_store_aop *i) { 
//       bool use = should_spill(i->src) || should_spill(i->dst);
//       if (use) load();
//       outputstream << "\t" << replace_all(i->toString()) << endl;
//      }
//     void visit(Instruction_load_aop *i) { outputstream << "\t" << i->toString() << endl; }
//     void visit(Instruction_compare *i) { outputstream << "\t" << i->toString() << endl; }
//     void visit(Instruction_cjump *i) {
//       bool use = should_spill(i->label) || should_spill(i->oprand1) || should_spill(i->oprand2);
//       if (use) load();
//       outputstream << "\t" << replace_all(i->toString()) << endl;
//       if (use) counter++;
//     }
//     void visit(Instruction_call *i) { 
//       bool use = should_spill(i->dst); 
//       if(use) load();
//       outputstream << "\t" << replace_all(i->toString()) << endl;
//      }
//     void visit(Instruction_call_print *i) { outputstream << "\t" << i->toString() << endl; }
//     void visit(Instruction_call_input *i) { outputstream << "\t" << i->toString() << endl; }
//     void visit(Instruction_call_allocate *i) { outputstream << "\t" << i->toString() << endl; }
//     void visit(Instruction_call_error *i) { outputstream << "\t" << i->toString() << endl; }
//     void visit(Instruction_label *i) { outputstream << "\t" << i->toString() << endl; }
//     void visit(Instruction_increment *i) { 
//       bool spill = should_spill(i->src);
//       if (spill) load();
//       outputstream << "\t" << replace_all(i->toString()) << endl;
//       if (spill) store();
//       if (spill) counter++;
//       }
//     void visit(Instruction_decrement *i) { outputstream << "\t" << i->toString() << endl; }
//     void visit(Instruction_at *i) { outputstream << "\t" << i->toString() << endl; }
//     void visit(Instruction_goto *i) { outputstream << "\t" << i->toString() << endl; }

//     private:
//       int counter = 0;
//       ::string prefix;
//       std::ostream &outputstream;
//       ::string spill_var;
//       void load() {
//         outputstream << "\t" << prefix + to_string(counter) << " <- mem rsp 0" << endl;
//       }
      
//       void store() {
//         outputstream << "\t" << "mem rsp 0 <- " << prefix + to_string(counter) << endl;
//       }

//       bool should_spill(Item *item) {
//         return (item->get_type() == item_variable && item->get_content() == spill_var);
//       }

//       string replace_all(string s) {
//         size_t n = 0;
//         string t = prefix + to_string(counter);
//         while ((n = s.find(spill_var, n)) != string::npos) {
//           s.replace(n, spill_var.size(), prefix + to_string(counter));
//           n += t.size();
//         }
//         return s;
//       }

//   };

//   bool checkVariablePresent(string spill_variable, Function* f){
//     for(auto i: f->instructions){
//       size_t n = i->toString().find(spill_variable); 
//       if(n != string::npos) return true;
//     }
//     return false; 
//   }
//   void spill(Program &p) {
//     int spill_variable_nb = 1;
//     Function *f = p.functions[0];
//     // cout << p.spill_variable << endl;
//     if(!checkVariablePresent(p.spill_variable, f)) spill_variable_nb = 0;
//     ostream &os = ::cout;
//     Spiller_single spiller(os, p.spill_prefix, p.spill_variable);
    
//     os << "(" << f->name << endl;
//     os << "\t" << f->arguments << " " << spill_variable_nb << endl;
//     for (auto i: f->instructions) {
//       i->spill(spiller);
//     }
//     os << ")" << endl;
//   }



// }
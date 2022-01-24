#include <string>
#include <iostream>
#include <fstream>

#include <liveness.h>

using namespace std;

namespace L2 {
    void print_vector(vector<Item *>& v) {
    for(auto i : v){
      if(i->get_type() == item_variable) {
        Item_variable* a = dynamic_cast<Item_variable*>(i); 
        std::cout << a->variable_name; 
      }
      else if (i->get_type() == item_register){
        Item_register* a = dynamic_cast<Item_register*>(i); 
        std::cout  << a->register_name; 
      }
      else if (i->get_type() == item_number) {
        Item_number* a = dynamic_cast<Item_number*>(i); 
        std::cout  << a->get_nb(); 
      }
      std::cout  << " "; 
    }
    std::cout  << std::endl; 
  }
  Instruction* get_successor(vector<Instruction*>& instructions, Instruction* i){
      //two successors
      Instruction_goto* a = dynamic_cast<Instruction_goto*>(i); 
      if(a != nullptr){
          Item* label = a->label; 
          cout << "curr label: " << label << endl; 
          int index = find(instructions.begin(), instructions.end(), i) - instructions.begin() + 1; 
          cout << "next index: " << index << endl; 
          while(index < instructions.size()){
              Instruction* curr = instructions[index]; 
              class Instruction_label* currLabel = dynamic_cast<class Instruction_label*>(curr); 
              if(currLabel != nullptr && &(currLabel->label) == &label){
                  return currLabel; 
              }
              index++; 
          }
      }
  }
    void liveness(Program p) {
        auto f = p.functions[0]; 
        for(auto i : f->instructions) {
            vector<Item*> gens = i->get_gen_set(); 
            vector<Item*> kills = i->get_kill_set(); 
            cout << "gens: "; 
            print_vector(gens);
            cout << "kills: "; 
            print_vector(kills);
            Instruction* s = get_successor(f->instructions, i);
        }
    }
}
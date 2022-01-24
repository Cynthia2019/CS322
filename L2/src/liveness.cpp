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
    void liveness(Program p) {
        auto f = p.functions[0]; 
        for(auto i : f->instructions) {
            vector<Item*> gens = i->get_gen_set(); 
            vector<Item*> kills = i->get_kill_set(); 
            cout << "gens: "; 
            print_vector(gens);
            cout << "kills: "; 
            print_vector(kills);
        }
    }
}
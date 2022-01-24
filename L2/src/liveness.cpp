#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <iterator>

#include <liveness.h>

using namespace std;

namespace L2 {
  void format_vector(const vector<set<string>> in, const vector<set<string>> out) {
    cout << "((in" << endl;
    for (auto i : in) {
      cout << "(";
      for (auto j : i) {
        cout << j << " ";
      }
        cout << ")" << endl;;
    }
    cout << ") (out" << endl;
    for (auto i : out) {
      cout << "(";
      for (auto j : i) {
        cout << j << " ";
      }
        cout << ")" << endl;;
    }
    cout << "))" << endl;
  }

    void print_vector(const vector<set<string>> v) {
        for (auto i : v) {
            cout << "{ ";
            for (auto j : i) {
                cout << j << ",";
            }
            cout << "}";
            cout << endl;
        }
    }

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
        vector<set<string>> gens;
        vector<set<string>> kills;

        for(auto i : f->instructions) {
            auto gen = i->get_gen_set();
            auto kill = i->get_kill_set();
            set<string> gen_str;
            set<string> kill_str;

            for (auto g : gen) {
                gen_str.insert(g->get_content());
            }
            gens.push_back(gen_str);

            for (auto k : gen) {
                kill_str.insert(k->get_content());
            }
            kills.push_back(kill_str);

            // cout << "instructions: " << i->tostring() << endl; 
        }
        // cout << "genset" << endl;
        // print_vector(gens);
        // cout << "kill set" << endl;
        // print_vector(kills);

        vector<set<string>> in(f->instructions.size());
        vector<set<string>> out(f->instructions.size());

        bool changed = false; do {
            changed = false;
            for (int i = 0; i < f->instructions.size(); i++) {
                auto gen = gens[i];
                auto kill = kills[i];
                auto in_before = in[i];
                in[i] = {};

                set<string> diff;
                std::set_difference(out[i].begin(), out[i].end(), kill.begin(), kill.end(),
                        std::inserter(diff, diff.end()));
                
                in[i].insert(gen.begin(), gen.end());
                in[i].insert(diff.begin(), diff.end());
                if (in[i] != in_before) {
                    changed = true;
                }

                // auto out_before = out[i];
                // out[i] = {};
                // if (out[i] != out_before) {
                //     changed = true;
                // }
            }
        } while(changed);
        format_vector(in, out);
    }
}
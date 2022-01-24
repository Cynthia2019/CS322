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

  vector<Instruction*> get_successor(vector<Instruction*>& instructions, Instruction* i){
      Instruction_goto* a = dynamic_cast<Instruction_goto*>(i); 
      if(a != nullptr){
          vector<Instruction*> successors; 
          Item* label = a->label; 
          int index = find(instructions.begin(), instructions.end(), i) - instructions.begin() + 1; 
          while(index < instructions.size()){
              Instruction* curr = instructions[index]; 
              class Instruction_label* currLabel = dynamic_cast<class Instruction_label*>(curr); 
              if(currLabel != nullptr && currLabel->label->get_content() == label->get_content()){
                  successors.push_back(currLabel); 
              }
              index++; 
          }
          return successors; 
      }
      //two successors
      Instruction_cjump* b = dynamic_cast<Instruction_cjump*>(i); 
      if(b != nullptr) {
          vector<Instruction*> successors; 
          Item* label = b->label; 
          int index = find(instructions.begin(), instructions.end(), i) - instructions.begin() + 1;  
          successors.push_back(instructions[index+1]); 
          while(index < instructions.size()){
              Instruction* curr = instructions[index]; 
              class Instruction_label* currLabel = dynamic_cast<class Instruction_label*>(curr); 
              if(currLabel != nullptr && currLabel->label->get_content() == label->get_content()){
                  successors.push_back(currLabel); 
              }
              index++; 
          }
          return successors; 
      }
      //no successor 
      Instruction_ret* c = dynamic_cast<Instruction_ret*>(i);
      if(c != nullptr){
          return {}; 
      }
      Instruction_call_error * d = dynamic_cast<Instruction_call_error*>(i); 
      if(d != nullptr){
          return {}; 
      }
      //one successor 
      int index = find(instructions.begin(), instructions.end(), i) - instructions.begin() + 1; 
      return {instructions[index]};

  }
    void liveness(Program p) {
        auto f = p.functions[0]; 
        vector<set<string>> gens;
        vector<set<string>> kills;

        for(auto i : f->instructions) {
            vector<Instruction*> successors = get_successor(f->instructions, i);
            for(auto s : successors) {
                cout << "ins: " << i->tostring()  << "  successor: " << s->tostring() << endl;
            }
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
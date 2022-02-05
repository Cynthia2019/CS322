#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <map>
#include <iterator>

#include <interference.h>
#include <liveness.h>
#include <architecture.h>

using namespace std;

namespace L2
{
    void print_map(map<Item*, set<Item*>>& m){
        for(auto i : m){
            cout << i.first->toString() << " "; 
            for(auto s : i.second){
                cout << s->toString() << " ";
            }
            cout << endl;
        }
    }
    void interference(Program p) {
        AnalysisResult* res = computeLiveness(p).first;
        std::map<Instruction*, std::set<Item*>> in = res->ins; 
        std::map<Instruction*, std::set<Item*>> out = res->outs; 
        std::map<Instruction*, std::set<Item*>> gens = res->gens; 
        std::map<Instruction*, std::set<Item*>> kills = res->kills; 

        map<Item*, set<Item*>> edges; 
        int length = p.functions[0]->instructions.size(); 
        //get all gp registers
        vector<Architecture::RegisterID> caller = Architecture::get_caller_saved_regs(); 
        vector<Architecture::RegisterID> callee = Architecture::get_callee_saved_regs(); 
        vector<Architecture::RegisterID> gp = caller; 
        gp.insert(gp.end(), callee.begin(), callee.end());  
        //connect gp to all other registers
        for(auto item1 : gp){
            for(auto item2 : gp){
                if(item1 != item2) {
                    edges[p.getRegister(item1)].insert(p.getRegister(item2)); 
                    edges[p.getRegister(item2)].insert(p.getRegister(item1)); 
                }
            }
        }
        for(Instruction* inst : p.functions[0]->instructions) {
            //connect each pair of variables in the same IN set 
            for(auto s1 : in[inst]){
                for(auto s2 : in[inst]){
                    if(s1 != s2) {
                        edges[s1].insert(s2); 
                        edges[s2].insert(s1); 
                    }
                }
            }
            //connect each pair of variables in the same out set 
            for(auto s1 : out[inst]){
                for(auto s2 : out[inst]){
                    if(s1 != s2) {
                        edges[s1].insert(s2); 
                        edges[s2].insert(s1); 
                    }
                }
            }
            //connect variables in kill with those in out
            for(auto s1 : kills[inst]){
                for(auto s2 : out[inst]){
                    if(s1 != s2){
                        edges[s1].insert(s2); 
                        edges[s2].insert(s1);
                    }
                }
            }
            //check restrictions for each instruction
            //sop 
            Instruction_shift* a = dynamic_cast<Instruction_shift*>(inst); 
            if(a != nullptr) {
                //connect every register with dst except rcx
                Item* src = a->src; 
                for(Architecture::RegisterID item : gp) {
                    if(item != Architecture::rcx && p.getRegister(item) != src) {
                        edges[src].insert(p.getRegister(item)); 
                        edges[p.getRegister(item)].insert(src); 
                    }
                }
                
            }

        }
        print_map(edges); 
    }
}
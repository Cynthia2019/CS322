// #include <string>
// #include <iostream>
// #include <fstream>
// #include <algorithm>
// #include <set>
// #include <map>
// #include <iterator>

// #include <interference.h>
// #include <liveness.h>
// #include <architecture.h>

// using namespace std;

// namespace L2
// {
//     void print_map(map<string, set<string>>& m){
//         for(auto i : m){
//             cout << i.first << " "; 
//             for(string s : i.second){
//                 cout << s << " ";
//             }
//             cout << endl;
//         }
//     }
//     vector<vector<set<string>>> liveness_helper(Program p)
//     {
//         auto f = p.functions[0];
//         vector<set<string>> gens;
//         vector<set<string>> kills;

//         for (auto i : f->instructions)
//         {
//             auto gen = i->get_gen_set();
//             auto kill = i->get_kill_set();
//             set<string> gen_str;
//             set<string> kill_str;
//             for (auto g : gen)
//             {
//                 gen_str.insert(g->toString());
//             }
//             gens.push_back(gen_str);
//             for (auto k : kill)
//             {
//                 kill_str.insert(k->toString());
//             }
//             kills.push_back(kill_str);
//         }

//         vector<set<string>> in(f->instructions.size());
//         vector<set<string>> out(f->instructions.size());

//         bool changed = false;
//         do
//         {
//             changed = false;
//             for (int i = f->instructions.size() - 1; i >= 0; i--)
//             {
//                 auto gen = gens[i];
//                 auto kill = kills[i];
//                 auto in_before = in[i];
//                 in[i] = {};

//                 set<string> diff;
//                 std::set_difference(out[i].begin(), out[i].end(), kill.begin(), kill.end(),
//                                     std::inserter(diff, diff.end()));

//                 in[i].insert(gen.begin(), gen.end());
//                 in[i].insert(diff.begin(), diff.end());
//                 if (in[i] != in_before)
//                 {
//                     changed = true;
//                 }

//                 auto out_before = out[i];
//                 out[i] = {};
//                 vector<int> idxs = get_successor(f->instructions, i);
//                 for (auto t : idxs)
//                 {
//                     out[i].insert(in[t].begin(), in[t].end());
//                 }
//                 if (out[i] != out_before)
//                 {
//                     changed = true;
//                 }
//             }
//         } while (changed);
//         return {in, out, gens, kills}; 
//     }

//     void interference(Program p) {
//         vector<vector<set<string>>> res = liveness_helper(p); 
//         vector<set<string>> in = res[0]; 
//         vector<set<string>> out = res[1]; 
//         vector<set<string>> gens = res[2]; 
//         vector<set<string>> kills = res[3]; 

//         map<string, set<string>> edges; 
//         int length = p.functions[0]->instructions.size(); 
//         //get all gp registers
//         vector<Item*> caller = get_caller_saved_regs(); 
//         vector<Item*> callee = get_callee_saved_regs(); 
//         vector<Item*> gp = caller; 
//         gp.insert(gp.end(), callee.begin(), callee.end());  
//         //connect gp to all other registers
//         for(auto item1 : gp){
//             for(auto item2 : gp){
//                 if(item1 != item2) {
//                     edges[item1->toString()].insert(item2->toString()); 
//                     edges[item2->toString()].insert(item1->toString()); 
//                 }
//             }
//         }
//         for(int idx = 0; idx < length; idx++) {
//             //connect each pair of variables in the same IN set 
//             for(string s1 : in[idx]){
//                 for(string s2 : in[idx]){
//                     if(s1 != s2) {
//                         edges[s1].insert(s2); 
//                         edges[s2].insert(s1); 
//                     }
//                 }
//             }
//             //connect each pair of variables in the same out set 
//             for(string s1 : out[idx]){
//                 for(string s2 : out[idx]){
//                     if(s1 != s2) {
//                         edges[s1].insert(s2); 
//                         edges[s2].insert(s1); 
//                     }
//                 }
//             }
//             //connect variables in kill with those in out
//             for(string s1 : kills[idx]){
//                 for(string s2 : out[idx]){
//                     if(s1 != s2){
//                         edges[s1].insert(s2); 
//                         edges[s2].insert(s1);
//                     }
//                 }
//             }
//             //check restrictions for each instruction
//             //sop 
//             Instruction* i = p.functions[0]->instructions[idx]; 
//             Instruction_shift* a = dynamic_cast<Instruction_shift*>(i); 
//             if(a != nullptr) {
//                 //connect every register with dst except rcx
//                 Item* src = a->src; 
//                 if(src->get_type() == item_variable) {
//                     for(auto item : gp) {
//                         if(item->toString() != "rcx" && item->toString() != src->toString()) {
//                             edges[src->toString()].insert(item->toString()); 
//                             edges[item->toString()].insert(src->toString()); 
//                         }
//                     }
//                 }
//             }

//         }
//         print_map(edges); 
//     }
// }
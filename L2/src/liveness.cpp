#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <iterator>

#include <liveness.h>

using namespace std;

namespace L2 {
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

            cout << "instructions: " << i->tostring() << endl; 
        }

        vector<set<string>> in(f->instructions.size());
        vector<set<string>> out(f->instructions.size());

        bool changed = false;
        do {
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
    }
}
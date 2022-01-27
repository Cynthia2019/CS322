#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <iterator>

#include <interference.h>
#include <liveness.h>

using namespace std;

namespace L2
{

    vector<vector<set<string>>> liveness_helper(Program p)
    {
        auto f = p.functions[0];
        vector<set<string>> gens;
        vector<set<string>> kills;

        for (auto i : f->instructions)
        {
            auto gen = i->get_gen_set();
            auto kill = i->get_kill_set();
            set<string> gen_str;
            set<string> kill_str;
            for (auto g : gen)
            {
                gen_str.insert(g->get_content());
            }
            gens.push_back(gen_str);
            for (auto k : kill)
            {
                kill_str.insert(k->get_content());
            }
            kills.push_back(kill_str);
        }

        vector<set<string>> in(f->instructions.size());
        vector<set<string>> out(f->instructions.size());

        bool changed = false;
        do
        {
            changed = false;
            for (int i = f->instructions.size() - 1; i >= 0; i--)
            {
                auto gen = gens[i];
                auto kill = kills[i];
                auto in_before = in[i];
                in[i] = {};

                set<string> diff;
                std::set_difference(out[i].begin(), out[i].end(), kill.begin(), kill.end(),
                                    std::inserter(diff, diff.end()));

                in[i].insert(gen.begin(), gen.end());
                in[i].insert(diff.begin(), diff.end());
                if (in[i] != in_before)
                {
                    changed = true;
                }

                auto out_before = out[i];
                out[i] = {};
                vector<int> idxs = get_successor(f->instructions, i);
                for (auto t : idxs)
                {
                    out[i].insert(in[t].begin(), in[t].end());
                }
                if (out[i] != out_before)
                {
                    changed = true;
                }
            }
        } while (changed);
        return {in, out, gens, kills}; 
    }

    void interference(Program p) {
        vector<vector<set<string>>> res = liveness_helper(p); 
        vector<set<string>> in = res[0]; 
        vector<set<string>> out = res[1]; 
        vector<set<string>> gens = res[2]; 
        vector<set<string>> kills = res[3]; 
        

    }
}
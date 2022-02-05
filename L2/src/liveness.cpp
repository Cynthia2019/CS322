#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <iterator>

#include <liveness.h>

using namespace std;

namespace L2
{
    void format_vector(const vector<set<Item*>> in, const vector<set<Item*>> out)
    {
        cout << "(" << endl;
        cout << "(in" << endl;
        for (auto i : in)
        {
            cout << "(";
            for (auto j : i)
            {
                cout << j->toString() << " ";
            }
            cout << ")" << endl;
            ;
        }
        cout << ")" << endl
             << endl;
        cout << "(out" << endl;
        for (auto i : out)
        {
            cout << "(";
            for (auto j : i)
            {
                cout << j->toString() << " ";
            }
            cout << ")" << endl;
            ;
        }
        cout << ")" << endl
             << endl;
        cout << ")" << endl;
    }

    vector<int> get_successor(vector<Instruction *> &instructions, int idx)
    {
        Instruction *i = instructions[idx];
        Instruction_goto *a = dynamic_cast<Instruction_goto *>(i);
        if (a != nullptr)
        {
            vector<int> successors;
            Item* label = a->label;
            int index = 0;
            while (index < instructions.size())
            {
                Instruction *curr = instructions[index];
                class Instruction_label *currLabel = dynamic_cast<class Instruction_label *>(curr);
                if (currLabel != nullptr && currLabel->label->toString() == label->toString())
                {
                    successors.push_back(index);
                }
                index++;
            }
            return successors;
        }
        // two successors
        Instruction_cjump *b = dynamic_cast<Instruction_cjump *>(i);
        if (b != nullptr)
        {
            vector<int> successors;
            Item *label = b->label;
            successors.push_back(idx + 1);
            int index = 0;
            while (index < instructions.size())
            {
                Instruction *curr = instructions[index];
                class Instruction_label *currLabel = dynamic_cast<class Instruction_label *>(curr);
                if (currLabel != nullptr && currLabel->label->toString() == label->toString())
                {
                    successors.push_back(index);
                }
                index++;
            }
            return successors;
        }
        // no successor
        Instruction_ret *c = dynamic_cast<Instruction_ret *>(i);
        if (c != nullptr)
        {
            return {};
        }
        Instruction_call_error *d = dynamic_cast<Instruction_call_error *>(i);
        if (d != nullptr)
        {
            return {};
        }
        // one successor
        if (idx == instructions.size() - 1)
        {
            return {};
        }
        return {idx + 1};
    }
    std::pair<AnalysisResult*, vector<vector<set<Item*>>>> computeLiveness(Program& p) {
        AnalysisResult* res = new AnalysisResult();
        auto f = p.functions[0];
        vector<set<Item*>> gens; 
        vector<set<Item*>> kills; 

        for (auto i : f->instructions)
        {
            auto gen = i->get_gen_set(p.registers);
            auto kill = i->get_kill_set(p.registers);
            set<Item*> gen_var;
            set<Item*> kill_var;
           // cout << "gen: ";
            for (int i = 0; i < gen.size(); i++)
            {
                //cout << gen[i]->toString() << " "; 
                gen_var.insert(gen[i]); 
            }
            gens.push_back(gen_var);
           // cout << endl << "kill: ";
            for (int i = 0; i < kill.size(); i++)
            {
              //  cout << kill[i]->toString() << " ";
                kill_var.insert(kill[i]);
            }
          //  cout << endl;
            kills.push_back(kill_var);
        }

        vector<set<Item*>> in(f->instructions.size());
        vector<set<Item*>> out(f->instructions.size());

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

                set<Item*> diff;
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
                Instruction* iptr = f->instructions[i];
                res->gens[iptr] = gen; 
                res->kills[iptr] = kill; 
                res->ins[iptr] = in[i];
                res->outs[iptr] = out[i];
            }
        } while (changed);
        return {res, {in, out}}; 
    }
    void liveness(Program p)
    {
        std::pair<AnalysisResult*, vector<vector<set<Item*>>>> res = computeLiveness(p); 
        vector<set<Item*>> in = res.second[0]; 
        vector<set<Item*>> out = res.second[1];
        format_vector(in, out);
    }
}
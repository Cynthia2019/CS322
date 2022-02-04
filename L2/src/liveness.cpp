// #include <string>
// #include <iostream>
// #include <fstream>
// #include <algorithm>
// #include <set>
// #include <iterator>

// #include <liveness.h>

// using namespace std;

// namespace L2
// {
//     void format_vector(const vector<set<string>> in, const vector<set<string>> out)
//     {
//         cout << "(" << endl;
//         cout << "(in" << endl;
//         for (auto i : in)
//         {
//             cout << "(";
//             for (auto j : i)
//             {
//                 cout << j << " ";
//             }
//             cout << ")" << endl;
//             ;
//         }
//         cout << ")" << endl
//              << endl;
//         cout << "(out" << endl;
//         for (auto i : out)
//         {
//             cout << "(";
//             for (auto j : i)
//             {
//                 cout << j << " ";
//             }
//             cout << ")" << endl;
//             ;
//         }
//         cout << ")" << endl
//              << endl;
//         cout << ")" << endl;
//     }

//     void print_vector(const vector<set<string>> v)
//     {
//         for (auto i : v)
//         {
//             cout << "{ ";
//             for (auto j : i)
//             {
//                 cout << j << ",";
//             }
//             cout << "}";
//             cout << endl;
//         }
//     }

//     vector<int> get_successor(vector<Instruction *> &instructions, int idx)
//     {
//         Instruction *i = instructions[idx];
//         Instruction_goto *a = dynamic_cast<Instruction_goto *>(i);
//         if (a != nullptr)
//         {
//             vector<int> successors;
//             Label *label = a->label;
//             int index = 0;
//             while (index < instructions.size())
//             {
//                 Instruction *curr = instructions[index];
//                 class Instruction_label *currLabel = dynamic_cast<class Instruction_label *>(curr);
//                 if (currLabel != nullptr && currLabel->label == label)
//                 {
//                     successors.push_back(index);
//                 }
//                 index++;
//             }
//             return successors;
//         }
//         // two successors
//         Instruction_cjump *b = dynamic_cast<Instruction_cjump *>(i);
//         if (b != nullptr)
//         {
//             vector<int> successors;
//             Label *label = b->label;
//             // int index = find(instructions.begin(), instructions.end(), i) - instructions.begin() + 1;
//             successors.push_back(idx + 1);
//             int index = 0;
//             while (index < instructions.size())
//             {
//                 Instruction *curr = instructions[index];
//                 class Instruction_label *currLabel = dynamic_cast<class Instruction_label *>(curr);
//                 if (currLabel != nullptr && currLabel->label== label)
//                 {
//                     successors.push_back(index);
//                 }
//                 index++;
//             }
//             return successors;
//         }
//         // no successor
//         Instruction_ret *c = dynamic_cast<Instruction_ret *>(i);
//         if (c != nullptr)
//         {
//             return {};
//         }
//         Instruction_call_error *d = dynamic_cast<Instruction_call_error *>(i);
//         if (d != nullptr)
//         {
//             return {};
//         }
//         // one successor
//         if (idx == instructions.size() - 1)
//         {
//             return {};
//         }
//         return {idx + 1};
//     }

//     void print(set<string> ss)
//     {
//         cout << "{";
//         for (auto s : ss)
//         {
//             cout << s << ", ";
//         }
//         cout << "}";
//         cout << endl;
//     }
//     void liveness(Program p)
//     {
//         auto f = p.functions[0];

//         vector<set<string>> gens;
//         vector<set<string>> kills;

//         for (auto i : f->instructions)
//         {
//             // cout << "ins: " << i->tostring() << endl;
//             auto gen = i->get_gen_set();
//             auto kill = i->get_kill_set();
//             set<string> gen_str;
//             set<string> kill_str;
//             // cout << "gens: ";
//             for (auto g : gen)
//             {
//                 gen_str.insert(g->toString());
//                 // cout << g->toString() << " ";
//             }
//             // cout << endl;
//             // cout << "kills: ";
//             gens.push_back(gen_str);
//             for (auto k : kill)
//             {
//                 kill_str.insert(k->toString());
//                 // cout << k->toString() << " ";
//             }
//             // cout << endl;
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
//         format_vector(in, out);
//     }
// }
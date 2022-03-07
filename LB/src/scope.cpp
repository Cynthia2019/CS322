#include "scope.h"
using namespace std;

namespace LB
{
    string findLongestName(Function *f) {
        size_t maxlen = 0;
        string LV = "";
        for (auto v : f->arguments) {
            string s = v->toString();
            if (s.length() > maxlen) {
                maxlen = s.length();
                LV = s;
            }
        }

        for (auto i : f->instructions) {
            auto declare = dynamic_cast<Instruction_declare *>(i);
            if (declare) {
                for (auto v : declare->declared) {
                    string s = v->toString();
                    if (s.length() > maxlen) {
                        maxlen = s.length();
                        LV = s;
                    }
                }
            }
        }

        return LV;
    }

    void unscope(Function *f) {
        string LV = findLongestName(f);
        cout << "longest variable: " << LV << endl;
        Scope *prev = nullptr;
        int64_t counter = 0;
        unordered_map<Scope *, int64_t> scopeid;
        for (auto i : f->instructions) {
            // cout << i->toString() << endl;
            int64_t id = 0;
            if (i->scope != prev) {
                prev = i->scope;
                if (scopeid.count(i->scope) == 0) {
                    id = counter;
                    scopeid[i->scope] = counter;
                    counter++;
                } else {
                    id = scopeid[i->scope];
                }
                cout << "new scope: " << id << endl;
            } else {
                id = scopeid[i->scope];
            }

            auto declare = dynamic_cast<Instruction_declare *>(i);
            if (!declare) continue;
            for (auto v : declare->declared) {
                auto old_name = v->get();
                v->setName(LV + "_" + old_name + "_" + to_string(id));
            }
        }
    }
    
} // namespace LB

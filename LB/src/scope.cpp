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
        cout << LV << endl;
    }
    
} // namespace LB

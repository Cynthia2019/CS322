#include <L3.h>

namespace L3 {

    class Context{
        public:
        void inline add(Instruction *i) { instructions.push_back(i);};
        bool inline isEmpty() { return instructions.size() == 0; }
        vector<Instruction *> instructions;
    };

    vector<Context *> identifyContext(Function *f);
}
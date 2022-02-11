#include <instructionSelect.h>

namespace L3 {
   vector<Context *> identifyContext(Function *f) {
        vector<Context *> context_list;
        Context *context = new Context();

        for (auto i : f->instructions) {
            Instruction_label *l = dynamic_cast<Instruction_label *>(i);
            Instruction_call *c = dynamic_cast<Instruction_call *>(i);
            
            if (l == nullptr && c == nullptr) {
                context->add(i);
            }

            Instruction_br *b = dynamic_cast<Instruction_br *>(i);
            Instruction_ret *r = dynamic_cast<Instruction_ret *>(i);
            if (l || c || b || r) {
                context_list.push_back(context);
                context = new Context();
            }
        }

        
        vector<Context *> context_list_noempty;
        for (auto c: context_list) {
            if (!c->isEmpty()) {
                context_list_noempty.push_back(c);
            }
        }

        return context_list_noempty;
    }
}

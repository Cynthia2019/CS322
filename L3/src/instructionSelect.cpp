#include <instructionSelect.h>
#include <L3.h>

using namespace std; 

namespace L3 {
    void Tree::visit(Instruction_ret_not* i) {
        this->i = i; 
        TreeNode* node = new TreeNode(i->op);
    }
    void Tree::visit(Instruction_ret_t* i) {
        this->i = i; 
        TreeNode* node = new TreeNode(i->op);
        node->oprand1 = new TreeNode(i->arg); 
    }
    void Tree::visit(Instruction_assignment* i) {
        this->i = i; 
        TreeNode* node = new TreeNode(i->dst);
        node->oprand1 = new TreeNode(i->src); 
    }

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
                if (!context->isEmpty()) {
                    context_list.push_back(context);
                }
                context = new Context();
            }
        }

        return context_list;
    }
}
#include <instructionSelect.h>
#include <L3.h>

using namespace std; 

namespace L3 {
    void Tree::visit(Instruction_ret* i) {
        this->i = i; 
        TreeNode* node = new TreeNode();
        node->op = i->op; 
        
    }
}
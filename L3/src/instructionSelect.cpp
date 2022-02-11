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
}
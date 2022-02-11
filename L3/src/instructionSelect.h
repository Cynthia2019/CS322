#pragma once

using namespace std;
namespace L3 {
    struct TreeNode {
        Item* val; 
        Operation* op;
        TreeNode* left; 
        TreeNode* right; 
    };
class Tree : public Visitor{
    public:
        TreeNode* root; 
        Tree() {root = nullptr}; 
        bool isEmpty() {return root == nullptr;};

        void visit(Instruction_ret *i) override;
        void visit(Instruction_ret_t *i) override;
        void visit(Instruction_assignment *i) override;
        void visit(Instruction_load *i) override;
        void visit(Instruction_matn *i) override;
        void visit(Instruction_store *i) override;
        void visit(Instruction_compare *i) override;
        void visit(Instruction_br *i) override;
        void visit(Instruction_br_t *i) override;
        private: 
            Instruction* i; 
    }
}
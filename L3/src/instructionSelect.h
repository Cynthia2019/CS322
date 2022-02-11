#pragma once
#include <string>
#include <L3.h>
using namespace std;
namespace L3 {
    class TreeNode {
        public: 
        TreeNode(Item* item); 
        Item* val; 
        Operation* op;
        TreeNode* oprand1; 
        TreeNode* oprand2; 
    };
class Tree : public Visitor{
    public:
        TreeNode* root; 
        Tree() {root = nullptr;}; 
        bool isEmpty() {return root == nullptr;};

        void visit(Instruction_ret_not *i) override;
        void visit(Instruction_ret_t *i) override;
        void visit(Instruction_assignment *i) override;
        void visit(Instruction_load *i) override;
        void visit(Instruction_math *i) override;
        void visit(Instruction_store *i) override;
        void visit(Instruction_compare *i) override;
        void visit(Instruction_br_label *i) override;
        void visit(Instruction_br_t *i) override;
        private: 
            Instruction* i; 
    };
}
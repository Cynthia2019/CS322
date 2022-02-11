#include <instructionSelect.h>
#include <L3.h>

using namespace std; 

namespace L3 {
    TreeNode::TreeNode(Item* item) {
        val = item;
    }
    Tree::Tree(Context* context) : context(context) {};
    void Tree::visit(Instruction_ret_not* i) {
        TreeNode* node = new TreeNode(i->op);
        root = node;
    }
    void Tree::visit(Instruction_ret_t* i) {
         
        TreeNode* node = new TreeNode(i->op);
        node->oprand1 = new TreeNode(i->arg); 
        root = node;
        uses.push_back(i->arg);
    }
    void Tree::visit(Instruction_assignment* i) {
         
        TreeNode* node = new TreeNode(i->dst);
        node->oprand1 = new TreeNode(i->src); 
        root = node;
        uses.push_back(i->src); 
    }
    void Tree::visit(Instruction_math* i) {
         
        TreeNode* node = new TreeNode(i->dst); 
        node->oprand1 = new TreeNode(i->oprand1); 
        node->op = dynamic_cast<Operation*>(i->op); 
        node->oprand2 = new TreeNode(i->oprand2);
        root = node;
        uses.push_back(i->oprand1);
        uses.push_back(i->oprand2);
    }
    void Tree::visit(Instruction_compare* i) {
         
        TreeNode* node = new TreeNode(i->dst); 
        node->oprand1 = new TreeNode(i->oprand1); 
        node->op = dynamic_cast<Operation*>(i->op); 
        node->oprand2 = new TreeNode(i->oprand2);
        root = node;
        uses.push_back(i->oprand1);
        uses.push_back(i->oprand2);
    }
    void Tree::visit(Instruction_load* i) {
         
        TreeNode* node = new TreeNode(i->dst); 
        node->oprand1 = new TreeNode(i->src); 
        node->op = dynamic_cast<Operation*>(i->op); 
        root = node;
        uses.push_back(i->src);
    }
    void Tree::visit(Instruction_store* i) {
         
        TreeNode* node = new TreeNode(i->dst); 
        node->oprand1 = new TreeNode(i->src); 
        node->op = dynamic_cast<Operation*>(i->op);  
        root = node;
        uses.push_back(i->src);
        uses.push_back(i->dst);
    }
    void Tree::visit(Instruction_br_t* i) {
         
        TreeNode* node = new TreeNode(i->op); 
        node->oprand1 = new TreeNode(i->condition); 
        node->oprand2 = new TreeNode(i->label); 
        root = node;
        uses.push_back(i->condition);
        uses.push_back(i->label);
    }
    void Tree::visit(Instruction_br_label* i) {
         
        TreeNode* node = new TreeNode(i->op); 
        node->oprand1 = new TreeNode(i->label); 
        root = node;
        uses.push_back(i->label);
    }
    void Tree::visit(Instruction_call_noassign *i) {};
    void Tree::visit(Instruction_call_assignment *i) {};
    void Tree::visit(Instruction_label *i) {};

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
    bool checkUseV(int a, int b, Variable* v, vector<Tree*>& trees) {
        for(int i = a; i <= b; i++){
            Tree* t = trees[i]; 
            for(Item* item : t->uses){
                Variable* u = dynamic_cast<Variable*>(item); 
                if(u != nullptr && u == v){
                    return true; 
                }
            }
        }
        return false; 
    }
    bool checkT2Uses(int a, int b, vector<Item*> uses, vector<Tree*>& trees){
        
    }
    vector<Tree*> getAllTree(Context* context){
        vector<Tree*> trees; 
        for(auto i : context->instructions) {
            Tree t{context}; 
            i->accept(&t);
        }
        return trees;
    }
    void mergeTrees(Context* context){
        int64_t size = context->instructions.size(); 
        vector<Tree*> trees = getAllTree(context); 
        for(int i = 0; i < size; i++){
            Tree* T2 = trees[i];
            Variable* v = dynamic_cast<Variable*>(T2->root->val); 
            //current root is not a variable
            //instruction must be return, br, return t, br t, skip
            if(v != nullptr){
                for(int j = i + 1; j < size; j++){
                    Tree* T1 = trees[j]; 
                    //condition 1: t_next uses t_curr root
                    if(T1->root->oprand1 && T1->root->oprand1->val == T2->root->val){
                        //check no other uses of T2->root
                        bool cond1 = checkUseV(i+1, j, v, trees);
                        T1->root->oprand1 = T2->root; 
                    }
                    //condition 1: t_next uses t_curr root
                    else if(T1->root->oprand1 && T1->root->oprand2->val == T2->root->val) {
                        //check no other uses of T2->root
                        bool cond1 = checkUseV(i+1, j, v, trees);
                        T1->root->oprand2 = T2->root; 
                    }
                    else continue;
                }
            }
        }

    }
}
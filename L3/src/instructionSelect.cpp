#include "instructionSelect.h"
#include "L3.h"
#include "liveness.h"
#include <string> 
#include <iostream>
#include <algorithm>
#include <tiling.h>
using namespace std; 

extern bool is_debug;

namespace L3 {
    TreeNode::TreeNode(Item* item) {
        val = item;
    }
    void TreeNode::printNode(TreeNode* node){
        if(node == nullptr) return ;
        if(node->val == nullptr) cout << "bug" << endl;
        cout << node->val->toString() << endl;
        if(node->op != nullptr) cout << node->op->toString() << endl; 
        if(node->oprand1 != nullptr) printNode(node->oprand1) ; 
        if (node->oprand2 != nullptr) printNode(node->oprand2);
    }
    Tree::Tree(Instruction* i) {
        this->instruction = i;
    }
    Instruction* Tree::getInstruction() {
        return this->instruction;
    }
    void Tree::printTree(Tree* tree){
        if(tree->root == nullptr) return; 
        tree->root->printNode(tree->root); 
    }
    void Tree::visit(Instruction_ret_not* i) {
        TreeNode* node = new TreeNode(i->op);
        root = node;
        uses = i->uses; 
        define = i->define;
    }
    void Tree::visit(Instruction_ret_t* i) {
         
        TreeNode* node = new TreeNode(i->op);
        node->oprand1 = new TreeNode(i->arg); 
        root = node;
        uses = i->uses; 
        define = i->define;
    }
    void Tree::visit(Instruction_assignment* i) {
         
        TreeNode* node = new TreeNode(i->dst);
        node->oprand1 = new TreeNode(i->src); 
        root = node;
        uses = i->uses; 
        define = i->define;
    }
    void Tree::visit(Instruction_math* i) {
         
        TreeNode* node = new TreeNode(i->dst); 
        node->oprand1 = new TreeNode(i->oprand1); 
        node->op = dynamic_cast<Operation*>(i->op); 
        node->oprand2 = new TreeNode(i->oprand2);
        root = node;
        uses = i->uses; 
        define = i->define;
    }
    void Tree::visit(Instruction_compare* i) {
         
        TreeNode* node = new TreeNode(i->dst); 
        node->oprand1 = new TreeNode(i->oprand1); 
        node->op = dynamic_cast<Operation*>(i->op); 
        node->oprand2 = new TreeNode(i->oprand2);
        root = node;
        uses = i->uses; 
        define = i->define;
    }
    void Tree::visit(Instruction_load* i) {
         
        TreeNode* node = new TreeNode(i->dst); 
        node->oprand1 = new TreeNode(i->src); 
        node->op = dynamic_cast<Operation*>(i->op); 
        root = node;
        uses = i->uses; 
        define = i->define;
    }
    void Tree::visit(Instruction_store* i) {
         
        TreeNode* node = new TreeNode(i->dst); 
        node->oprand1 = new TreeNode(i->src); 
        node->op = dynamic_cast<Operation*>(i->op);  
        root = node;
        uses = i->uses; 
        define = i->define;
    }
    void Tree::visit(Instruction_br_t* i) {
         
        TreeNode* node = new TreeNode(i->op); 
        node->oprand1 = new TreeNode(i->condition); 
        node->oprand2 = new TreeNode(i->label); 
        root = node;
        uses = i->uses; 
        define = i->define;
    }
    void Tree::visit(Instruction_br_label* i) {
         
        TreeNode* node = new TreeNode(i->op); 
        node->oprand1 = new TreeNode(i->label); 
        root = node;
        uses = i->uses; 
        define = i->define;
    }
    void Tree::visit(Instruction_call_noassign *i) {}
    void Tree::visit(Instruction_call_assignment *i) {}
    void Tree::visit(Instruction_label *i) {}

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
    //check if there is no definitions of variables used by T2 between T2 and T1
    bool checkDependency(int a, int b, Tree* T2, Variable* v, vector<Tree*>& trees){
        //between [T2,T1)
        for(int i = a; i < b; i++){
            Tree* t = trees[i]; 
            //check v not used
            for(Item* item : t->uses){
                Variable* u = dynamic_cast<Variable*>(item); 
                if(u != nullptr && u == v){
                    return false; 
                }
            }
            //check used var in T2 not defined between T2 and T1 
            for(Item* item : T2->uses){
                Variable* v_used = dynamic_cast<Variable*>(item); 
                //if v is not a variable, no need to compare, skip 
                if(v_used != nullptr) {
                    for(Item* d : t->define) {
                        Variable* u = dynamic_cast<Variable*>(d); 
                        if(u != nullptr && u == v_used) return false; 
                    }
                }
            }
        }
        return true; 
    }
    bool checkMemoryInst(int a, int b, vector<Instruction*> insts){
        for(int i = a; i <= b; i++){
            Instruction* inst = insts[i];
            Instruction_load* load = dynamic_cast<Instruction_load*>(inst); 
            if(load != nullptr) return false; 
            Instruction_store* store = dynamic_cast<Instruction_store*>(inst);
            if(store != nullptr) return false; 
        }
        return true;
    }
    bool checkDead(int a, Item* v, AnalysisResult* res, vector<Tree*> trees){
        for(int i = a; i < trees.size(); i++){
            Tree* t = trees[i]; 
            set<Item*> out = res->outs[t->getInstruction()]; 
            if(out.find(v) != out.end()){
                return false; 
            }
        }
        return true;
    }
    bool checkOnlyUsedByT1(int a, Item*v, vector<Instruction*> insts){
        for(int i = a; i < insts.size(); i++){
            vector<Item*> uses = insts[i]->uses; 
            if(find(uses.begin(), uses.end(), v) != uses.end()) return false; 
        }
        return true;
    }
    vector<Tree*> getAllTree(Context* context){
        vector<Tree*> trees; 
        for(auto i : context->instructions) {
            Tree* t = new Tree(i); 
            i->accept(t);
            // cout << "tree: " << endl; 
            // t->printTree(t);
            trees.push_back(t);
        }
        return trees;
    }
    void mergeTrees(Context* context, AnalysisResult* res){
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
                    if(is_debug) {
                        cout << "T2: " << T2->getInstruction()->toString() << endl;
                        cout << "T1: " << T1->getInstruction()->toString() << endl;
                    }
                    //condition 1: T1 uses T2 root
                    if(T1->root->oprand1 && T1->root->oprand1->val == T2->root->val){
                        cout << "match" << endl;
                        //condition A, T2->root->val dead after T1
                        bool dead = checkDead(j+1, T2->root->val, res, trees);
                        bool only = checkOnlyUsedByT1(j+1, T2->root->val, context->instructions);
                        if(dead || only) {
                            //condition B
                            bool notDependent = checkDependency(i, j, T2, v, trees); 
                            //T1 defines v
                            if(T1->root->val == T2->root->val) notDependent = false;
                            //another oprand in T1 uses v 
                            if(T1->root->oprand2 && T1->root->oprand2->val == T2->root->val) notDependent = false;
                            bool noMemoryInst = checkMemoryInst(i+1, j-1, context->instructions);
                            if(notDependent && noMemoryInst){
                                //merge
                                T1->root->oprand1 = T2->root; 
                            }
                        }
                    }
                    //condition 1: t_next uses t_curr root
                    else if(T1->root->oprand2 && T1->root->oprand2->val == T2->root->val) {
                        //condition A, T2->root->val dead after T1
                        bool dead = checkDead(j+1, T2->root->val, res, trees);
                        bool only = checkOnlyUsedByT1(j+1, T2->root->val, context->instructions);
                        if(dead || only) {
                            //condition B
                            bool notDependent = checkDependency(i, j, T2, v, trees); 
                            //T1 defines v
                            if(T1->root->val == T2->root->val) notDependent = false;
                            //another oprand in T1 uses v 
                            if(T1->root->oprand1->val == T2->root->val) notDependent = false;
                            bool noMemoryInst = checkMemoryInst(i+1, j-1, context->instructions);
                            if(notDependent && noMemoryInst){
                                //merge
                                T1->root->oprand2 = T2->root; 
                            }
                        }
                    }
                    else continue;
                }
            }
        }

        vector<Tile *> alltiles;
        Tile_math *tile = new Tile_math("*");
        alltiles.push_back(tile);
        tile = new Tile_math("+");
        alltiles.push_back(tile);
        cout << "tree after merge: " << endl;
        vector<TreeNode *> subtrees;
        for(Tree* t : trees){
            vector<Tile *> tiled;
            cout << "new tree: " << endl ;
            t->printTree(t);
            tiling(t->root, tiled, alltiles);
            // cout << tiled.size() << endl;
        }
    }
    void instructionSelection(Program p, Function* f){        
        //perform liveness analysis on instructions
        GenKill genkill; 
        for(Instruction* i : f->instructions) {
            i->accept(&genkill);
        }
        AnalysisResult* res = computeLiveness(f);
        vector<Context*> ctx = identifyContext(f); 
        for(Context* c : ctx){
            cout << "merge trees in a context" << endl;
            mergeTrees(c, res); 
        }
    }
}
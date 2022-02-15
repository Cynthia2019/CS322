#include <codegenerator.h>
#include <L3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace std;

extern bool is_debug; 
namespace L3{

    CodeGen::CodeGen(/*Function *f*/) {}

    void CodeGen::visit(Tile_return* t){
        TreeNode *tree = t->getTree();
        L2_instructions.push_back("\treturn\n");
    }
    void CodeGen::visit(Tile_return_t* t){
        TreeNode *tree = t->getTree();
        string line = "\t" + tree->oprand1->val->toString() + " <- rdi\n";
        L2_instructions.push_back(line);
        line = "\trax <- " + tree->oprand1->val->toString() + "\n";
        L2_instructions.push_back(line);
        L2_instructions.push_back("\treturn\n");
    }
    void CodeGen::visit(Tile_assign* t){
        TreeNode *tree = t->getTree();
        string line = "\t" + tree->val->toString() + " <- " + tree->oprand1->val->toString() + '\n';
        L2_instructions.push_back(line);
    }

    void CodeGen::visit(Tile_math* t) {
        cout << "translating tile_math" << endl;
        TreeNode *tree = t->getTree();
        if (!tree) cout << "bug " << endl;
        string dst = tree->val->toString(); 
        string oprand1 = tree->oprand1->val->toString();
        string op = tree->op->toString(); 
        string oprand2 = tree->oprand2->val->toString();
        string line = '\t' + dst + " <- " + oprand1 + '\n'; 
        L2_instructions.push_back(line);
        line = '\t' + dst + " " + op + "= " + oprand2 + '\n';
        L2_instructions.push_back(line);
        cout << "end translating tile_math" << endl;
    }
    void CodeGen::visit(Tile_math_specialized* t) {
        TreeNode *tree = t->getTree();
        string dst = tree->val->toString(); 
        string oprand1 = tree->oprand1->val->toString();
        string op = tree->op->toString(); 
        string oprand2 = tree->oprand2->val->toString();
        string line;
        if(dst == oprand1){
            line = '\t' + dst + " " + op + "= " + oprand1 + '\n';   
        }
        else {
            line = '\t' + dst + " " + op + "= " + oprand2 + '\n';   
        }
        L2_instructions.push_back(line);
    }

    void CodeGen::visit(Tile_compare *t){
        TreeNode *tree = t->getTree();
        string dst = tree->val->toString(); 
        string oprand1 = tree->oprand1->val->toString();
        string op = tree->op->toString(); 
        string oprand2 = tree->oprand2->val->toString();
        string line = '\t' + dst + " <- " + oprand1 + '\n'; 
        L2_instructions.push_back(line);
        line = '\t' + dst + " " + op + "= " + oprand2 + '\n';
        L2_instructions.push_back(line);
    }

    void CodeGen::visit(Tile_load *t){
        TreeNode *tree = t->getTree();
        string dst = tree->val->toString(); 
        string oprand1 = tree->oprand1->val->toString();
        //need to know current number of item on stack 
        // string M = to_string(this->f->sizeOfStack * 8);
        // string line = '\t' + dst + " <- mem " + oprand1 + " " + M + '\n'; 
        string line = '\t' + dst + " <- mem " + oprand1 + " 0\n"; 
        L2_instructions.push_back(line);
    }
    void CodeGen::visit(Tile_store *t){
        TreeNode *tree = t->getTree();
        string dst = tree->val->toString(); 
        string oprand1 = tree->oprand1->val->toString();
        //need to know current number of item on stack 
        string line = "\t mem" + dst + " 0 <- " + oprand1 + '\n'; 
        L2_instructions.push_back(line);
    }
    void CodeGen::visit(Tile_br* t){
        TreeNode *tree = t->getTree();
        string label = tree->oprand1->val->toString(); 
        string line = "\t goto " + label + '\n'; 
        L2_instructions.push_back(line);
    }
    //TODO might need fix
    void CodeGen::visit(Tile_br_t* t){
        TreeNode *tree = t->getTree();
        string label = tree->oprand2->val->toString(); 
        Item* condition = tree->oprand1->val;
        Number* n = dynamic_cast<Number*>(condition); 
        string line; 
        if(n != nullptr){
            line = "\t cjump 1 = 1 " + label + '\n'; 
        }
        else {
            //condition is a variable, find the two nodes in the tree that define this variable
            line = "\t cjump " + condition->toString() + " = 1 " + label + "\n";   
        }
        L2_instructions.push_back(line);
    }

    void CodeGen::visit(Tile_increment* t){
        TreeNode *tree = t->getTree();
        string dst = tree->val->toString(); 
        string line; 
        if(tree->op->toString() == "+"){
            line = "\t" + dst + "++";
        }
        else {
            line = "\t" + dst + "--";
        }
        L2_instructions.push_back(line);
    }

    void CodeGen::visit(Tile_at* t){
        TreeNode *tree = t->getTree();
        string dst = tree->val->toString(); 
        string src_add = tree->oprand2->val->toString(); 
        string src_mult = tree->oprand1->oprand1->val->toString(); 
        string src_const = tree->oprand1->oprand2->val->toString(); 
        string line = "\t" + dst + " @ " + src_add + " " + src_mult + " " + src_const + "\n";
        L2_instructions.push_back(line);       
    }

    void generate_code(Program p){
        /* 
        * Open the output file.
        */ 
        // std::ofstream outputFile;
        // outputFile.open("prog.L2");
        // outputFile << "(:main" << endl;
        // for(Function* f : p.functions){
        //     //initialize function setup 
        //     outputFile << "(" << f->name << endl; 
        //     outputFile << "\t" << f->arguments.size() << endl;
        //     //step 1: instruction selection

        //     outputFile << ")\n"; 

        // }
        // outputFile << ")\n" << endl;
        return;
    }
}
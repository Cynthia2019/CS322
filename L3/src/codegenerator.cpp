#include <codegenerator.h>
#include <L3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <globalize.h>
#include <instructionSelect.h>

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
        Variable* arg = dynamic_cast<Variable*>(tree->val); 
        string line;
        if(arg){
            line = "\t" + tree->oprand1->val->toString() + " <- rdi\n";
            L2_instructions.push_back(line);
        }
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
        string line;
        // string line = '\t' + dst + " <- " + oprand1 + '\n'; 
        // L2_instructions.push_back(line);
        // line = '\t' + dst + " " + op + "= " + oprand2 + '\n';
        // L2_instructions.push_back(line);
        if(op == ">="){
            line = '\t' + dst + " <- " + oprand2 + " <= " + oprand1 +  '\n'; 
        }
        else if(op == ">"){
            line = '\t' + dst + " <- " + oprand2 + " < " + oprand1 +  '\n'; 
        }
        else {
            line = '\t' + dst + " <- " + oprand1 + " " + op + " " + oprand2 +  '\n'; 
        }
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
        string line = "\tmem" + dst + " 0 <- " + oprand1 + '\n'; 
        L2_instructions.push_back(line);
    }
    void CodeGen::visit(Tile_br* t){
        TreeNode *tree = t->getTree();
        string label = tree->oprand1->val->toString(); 
        string line = "\tgoto " + label + '\n'; 
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
            if(n->get() == 0) line = "\tcjump 0 = 1 " + label + '\n'; 
            else line = "\tcjump 1 = 1 " + label + '\n'; 
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
            line = "\t" + dst + "++\n";
        }
        else {
            line = "\t" + dst + "--\n";
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
       //globalize labels
        L3::labelGlobalize(&p);
        std::ofstream outputFile;
        outputFile.open("prog.L2");
        outputFile << "(:main" << endl;  
        for(Function* f : p.functions){
            //step 1: instruction selection
            vector<string> L2_instructions = L3::instructionSelection(p, f);
            //initialize function setup 
            outputFile << "\t(" << f->name << endl; 
            outputFile << "\t\t" << f->arguments.size() << endl;
            //step 2: output L2 instructions
            for(string s : L2_instructions){
                outputFile << "\t" << s; 
            }
            outputFile << "\t)\n"; 

        }
        outputFile << ")\n" << endl;
        return;
    }
}
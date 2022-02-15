#include <codegenerator.h>
#include <L3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace std;

extern bool is_debug; 
namespace L3{

    CodeGen::CodeGen(Function *f, Tree* tree) : 
    tree(tree) {}

    void CodeGen::visit(Tile_return* t){
        this->tree->L2_instructions.push_back("\treturn\n");
    }
    void CodeGen::visit(Tile_return_t* t){
        string line = "\t" + this->tree->root->oprand1->val->toString() + " <- rdi\n";
        this->tree->L2_instructions.push_back(line);
        line = "\trax <- " + this->tree->root->oprand1->val->toString() + "\n";
        this->tree->L2_instructions.push_back(line);
        this->tree->L2_instructions.push_back("\treturn\n");
    }
    void CodeGen::visit(Tile_assign* t){
        string line = "\t" + this->tree->root->val->toString() + " <- " + this->tree->root->oprand1->val->toString() + '\n';
        this->tree->L2_instructions.push_back(line);
    }

    void CodeGen::visit(Tile_math* t) {
        string dst = this->tree->root->val->toString(); 
        string oprand1 = this->tree->root->oprand1->val->toString();
        string op = this->tree->root->op->toString(); 
        string oprand2 = this->tree->root->oprand2->val->toString();
        string line = '\t' + dst + " <- " + oprand1 + '\n'; 
        this->tree->L2_instructions.push_back(line);
        line = '\t' + dst + " " + op + "= " + oprand2 + '\n';
        this->tree->L2_instructions.push_back(line);
    }
    void CodeGen::visit(Tile_math_specialized* t) {
        string dst = this->tree->root->val->toString(); 
        string oprand1 = this->tree->root->oprand1->val->toString();
        string op = this->tree->root->op->toString(); 
        string oprand2 = this->tree->root->oprand2->val->toString();
        string line;
        if(dst == oprand1){
            line = '\t' + dst + " " + op + "= " + oprand1 + '\n';   
        }
        else {
            line = '\t' + dst + " " + op + "= " + oprand2 + '\n';   
        }
        this->tree->L2_instructions.push_back(line);
    }
    void CodeGen::visit(Tile_load *t){
        string dst = this->tree->root->val->toString(); 
        string oprand1 = this->tree->root->oprand1->val->toString();
        //need to know current number of item on stack 
        // string M = to_string(this->f->sizeOfStack * 8);
        // string line = '\t' + dst + " <- mem " + oprand1 + " " + M + '\n'; 
        string line = '\t' + dst + " <- mem " + oprand1 + " 0\n"; 
        this->tree->L2_instructions.push_back(line);
    }
    void CodeGen::visit(Tile_store *t){
        string dst = this->tree->root->val->toString(); 
        string oprand1 = this->tree->root->oprand1->val->toString();
        //need to know current number of item on stack 
        string line = "\t mem" + dst + " 0 <- " + oprand1 + '\n'; 
        this->tree->L2_instructions.push_back(line);
    }
    void CodeGen::visit(Tile_br* t){
        string label = this->tree->root->oprand1->val->toString(); 
        string line = "\t goto " + label + '\n'; 
        this->tree->L2_instructions.push_back(line);
    }
    //TODO might need fix
    void CodeGen::visit(Tile_br_t* t){
        string label = this->tree->root->oprand2->val->toString(); 
        Item* condition = this->tree->root->oprand1->val;
        Number* n = dynamic_cast<Number*>(condition); 
        string line; 
        if(n != nullptr){
            line = "\t cjump 1 = 1 " + label + '\n'; 
        }
        else {
            //condition is a variable, find the two nodes in the tree that define this variable
            line = "\t cjump " + condition->toString() + " = 1 " + label + "\n";   
        }
        this->tree->L2_instructions.push_back(line);
    }

    void CodeGen::visit(Tile_increment* t){
        string dst = this->tree->root->val->toString(); 
        string line; 
        if(this->tree->root->op->toString() == "+"){
            line = dst + "++";
        }
        else {
            line = dst + "--";
        }
        this->tree->L2_instructions.push_back(line);
    }

    void CodeGen::visit(Tile_at* t){
        string dst = this->tree->root->val->toString(); 
        string src_add = this->tree->root->oprand2->val->toString(); 
        string src_mult = this->tree->root->oprand1->oprand1->val->toString(); 
        string src_const = this->tree->root->oprand1->oprand2->val->toString(); 
        string line = "\t" + dst + " @ " + src_add + " " + src_mult + " " + src_const + "\n";
        this->tree->L2_instructions.push_back(line);       
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
#include <codegenerator.h>
#include <L3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace std;

extern bool is_debug; 

namespace L3 {
    CodeGen::CodeGen(Function *f, Tree* tree, std::ofstream &outputFile) : 
    tree(tree), outputFile(outputFile) {}

    void CodeGen::visit(Tile_return* t){
        this->outputFile << "\treturn\n";
    }
    void CodeGen::visit(Tile_return_t* t){
        string line = "\t" + this->tree->root->oprand1->val->toString() + " <- rdi\n";
        this->outputFile << line;
        line = "\trax <- " + this->tree->root->oprand1->val->toString() + "\n";
        this->outputFile << line; 
        this->outputFile << "\treturn\n";
    }
    void CodeGen::visit(Tile_assign* t){
        string line = "\t" + this->tree->root->val->toString() + " <- " + this->tree->root->oprand1->val->toString() + '\n';
        this->outputFile << line;
    }

    void CodeGen::visit(Tile_math* t) {
        string dst = this->tree->root->val->toString(); 
        string oprand1 = this->tree->root->oprand1->val->toString();
        string op = this->tree->root->op->toString(); 
        string oprand2 = this->tree->root->oprand2->val->toString();
        string line = '\t' + dst + " <- " + oprand1 + '\n'; 
        this->outputFile << line; 
        line = '\t' + dst + " " + op + "= " + oprand2 + '\n';
        this->outputFile << line; 
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
        this->outputFile << line; 
    }
    void CodeGen::visit(Tile_load *t){
        string dst = this->tree->root->val->toString(); 
        string oprand1 = this->tree->root->oprand1->val->toString();
        //need to know current number of item on stack 
        // string M = to_string(this->f->sizeOfStack * 8);
        // string line = '\t' + dst + " <- mem " + oprand1 + " " + M + '\n'; 
        string line = '\t' + dst + " <- mem " + oprand1 + " 0\n"; 
        this->outputFile << line; 
    }
    void CodeGen::visit(Tile_store *t){
        string dst = this->tree->root->val->toString(); 
        string oprand1 = this->tree->root->oprand1->val->toString();
        //need to know current number of item on stack 
        string M = to_string(this->f->sizeOfStack * 8);
        string line = "\t mem" + dst + " 0 <- " + oprand1 + '\n'; 
        this->outputFile << line; 
    }
    void CodeGen::visit(Tile_br* t){
        string label = this->tree->root->oprand1->val->toString(); 
        string line = "\t goto " + label + '\n'; 
        this->outputFile << line; 
    }
    void CodeGen::visit(Tile_br_t* t){
        string label = this->tree->root->oprand2->val->toString(); 
        string condition = this->tree->root->oprand2->val->toString();
        string line = "\t cjump " + condition  + label + '\n'; 
        this->outputFile << line; 
    }

    void generate_code(Program p){

    }
}
#include <codegenerator.h>
#include <L3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace std;

extern bool is_debug; 

namespace L3 {
    CodeGen::CodeGen(Tree* tree, std::ofstream &outputFile) : 
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

    }

    void generate_code(Program p){

    }
}
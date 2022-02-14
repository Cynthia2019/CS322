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
        outputFile << "return\n";
    }
    void CodeGen::visit(Tile_return_t* t){
        outputFile << this->tree->root->oprand1->val->toString() << " <- rdi\n";
    }
    void generate_code(Program p){

    }
}
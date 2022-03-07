#pragma once
#include <LB.h>

using namespace std; 
namespace LB {
    struct WhileLoop {
        Instruction* i; 
        Label* begin; 
        Label* end; 
        Label* cond; 
    };
    map<Instruction*, WhileLoop*> processLoop(Program& p, Function* f); 
}
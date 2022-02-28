#pragma once 
#include <LA.h> 

using namespace std;

namespace LA {
    vector<Item*> toDecode(Instruction* i); 
    vector<Item*> toEncode(Instruction* i); 
}
#include <string>
#include <iostream>
#include <fstream>

#include <liveness.h>

using namespace std;

namespace L2 {
    void liveness(Program p) {
        auto f = p.functions[0]; 
        for(auto i : f->instructions) {
            cout << "instructions: " << i->tostring() << endl; 
        }
    }
}
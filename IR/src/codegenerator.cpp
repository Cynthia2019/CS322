#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <codegenerator.h>

using namespace std;

extern bool is_debug; 

namespace IR {

    int count = 0;

    void CodeGenerator::visit(Instruction_ret_not *i) { }
    void CodeGenerator::visit(Instruction_ret_t *i) { }
    void CodeGenerator::visit(Instruction_assignment *i) { 
        // VarTypes dstType = i->dst->getVariableType(); 
      //  VarTypes srcType = i->src->getVariableType();
        // string s; 
        // if(dstType == var_tuple && srcType == var_tuple) {
        //     string varNameDst = "newVar" + to_string(count); 
        //     s = "\t" + varNameDst + " <- " + i->dst->toString() + " + 8\n"; 
        //     count++; 
        //     string varNameSrc = "newVar" + toString(count);
        //     s += "\t" + varNameSrc + " <- " + i->src->toString() + " 8\n"; 
        //     s += "\tstore " + varNameDst + " <- " + i->dst->toString(); 
        //     outputFile << s; 
        // }
        // else if(dstType == var_tuple) {
        //     string varName = "newVar" + to_string(count); 
        //     s = "\t" + varName + " <- " + i->dst->toString() + " + 8\n"; 
        //     s += "\tstore " + varName + " <- " + i->src->toString(); 
        //     outputFile << s; 
        //     count++; 
        // }

    }
    void CodeGenerator::visit(Instruction_load *i) {
        if(is_debug) cout << "visit load\n"; 
        
     }
    void CodeGenerator::visit(Instruction_op *i) { }
    void CodeGenerator::visit(Instruction_store *i) { }
    void CodeGenerator::visit(Instruction_declare *i) { }
    void CodeGenerator::visit(Instruction_br_label *i) { }
    void CodeGenerator::visit(Instruction_br_t *i) { }
    void CodeGenerator::visit(Instruction_call_noassign *i) { }
    void CodeGenerator::visit(Instruction_call_assignment *i) { }
    void CodeGenerator::visit(Instruction_label *i) { }
    void CodeGenerator::visit(Instruction_length *i) { }
    void CodeGenerator::visit(Instruction_array *i) { }
    void CodeGenerator::visit(Instruction_tuple *i) {
        if(is_debug) cout << "visit tuple\n"; 
        outputFile << "\t" << i->dst->toString() << "call allocate(" << i->arg->toString() << ", 1)\n"; 
     }


    void generate_code(Program p) {
        /* 
        * Open the output file.
        */ 
        std::ofstream outputFile;
        outputFile.open("prog.L3");
        // outputFile << "(" <<p.entryPointLabel << endl;
    }

}
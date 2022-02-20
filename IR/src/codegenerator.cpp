#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <codegenerator.h>

using namespace std;

extern bool is_debug; 

namespace IR {

      void CodeGenerator::visit(Instruction_ret_not *i) { }
      void CodeGenerator::visit(Instruction_ret_t *i) { }
      void CodeGenerator::visit(Instruction_assignment *i) { }
      void CodeGenerator::visit(Instruction_load *i) { }
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
      void CodeGenerator::visit(Instruction_tuple *i) { }


    void generate_code(Program p) {
        /* 
        * Open the output file.
        */ 
        std::ofstream outputFile;
        outputFile.open("prog.L3");
        // outputFile << "(" <<p.entryPointLabel << endl;
    }

}
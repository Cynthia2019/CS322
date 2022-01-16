#include <string>
#include <iostream>
#include <fstream>

#include <code_generator.h>

using namespace std;

namespace L1{
  void generate_code(Program p){

    /* 
     * Open the output file.
     */ 
    std::ofstream outputFile;
    outputFile.open("prog.S");
    /* 
     * Generate target code
     */ 
    //TODO
    //write pushq for registers
    outputFile << ".text\n"; 
    outputFile << " .globl go\n"; 
    outputFile << ".go: \n"; 
    outputFile << " pushq %rbx\n";
    outputFile << " pushq %rbp\n";
    outputFile << " pushq %r12\n";
    outputFile << " pushq %r13\n";
    outputFile << " pushq %r14\n";
    outputFile << " pushq %r15\n";
    std::string name = p.entryPointLabel; 
    name[0] = '_';
    cout << "programe name: " << name << endl; 
    outputFile << " call " + name + "\n"; 
    outputFile << " popq %r15\n";
    outputFile << " popq %r14\n";
    outputFile << " popq %r13\n";
    outputFile << " popq %r12\n";
    outputFile << " popq %rbp\n";
    outputFile << " popq %rbx\n";
    outputFile << " retq\n"; 

    for(auto f : p.functions) {
      std::string f_name = f->name; 
      f_name[0] = '_'; 
      outputFile << f_name + ":\n"; 
      std::string translated = "\t"; 
      for(Instruction* i : f->instructions){
        //if assignment
        if(i->instructionName == "assignment") {
          Instruction_assignment* a= static_cast<Instruction_assignment *>(i); 
          translated.append("movq "); 
          if(a->src.isARegister){
            translated += '%';
            translated.append(a->src.register_name);
          }
          else if(a->src.isAConstant) {
            translated += '$';
            translated.append(std::to_string(a->src.num)); 
          }
          else if(a->src.isALabel) {
            std::string func_name = a->src.labelName; 
            func_name[0] = '_';
            translated += '$';
            translated.append(func_name); 
          }
          translated.append(", %");
          translated.append(a->dst.register_name);
          translated += '\n'; 
          cout << "current result: " << translated << endl; 
        }
      }
      outputFile << translated; 
    }
    
    /* 
     * Close the output file.
     */ 
    outputFile.close();
   
    return ;
  }
}

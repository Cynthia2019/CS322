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
      outputFile << '_' + f->name + ":\n"; 
      for(Instruction* i : f->instructions){
        //if assignment
        cout << "instruction: " << i->instructionName << endl;
        //Instruction_assignment *a = dynamic_cast<Instruction_assignment *>(i);
        std::string translated = "\t\t"; 
        // if(a != nullptr) {
        //   translated.append("movq "); 
        //   if(a->isARegister){
        //     translated.append('%' + a->r + ',');
             
        //   }
        // }
      }
    }
    
    /* 
     * Close the output file.
     */ 
    outputFile.close();
   
    return ;
  }
}

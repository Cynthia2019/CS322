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
      for(Instruction* i : f->instructions){
        std::string translated = " "; 
        //if assignment
        if(i->instructionName == "assignment") {
          Instruction_assignment* a= static_cast<Instruction_assignment *>(i); 
          translated += "movq "; 
          if(a->src.isARegister){
            translated += '%' + a->src.register_name;
          }
          else if(a->src.isAConstant) {
            translated += '$' + std::to_string(a->src.num);
          }
          else if(a->src.isALabel) {
            std::string func_name = a->src.labelName; 
            func_name[0] = '_';
            translated += '$' + func_name;
          }
          translated += ", %" + a->dst.register_name + '\n';
        }
        //if load
        else if(i->instructionName == "load"){
          Instruction_load* a = static_cast<Instruction_load*>(i); 
          translated += "movq " + std::to_string(a->constant.num) + "(%" + a->src.register_name + "), %" + a->dst.register_name + '\n';
        }
        //if store
        else if(i->instructionName == "store") {
          Instruction_store* a = static_cast<Instruction_store*>(i); 
          translated += "movq "; 
          if(a->src.isARegister){
            translated += '%' + a->src.register_name;
          }
          else if(a->src.isAConstant) {
            translated += std::to_string(a->src.num);
          }
          else if(a->src.isALabel) {
            std::string func_name = a->src.labelName; 
            func_name[0] = '_';
            translated += '$' + func_name;
          }
          translated += ", " + std::to_string(a->constant.num) + "(%" + a->dst.register_name + ")\n";
        }
        else if(i->instructionName == "aop") {
          Instruction_aop* a = static_cast<Instruction_aop*>(i); 
          if(a->op.op == "+=") {
            translated += "addq ";
          }
          else if(a->op.op == "-=") {
            translated += "subq ";
          }
          else if(a->op.op == "*=") {
            translated += "imulq ";
          }
          else {
            translated += "andq "; 
          }
          if(a->src.isARegister){
            translated += '%' + a->src.register_name;
          }
          else {
            translated += '$' + std::to_string(a->src.num);
          }
          translated += ", %" + a->dst.register_name + '\n';
        }
        else if(i->instructionName == "increment"){
          Instruction_increment* a = static_cast<Instruction_increment*>(i); 
          translated += "inc %" + a->src.register_name + '\n';
        }
        else if(i->instructionName == "decrement"){
          Instruction_decrement* a = static_cast<Instruction_decrement*>(i); 
          translated += "dec %" + a->src.register_name + '\n';
        }
        else if(i->instructionName == "load_aop"){
          Instruction_load_aop* a = static_cast<Instruction_load_aop*>(i); 
          if(a->op.op == "+="){
            translated += "addq "; 
          }
          else {
            translated += "subq ";
          }
          translated += std::to_string(a->constant.num) + "(%" + a->src.register_name + "), " + a->dst.register_name + '\n'; 
        }
        else if(i->instructionName == "store_aop"){
          Instruction_store_aop* a = static_cast<Instruction_store_aop*>(i); 
          if(a->op.op == "+="){
            translated += "addq "; 
          }
          else {
            translated += "subq ";
          }
          translated += '%' + a->src.register_name + ", " + std::to_string(a->constant.num) + "(%" + a->dst.register_name + ")\n";
        }
        else if(i->instructionName == "at"){
          Instruction_at*a = static_cast<Instruction_at*>(i); 
          translated += "lea (%" + a->src_add.register_name + ", " + a->src_mult.register_name + ", " + std::to_string(a->constant.num) + "), %" + a->dst.register_name + '\n';
        }

        outputFile << translated; 
      }
    }
    
    /* 
     * Close the output file.
     */ 
    outputFile.close();
   
    return ;
  }
}

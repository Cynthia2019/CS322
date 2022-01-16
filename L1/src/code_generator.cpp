#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

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

    
    unordered_map<string, string> reg_64_to_8 = {
      {"r10", "r10b"},
      {"r11", "r11b"},
      {"r12", "r12b"},
      {"r13", "r13b"},
      {"r14", "r14b"},
      {"r15", "r15b"},
      {"r8", "r8b"},
      {"r8", "r8b"},
      {"r9", "r9b"},
      {"rax", "al"},
      {"rbp", "bpl"},
      {"rbx", "bl"},
      {"rcx", "cl"},
      {"rdi", "dil"},
      {"rdx", "dl"},
      {"rsi", "sil"},
    };


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

        else if(i->instructionName == "compare") {
          Instruction_compare* com = static_cast<Instruction_compare *>(i);
          string dst = com->dst.register_name;
          string dst_8 = reg_64_to_8[dst];
          if (com->oprand1.isAConstant && com->oprand2.isAConstant) {
            if ((com->op.op == "<=" && com->oprand1.num <= com->oprand2.num)
              ||(com->op.op == "<" && com->oprand1.num < com->oprand2.num)
              ||(com->op.op == "=" && com->oprand1.num == com->oprand2.num)) {
                string mov = "movq $1, %";
                translated.append(mov + dst);
                translated.append("\n");
            } else {
                string mov = "movq $0, %";
                translated.append(mov + dst);
                translated.append("\n");
            }
          } else if (com->oprand1.isAConstant && com->oprand2.isARegister) {
              translated.append("cmpq $");
              translated.append(to_string(com->oprand1.num));
              translated.append(", %");
              translated.append(com->oprand2.register_name);
              translated.append("\n");
              if (com->op.op == "<=") {
                translated.append("setge %");
              }
              else if (com->op.op == "<") {
                translated.append("setg %"); //TODO: check this
              }
              else if (com->op.op == "=") {
                translated.append("sete %");
              }
              translated.append(dst_8);
              translated.append("\n");
              translated.append("movzbq %");
              translated.append(dst_8);
              translated.append(", %");
              translated.append(dst);
              translated.append("\n");
          } else {
            translated.append("cmpq ");
            if (com->oprand2.isAConstant) {
              translated.append("$");
              translated.append(to_string(com->oprand2.num));
            } else {
              translated.append("%");
              translated.append(com->oprand2.register_name);
            }
            translated.append(", %");
            translated.append(com->oprand1.register_name);
            translated.append("\n");
            if (com->op.op == "<=") {
              translated.append("setle %");
            }
            else if (com->op.op == "<") {
              translated.append("setl %"); //TODO: check this
            }
            else if (com->op.op == "=") {
              translated.append("sete %");
            }
            // translated.append("setle %");
            translated.append(dst_8);
            translated.append("\n");
            translated.append("movzbq %");
            translated.append(dst_8);
            translated.append(", %");
            translated.append(dst);
            translated.append("\n");
          }
        }

        else if(i->instructionName == "shift") {
          Instruction_shift* shi = static_cast<Instruction_shift *>(i);
          if (shi->op.op == "<<=") {
            translated.append("salq ");
          } else {
            translated.append("sarq ");
          }

          if (shi->src.isARegister) {
            translated.append("%");
            translated.append(reg_64_to_8[shi->src.register_name]);
          } else {
            translated.append("$");
            translated.append(to_string(shi->src.num));
          }

          translated.append(", %");
          translated.append(shi->dst.register_name);
          translated.append("\n");
        }
          
        else if(i->instructionName == "cjump") {
          Instruction_cjump* cmd = static_cast<Instruction_cjump *>(i);
          string label = cmd->label.labelName;
          if (label.length() <= 0) abort();
          label[0] = '_';
          if (cmd->oprand1.isAConstant && cmd->oprand2.isAConstant) {
            if ((cmd->op.op == "<=" && cmd->oprand1.num <= cmd->oprand2.num)
              ||(cmd->op.op == "<" && cmd->oprand1.num < cmd->oprand2.num)
              ||(cmd->op.op == "=" && cmd->oprand1.num == cmd->oprand2.num)) {
                translated.append("jmp ");
                translated.append(label);
                translated.append("\n");
            }
          }
          else if (cmd->oprand1.isAConstant && cmd->oprand2.isARegister) {
            translated.append("cmpq $");
            translated.append(to_string(cmd->oprand1.num));
            translated.append(", %");
            translated.append(cmd->oprand2.register_name);
            translated.append("\n");
            if (cmd->op.op == "<=") {
              translated.append("jge ");
            }
            else if (cmd->op.op == "<") {
              translated.append("jg ");
            }
            else if (cmd->op.op == "=") {
              translated.append("je ");
            }
            translated.append(label);
            translated.append("\n");
          }
          else {
            translated.append("cmpq ");
            if (cmd->oprand2.isARegister) {
              translated.append("%");
              translated.append(cmd->oprand2.register_name);
            } else {
              translated.append("$");
              translated.append(to_string(cmd->oprand2.num));
            }
            translated.append(", %");
            translated.append(cmd->oprand1.register_name);
            translated.append("\n");

            if (cmd->op.op == "<=") {
              translated.append("jle ");
            }
            else if (cmd->op.op == "<") {
              translated.append("jl ");
            }
            else if (cmd->op.op == "=") {
              translated.append("je ");
            }
            translated.append(label);
            translated.append("\n");
          } 
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

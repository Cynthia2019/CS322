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
    outputFile << "go: \n"; 
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
      //check if there are locals
      if(f->locals > 0 || f->arguments > 6){
        int64_t offset = (f->locals + max((int)f->arguments - 6, 0)) * 8; 
        std::string line = " subq $";
        line += to_string(offset) + ", %rsp\n";  
        outputFile << line; 
      }
      for(Instruction* i : f->instructions){
        std::string translated = ""; 
        //if assignment
        if(i->instructionName == "assignment") {
          Instruction_assignment* a= static_cast<Instruction_assignment *>(i); 
          translated += " movq "; 
          if(a->src.isARegister){
            translated += '%' + a->src.register_name;
          }
          else if(a->src.isAConstant) {
            translated += '$' + to_string(a->src.num);
          }
          else if(a->src.isALabel) {
            std::string func_name = a->src.labelName; 
            func_name[0] = '_';
            translated += '$' + func_name;
          }
          translated += ", %" + a->dst.register_name + '\n';
        }

        else if(i->instructionName == "compare") {
          Instruction_compare* com = static_cast<Instruction_compare *>(i);
          string dst = com->dst.register_name;
          string dst_8 = reg_64_to_8[dst];
          if (com->oprand1.isAConstant && com->oprand2.isAConstant) {
            if ((com->op.op == "<=" && com->oprand1.num <= com->oprand2.num)
              ||(com->op.op == "<" && com->oprand1.num < com->oprand2.num)
              ||(com->op.op == "=" && com->oprand1.num == com->oprand2.num)) {
                translated += " movq $1, %" + dst + '\n';
            } else {
                translated += " movq $0, %" + dst + '\n';
            }
          } else if (com->oprand1.isAConstant && com->oprand2.isARegister) {
              translated += " cmpq $" + to_string(com->oprand1.num) + ", %" + com->oprand2.register_name + '\n';
              if (com->op.op == "<=") {
                translated += " setge %";
              }
              else if (com->op.op == "<") {
                translated += " setg %"; //TODO: check this
              }
              else if (com->op.op == "=") {
                translated += " sete %";
              }
              translated += dst_8 + '\n' + " movzbq %" + dst_8 + ", %" + dst + '\n'; 
          } else {
            translated += " cmpq ";
            if (com->oprand2.isAConstant) {
              translated += "$" + to_string(com->oprand2.num);
            } else {
              translated += "%" + com->oprand2.register_name;
            }
            translated += ", %" + com->oprand1.register_name + '\n';
            if (com->op.op == "<=") {
              translated += " setle %"; 
            }
            else if (com->op.op == "<") {
              translated += " setl %"; //TODO: check this
            }
            else if (com->op.op == "=") {
              translated += " sete %";
            }
            // translated.append("setle %");
            translated += dst_8 + '\n' + " movzbq %" + dst_8 + ", %" + dst + '\n';
          }
        }

        else if(i->instructionName == "shift") {
          Instruction_shift* shi = static_cast<Instruction_shift *>(i);
          if (shi->op.op == "<<=") {
            translated += " salq ";
          } else {
            translated += " sarq ";
          }

          if (shi->src.isARegister) {
            translated += "%" + reg_64_to_8[shi->src.register_name];
          } else {
            translated += "$" + to_string(shi->src.num);
          }
          translated += ", %" + shi->dst.register_name + '\n'; 
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
                translated += "jmp " + label + '\n';
            }
          }
          else if (cmd->oprand1.isAConstant && cmd->oprand2.isARegister) {
            translated += " cmpq $" + to_string(cmd->oprand1.num) + ", %" + cmd->oprand2.register_name + '\n';
            if (cmd->op.op == "<=") {
              translated += " jge ";
            }
            else if (cmd->op.op == "<") {
              translated += " jg ";
            }
            else if (cmd->op.op == "=") {
              translated += " je ";
            }
            translated += label + '\n';
          }
          else {
            translated += " cmpq ";
            if (cmd->oprand2.isARegister) {
              translated += "%" + cmd->oprand2.register_name; 
            } else {
              translated += "$" + to_string(cmd->oprand2.num); 
            }
            translated += ", %" + cmd->oprand1.register_name + '\n'; 

            if (cmd->op.op == "<=") {
              translated += " jle ";
            }
            else if (cmd->op.op == "<") {
              translated += " jl ";
            }
            else if (cmd->op.op == "=") {
              translated += " je ";
            }
            translated += label + '\n';
          } 
        }

        //if load
        else if(i->instructionName == "load"){
          Instruction_load* a = static_cast<Instruction_load*>(i); 
          translated += " movq " + to_string(a->constant.num) + "(%" + a->src.register_name + "), %" + a->dst.register_name + '\n';
        }
        //if store
        else if(i->instructionName == "store") {
          Instruction_store* a = static_cast<Instruction_store*>(i); 
          translated += " movq "; 
          if(a->src.isARegister){
            translated += '%' + a->src.register_name;
          }
          else if(a->src.isAConstant) {
            translated += to_string(a->src.num);
          }
          else if(a->src.isALabel) {
            std::string func_name = a->src.labelName; 
            func_name[0] = '_';
            translated += '$' + func_name;
            // outputFile << "subq $8, %rsp\n"; 
          }
          translated += ", " + to_string(a->constant.num) + "(%" + a->dst.register_name + ")\n";
        }
        else if(i->instructionName == "aop") {
          Instruction_aop* a = static_cast<Instruction_aop*>(i); 
          if(a->op.op == "+=") {
            translated += " addq ";
          }
          else if(a->op.op == "-=") {
            translated += " subq ";
          }
          else if(a->op.op == "*=") {
            translated += " imulq ";
          }
          else {
            translated += " andq "; 
          }
          if(a->src.isARegister){
            translated += '%' + a->src.register_name;
          }
          else {
            translated += '$' + to_string(a->src.num);
          }
          translated += ", %" + a->dst.register_name + '\n';
        }
        else if(i->instructionName == "increment"){
          Instruction_increment* a = static_cast<Instruction_increment*>(i); 
          translated += " inc %" + a->src.register_name + '\n';
        }
        else if(i->instructionName == "decrement"){
          Instruction_decrement* a = static_cast<Instruction_decrement*>(i); 
          translated += " dec %" + a->src.register_name + '\n';
        }
        else if(i->instructionName == "load_aop"){
          Instruction_load_aop* a = static_cast<Instruction_load_aop*>(i); 
          if(a->op.op == "+="){
            translated += " addq "; 
          }
          else {
            translated += " subq ";
          }
          translated += to_string(a->constant.num) + "(%" + a->src.register_name + "), " + a->dst.register_name + '\n'; 
        }
        else if(i->instructionName == "store_aop"){
          Instruction_store_aop* a = static_cast<Instruction_store_aop*>(i); 
          if(a->op.op == "+="){
            translated += " addq "; 
          }
          else {
            translated += " subq ";
          }
          translated += '%' + a->src.register_name + ", " + to_string(a->constant.num) + "(%" + a->dst.register_name + ")\n";
        }
        else if(i->instructionName == "at"){
          Instruction_at* a = static_cast<Instruction_at*>(i); 
          translated += " lea (%" + a->src_add.register_name + ", " + a->src_mult.register_name + ", " + to_string(a->constant.num) + "), %" + a->dst.register_name + '\n';
        }
        else if(i->instructionName == "call") {
          Instruction_call* a = static_cast<Instruction_call*>(i);
          translated += " subq $"; 
          int64_t space = max((int)a->constant.num - 6, 0) * 8 + 8; 
          translated += to_string(space) + ", %rsp\n";
          outputFile << translated; 
          translated = " jmp "; 
          if(a->dst.isALabel){
            std::string name = a->dst.labelName; 
            name[0] = '_'; 
            translated += name + '\n'; 
          }
          else {
            std::string name = a->dst.register_name; 
            translated += "*%" + name + '\n'; 
          }
        }
        else if(i->instructionName == "call_print") {
          translated += " call print\n"; 
        }
        else if(i->instructionName == "call_input") {
          translated += " call input\n"; 
        }
        else if(i->instructionName == "call_allocate") {
          translated += " call allocate\n"; 
        }
        else if(i->instructionName == "call_error") {
          Instruction_call_error* a = static_cast<Instruction_call_error*>(i); 
          if(a->constant.num == 1){
            translated += " call array_tensor_error_null\n"; 
          }
          else if(a->constant.num == 3){
            translated += " call array_error\n";
          }
          else {
            translated += " call tensor_error\n"; 
          }
        }
        else if(i->instructionName == "goto"){
          Instruction_goto* a = static_cast<Instruction_goto*>(i); 
          string name = a->label.labelName; 
          name[0] = '_';
          translated += "jmp " + name + '\n';  
        }
        else if(i->instructionName == "label") {
          Instruction_label* a = static_cast<Instruction_label*>(i);
          string label = a->label.labelName; 
          label[0] = '_'; 
          translated += label + ':' + '\n'; 
        }
        outputFile << translated; 
      }
      //restore locals 
      if(f->locals > 0){
        int64_t offset = (f->locals + max((int)f->arguments - 6, 0)) * 8; 
        std::string restore = " addq $";
        restore += to_string(offset) + ", %rsp\n";  
        outputFile << restore; 
      }
      outputFile << " retq\n";
    }
    
    /* 
     * Close the output file.
     */ 
    outputFile.close();
   
    return ;
  }
}

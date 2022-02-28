#pragma once

#include <LA.h>

namespace LA{
    void generate_code(Program p);       
  class CodeGenerator : public Visitor {
    public: 
      CodeGenerator(Function* f, std::ofstream &); 
      void visit(Instruction_ret_not *i) override;
      void visit(Instruction_ret_t *i) override;
      void visit(Instruction_assignment *i) override;
      void visit(Instruction_load *i) override;
      void visit(Instruction_op *i) override;
      void visit(Instruction_store *i) override;
      void visit(Instruction_declare *i) override;
      void visit(Instruction_br_label *i) override;
      void visit(Instruction_br_t *i) override;
      void visit(Instruction_call_noassign *i) override;
      void visit(Instruction_call_assignment *i) override;
      void visit(Instruction_label *i) override;
      void visit(Instruction_length *i) override;
      void visit(Instruction_array *i) override;
      void visit(Instruction_tuple *i) override;
      std::string newVar(Variable* v); 
    private: 
      std::ofstream &outputFile;
      Function* f;
      int64_t counter = 0; 
  };

}


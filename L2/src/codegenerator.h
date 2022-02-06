#pragma once

#include <L2.h>
#include <coloring.h>

namespace L2{
    void generate_code(Program p);
  class CodeGenerator : public Visitor {
    public: 
      CodeGenerator(Function* f, std::ofstream &outputFile, Colorer* c); 
       void visit(Instruction_ret *i) override;
       void visit(Instruction_assignment *i) override;
       void visit(Instruction_load *i) override;
       void visit(Instruction_shift *i) override;
       void visit(Instruction_store *i) override;
       void visit(Instruction_stack *i) override;
       void visit(Instruction_aop *i) override;
       void visit(Instruction_store_aop *i) override;
       void visit(Instruction_load_aop *i) override;
       void visit(Instruction_compare *i) override;
       void visit(Instruction_cjump *i) override;
       void visit(Instruction_call *i) override;
       void visit(Instruction_call_print *i) override;
       void visit(Instruction_call_input *i) override;
       void visit(Instruction_call_allocate *i) override;
       void visit(Instruction_call_error *i) override;
       void visit(Instruction_label *i) override;
       void visit(Instruction_increment *i) override;
       void visit(Instruction_decrement *i) override;
       void visit(Instruction_at *i) override;
       void visit(Instruction_goto *i) override;
    private: 
      std::ofstream &outputFile;
      Function* f;
      Colorer* colorer;
  };

}

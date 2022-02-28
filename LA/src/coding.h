#pragma once 
#include <LA.h> 

using namespace std;

namespace LA {
    vector<Item*> toDecode(Instruction* i); 
    vector<Item*> toEncode(Instruction* i); 
    class Encoder : public Visitor {
    public: 
      Encoder(Function* f); 
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
    private: 
      Function* f;
  };
    void encodeAll(Program &p, Function* f); 
}
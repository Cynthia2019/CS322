#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <codegenerator.h>
#include <liveness.h>
#include <interference.h> 
#include <coloring.h>
#include <architecture.h>

using namespace std;

extern bool is_debug; 

namespace L2 {
    std::string fromItemToString(Item* item, Graph* graph){
        Variable* v = dynamic_cast<Variable*>(item); 
        if(is_debug) {
            cout << "Item: " << item->toString() << endl;
        }
        if(v == nullptr) {
            return item->toString();
        }
        Node* node = graph->nodes[v]; 
        Architecture::RegisterID rid = Architecture::fromColorToRegister(node->color);
        return Architecture::fromRegisterToString(rid);
    } 
    CodeGenerator::CodeGenerator(Function* f, std::ofstream &outputFile, Colorer* c) 
    : f(f), outputFile(outputFile), colorer(c) {};

    void CodeGenerator::visit(Instruction_ret *i) {
        this->outputFile << "\treturn" << endl;
    };
    void CodeGenerator::visit(Instruction_assignment *i) {
        string s = fromItemToString(i->src, colorer->getGraph()); 
        string d = fromItemToString(i->dst, colorer->getGraph()); 
        // if(is_debug) {
        //     cout << "src: " << s << " dst: " << d << endl;
        // }
        string ans = "\t" + d + " <- " + s + "\n";
        this->outputFile << ans << endl;
    };

       void CodeGenerator::visit(Instruction_load *i) {};
       void CodeGenerator::visit(Instruction_shift *i) {};
       void CodeGenerator::visit(Instruction_store *i) {};
       void CodeGenerator::visit(Instruction_stack *i) {};
       void CodeGenerator::visit(Instruction_aop *i) {};
       void CodeGenerator::visit(Instruction_store_aop *i) {};
       void CodeGenerator::visit(Instruction_load_aop *i) {};
       void CodeGenerator::visit(Instruction_compare *i) {};
       void CodeGenerator::visit(Instruction_cjump *i) {};
       void CodeGenerator::visit(Instruction_call *i) {};
       void CodeGenerator::visit(Instruction_call_print *i) {};
       void CodeGenerator::visit(Instruction_call_input *i) {};
       void CodeGenerator::visit(Instruction_call_allocate *i) {};
       void CodeGenerator::visit(Instruction_call_error *i) {};
       void CodeGenerator::visit(Instruction_label *i) {};
       void CodeGenerator::visit(Instruction_increment *i) {};
       void CodeGenerator::visit(Instruction_decrement *i) {};
       void CodeGenerator::visit(Instruction_at *i) {};
       void CodeGenerator::visit(Instruction_goto *i) {};

    void generate_code(Program p) {
        /* 
        * Open the output file.
        */ 
        std::ofstream outputFile;
        outputFile.open("prog.L1");

        for(Function* f : p.functions){
            liveness(p, f); 
            interference(p, f); 
            Colorer* colorer = new L2::Colorer(p, f);
            colorer->registerAllocate(f); 
            CodeGenerator CodeGen{f, outputFile, colorer};
            for(auto i : f->instructions){
                i->accept(&CodeGen);
            } 
            outputFile << "\n";
            return ;
        }
    }

}
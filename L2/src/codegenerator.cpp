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
        this->outputFile << "\treturn\n";
    };
    void CodeGenerator::visit(Instruction_assignment *i) {
        string s = fromItemToString(i->src, colorer->getGraph()); 
        string d = fromItemToString(i->dst, colorer->getGraph()); 
        string ans = "\t" + d + " <- " + s + "\n";
        this->outputFile << ans;
    };
    void CodeGenerator::visit(Instruction_label *i) {
        string label = fromItemToString(i->label, colorer->getGraph());
        string ans = "\t" + label + "\n";
        this->outputFile << ans;
    };

    void CodeGenerator::visit(Instruction_load *i) {
        string src = fromItemToString(i->src, colorer->getGraph()); 
        string dst = fromItemToString(i->dst, colorer->getGraph()); 
        string constant = fromItemToString(i->constant, colorer->getGraph());

        string ans = "\t" + dst + " <- mem " + src + " " + constant + "\n"; 
        this->outputFile << ans;
    };
    void CodeGenerator::visit(Instruction_shift *i) {
        string src = fromItemToString(i->src, colorer->getGraph()); 
        string dst = fromItemToString(i->dst, colorer->getGraph()); 
        string op = fromItemToString(i->op, colorer->getGraph());
        string ans = "\t" + dst + " " + op + " " + src + "\n"; 
        this->outputFile << ans;
    };
    void CodeGenerator::visit(Instruction_store *i) {
        string src = fromItemToString(i->src, colorer->getGraph()); 
        string dst = fromItemToString(i->dst, colorer->getGraph()); 
        string constant = fromItemToString(i->constant, colorer->getGraph());
        string ans = "\tmem " + dst + " " + constant + " <- " + src + "\n"; 
        this->outputFile << ans;
    };
    void CodeGenerator::visit(Instruction_stack *i) {
        string src = fromItemToString(i->src, colorer->getGraph()); 
        string dst = fromItemToString(i->dst, colorer->getGraph());       
    };
    void CodeGenerator::visit(Instruction_aop *i) {
        string src = fromItemToString(i->src, colorer->getGraph()); 
        string dst = fromItemToString(i->dst, colorer->getGraph()); 
        string op = fromItemToString(i->op, colorer->getGraph());
        string ans = "\t" + dst + " " + op + " " + src + "\n"; 
        this->outputFile << ans;
    };
       void CodeGenerator::visit(Instruction_store_aop *i) {
        string src = fromItemToString(i->src, colorer->getGraph()); 
        string dst = fromItemToString(i->dst, colorer->getGraph()); 
        string constant = fromItemToString(i->constant, colorer->getGraph());
        string op = fromItemToString(i->op, colorer->getGraph());
        string ans = "\tmem " + dst + " " + constant + " " + op + " " + src + "\n"; 
        this->outputFile << ans;
       };
       void CodeGenerator::visit(Instruction_load_aop *i) {
        string src = fromItemToString(i->src, colorer->getGraph()); 
        string dst = fromItemToString(i->dst, colorer->getGraph()); 
        string constant = fromItemToString(i->constant, colorer->getGraph());
        string op = fromItemToString(i->op, colorer->getGraph());
        string ans = "\t" + dst + " " + op + " mem " + src + " " + constant + "\n"; 
        this->outputFile << ans;
       };
       void CodeGenerator::visit(Instruction_compare *i) {
           string oprand1 = fromItemToString(i->oprand1, colorer->getGraph()); 
           string oprand2 = fromItemToString(i->oprand2, colorer->getGraph()); 
           string op = fromItemToString(i->op, colorer->getGraph()); 
           string dst = fromItemToString(i->dst, colorer->getGraph()); 
           string ans = "\t" + dst + " " + oprand1 + " " + op + " " + oprand2 + "\n";
            this->outputFile << ans;
       };
       void CodeGenerator::visit(Instruction_cjump *i) {
           string oprand1 = fromItemToString(i->oprand1, colorer->getGraph()); 
           string oprand2 = fromItemToString(i->oprand2, colorer->getGraph()); 
           string op = fromItemToString(i->op, colorer->getGraph()); 
           string label = fromItemToString(i->label, colorer->getGraph()); 
           string ans = "\tcjump " + oprand1 + " " + op + " " + oprand2 + " " + label + "\n";
            this->outputFile << ans;
       };
       void CodeGenerator::visit(Instruction_call *i) {
           string dst = fromItemToString(i->dst,colorer->getGraph()); 
           string constant = fromItemToString(i->constant, colorer->getGraph());
           string ans = "\tcall " + dst + " " + constant + '\n';
           this->outputFile << ans; 
       };
       void CodeGenerator::visit(Instruction_call_print *i) {
           this->outputFile << "\tcall print 1\n";
       };
       void CodeGenerator::visit(Instruction_call_input *i) {
            this->outputFile << "\tcall input 0\n";
       };
       void CodeGenerator::visit(Instruction_call_allocate *i) {
            this->outputFile << "\tcall allocate 2\n";
       };
       void CodeGenerator::visit(Instruction_call_error *i) {
           string constant = fromItemToString(i->constant, colorer->getGraph()); 
            this->outputFile << "\tcall tensor-error "<< constant << " \n";
       };
       void CodeGenerator::visit(Instruction_increment *i) {
            string src = fromItemToString(i->src,colorer->getGraph()); 
            this->outputFile << "\t " << src << "++\n";
       };
       void CodeGenerator::visit(Instruction_decrement *i) {
            string src = fromItemToString(i->src,colorer->getGraph()); 
            this->outputFile << "\t " << src << "--\n";
       };
       void CodeGenerator::visit(Instruction_at *i) {
           string constant = fromItemToString(i->constant, colorer->getGraph()); 
           string src_mult = fromItemToString(i->src_mult,colorer->getGraph()); 
           string src_add = fromItemToString(i->src_add, colorer->getGraph()); 
           string dst = fromItemToString(i->dst, colorer->getGraph()); 
           string ans = "\t" + dst + " @ " + src_add + " " + src_mult + " " + constant + "\n";
           this->outputFile << ans;
       };
       void CodeGenerator::visit(Instruction_goto *i) {
           string label = fromItemToString(i->label, colorer->getGraph()); 
           this->outputFile << "\tgoto " + label + "\n"; 
       };

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
            outputFile << "(" << f->name << endl;
            for(auto i : f->instructions){
                i->accept(&CodeGen);
            } 
            outputFile << ")\n";
            return ;
        }
    }

}
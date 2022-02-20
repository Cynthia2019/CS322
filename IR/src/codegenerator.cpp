#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include "codegenerator.h"

using namespace std;

extern bool is_debug; 

namespace IR {
    std::string decode(Item *v) {
        return v->toString() + " <- " + v->toString() + ">> 1\n";
    }

    std::string decode(string v) {
        return v + " <- " + v + " >> 1\n";
    }

    std::string encode(Item *v) {
        ::string res = v->toString() + " <- " + v->toString() + " << 1\n";
        res += "\t" + v->toString() + " <- " + v->toString() + " + 1\n";
        return res;
    }

    std::string encode(string s) {
        ::string res = s + " <- " + s + " << 1\n";
        res += "\t" + s + " <- " + s + " + 1\n";
        return res;
    }

    CodeGenerator::CodeGenerator(Function *f, std::ofstream &ofs) :f(f), outputFile(ofs) {}

    std::string CodeGenerator::newTempVar() {
        ::string res =  "%temp_" + to_string(temp_counter);
        temp_counter++;
        return res;
    }

    void CodeGenerator::visit(Instruction_ret_not *i) { }
    void CodeGenerator::visit(Instruction_ret_t *i) { }
    void CodeGenerator::visit(Instruction_assignment *i) { }
    void CodeGenerator::visit(Instruction_load *i) { }
    void CodeGenerator::visit(Instruction_op *i) { }
    void CodeGenerator::visit(Instruction_store *i) { }
    void CodeGenerator::visit(Instruction_declare *i) { }
    void CodeGenerator::visit(Instruction_br_label *i) { }
    void CodeGenerator::visit(Instruction_br_t *i) { }
    void CodeGenerator::visit(Instruction_call_noassign *i) { }
    void CodeGenerator::visit(Instruction_call_assignment *i) { }
    void CodeGenerator::visit(Instruction_label *i) { }
    void CodeGenerator::visit(Instruction_length *i) { }

    void CodeGenerator::visit(Instruction_array *i) {
        int64_t dimension = i->args.size();
        vector<int64_t> lens;
        int64_t total_len = 0;
        int64_t allocate_len = 0;
        for (auto a : i->args) {
            Number *n = dynamic_cast<Number *>(a);
            if (!n) cerr << "something's wrong" << endl;
            lens.push_back(n->get());
            total_len += n->get();
        }

        allocate_len = total_len + 1 + dimension;
        ::string line;

        ::string total_len_var = newTempVar();
        outputFile << "\t" << total_len_var << " <- 1\n";
        for (auto a : i->args) {
            ::string temp = newTempVar();
            outputFile << "\t" << temp + " <- " + a->toString() + "\n";
            outputFile << "\t" << decode(temp);
            outputFile << "\t" << total_len_var + " <- " + total_len_var + " * " + temp << endl;
        }
        outputFile << "\t" << total_len_var << " <- " << total_len_var << " + " << (dimension + 1) << endl;

        outputFile << "\t" << encode(total_len_var);

        ::string array_ptr_t = newTempVar();
        outputFile << "\t" << array_ptr_t << " <- " << "call allocate(" << total_len_var << ", 1)" << endl;

        ::string dimension_ptr = newTempVar();
        int64_t dimension_encoded = dimension << 1;
        dimension_encoded += 1;
        outputFile << "\t" << dimension_ptr << " <- " << array_ptr_t << " + 8" << endl;
        outputFile << "\t" << "store " << dimension_ptr << " <- " << dimension_encoded << endl;
        
        for (int k = 0; k < i->args.size(); k++) {
            ::string tmp_ptr = newTempVar();
            int64_t offset = 16 + k * 8;
            outputFile << "\t" << tmp_ptr << " <- " << array_ptr_t << " + " << offset << endl;
            outputFile << "\t" << "store " << tmp_ptr << " <- " << i->args[k]->toString() << endl;
        }
    }

    void CodeGenerator::visit(Instruction_tuple *i) { }


    void generate_code(Program p) {
        /* 
        * Open the output file.
        */ 
        std::ofstream outputFile;
        outputFile.open("prog.L3");
        for (auto f : p.functions) {
            CodeGenerator cg(f, outputFile);
            outputFile << "define :" << f->name << "(";
            for (int i = 0; i < f->arguments.size(); i++) {
                outputFile << f->arguments[i]->toString();
                if (i != f->arguments.size() - 1) {
                    outputFile << ", ";
                }
            }
            outputFile << ") {" << endl;
            //TODO: chage this to use trace
            for (auto bb : f->basicBlocks) {
                for (auto i : bb->instructions) {
                    i->accept(&cg);
                }
            }

            outputFile << "}" << endl;
        }
    }

}
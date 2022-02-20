#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
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

    void CodeGenerator::visit(Instruction_ret_not *i) {
        outputFile << "\treturn" << endl;
    }
    void CodeGenerator::visit(Instruction_ret_t *i) {
    }
    void CodeGenerator::visit(Instruction_assignment *i) { }
    void CodeGenerator::visit(Instruction_load *i) {
        ArrayVar *array = dynamic_cast<ArrayVar *>(i->src);
        if (!array) {
            cerr << "reading from non-array variable " << i->src->toString() << endl;
            abort();
        }

        vector<Item *> indices = i->indices;
        reverse(indices.begin(), indices.end());
        ::string total_offset = newTempVar();
        outputFile << "\t" << total_offset << " <- 0" << endl;
        for (int k = 0; k < i->indices.size(); k++) {
            ::string dim_size_ptr = newTempVar();
            ::string dim_size = newTempVar();
            int64_t off = 16 + (k + 1) * 8;
            outputFile << "\t" << total_offset << " <- " << total_offset << " + " << indices[k]->toString() << endl;
            if (k == i->indices.size() - 1) break;
            outputFile << "\t" << dim_size_ptr << " <- " << array->toString() << " + " << off << endl;
            outputFile << "\t" << dim_size << " <- load " << dim_size_ptr << endl;
            outputFile << decode(dim_size);
            outputFile << "\t" << total_offset << " <- " << total_offset << " * " << dim_size << endl;
        }
        int64_t offset_base = 16 + array->dimension * 8;
        outputFile << "\t" << total_offset << " <- " << total_offset << " * 8" << endl;
        outputFile << "\t" << total_offset << " <- " << total_offset << " + " << offset_base << endl;
        outputFile << "\t" << total_offset << " <- " << total_offset << " + " << array->toString() << endl;

        outputFile << "\t" << i->dst->toString() << " <- load " << total_offset << endl;
    }

    void CodeGenerator::visit(Instruction_op *i) { }
    void CodeGenerator::visit(Instruction_store *i) {
        ArrayVar *array = dynamic_cast<ArrayVar *>(i->dst);
        if (!array) {
            cerr << "accessing non-array variable " << i->src->toString() << endl;
            abort();
        }

        vector<Item *> indices = i->indices;
        reverse(indices.begin(), indices.end());
        ::string total_offset = newTempVar();
        outputFile << "\t" << total_offset << " <- 0" << endl;
        for (int k = 0; k < i->indices.size(); k++) {
            ::string dim_size_ptr = newTempVar();
            ::string dim_size = newTempVar();
            int64_t off = 16 + (k + 1) * 8;
            outputFile << "\t" << total_offset << " <- " << total_offset << " + " << indices[k]->toString() << endl;
            if (k == i->indices.size() - 1) break;
            outputFile << "\t" << dim_size_ptr << " <- " << array->toString() << " + " << off << endl;
            outputFile << "\t" << dim_size << " <- load " << dim_size_ptr << endl;
            outputFile << decode(dim_size);
            outputFile << "\t" << total_offset << " <- " << total_offset << " * " << dim_size << endl;
        }
        int64_t offset_base = 16 + array->dimension * 8;
        outputFile << "\t" << total_offset << " <- " << total_offset << " * 8" << endl;
        outputFile << "\t" << total_offset << " <- " << total_offset << " + " << offset_base << endl;
        outputFile << "\t" << total_offset << " <- " << total_offset << " + " << array->toString() << endl;

        outputFile << "\t" << "store " << total_offset << " <- " << i->src->toString() << endl;
    }
    void CodeGenerator::visit(Instruction_declare *i) { }
    void CodeGenerator::visit(Instruction_br_label *i) { }
    void CodeGenerator::visit(Instruction_br_t *i) { }
    void CodeGenerator::visit(Instruction_call_noassign *i) {
        outputFile << "\t" << i->toString() << endl;
    }
    void CodeGenerator::visit(Instruction_call_assignment *i) { }
    void CodeGenerator::visit(Instruction_label *i) { }
    void CodeGenerator::visit(Instruction_length *i) {
        ArrayVar *a = dynamic_cast<ArrayVar *>(i->src);
        if (!a) {
            cerr << "reading lengh of a variable " << i->src->toString() << " which is not an array" << endl;
            abort();
        }
        ::string offset = newTempVar();
        ::string nb_dim = newTempVar();
        outputFile <<"\t" << offset << " <- 16" << endl; 
        outputFile << "\t" << nb_dim << " <- 8 * " << i->dimID->toString() << endl; 
        outputFile << "\t" << offset << " <- " << offset << " + " << nb_dim << endl; 
        ::string tmp_ptr = newTempVar();
        outputFile << "\t" << tmp_ptr << " <- " << i->src->toString() << " + " << offset << endl;
        outputFile <<"\t" <<  i->dst->toString() << " <- load " << tmp_ptr << endl;
     }

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

        ::string array_ptr_t = i->dst->toString();
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
        // outputFile << i->dst->toString() << " <- " << array_ptr_t
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
            outputFile << "define " << f->name << "(";
            for (int i = 0; i < f->arguments.size(); i++) {
                outputFile << f->arguments[i]->toString();
                if (i != f->arguments.size() - 1) {
                    outputFile << ", ";
                }
            }
            outputFile << ") {" << endl;
            //TODO: chage this to use trace
            for (auto bb : f->basicBlocks) {
                outputFile << "\t" << bb->label->get() << endl;
                for (auto i : bb->instructions) {
                    i->accept(&cg);
                }
                bb->te->accept(&cg);
            }

            outputFile << "}" << endl;
        }
    }

}
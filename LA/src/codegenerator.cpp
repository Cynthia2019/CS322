#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <LA.h> 
#include <codegenerator.h> 
#include <coding.h>

using namespace std; 

extern bool is_debug; 


namespace LA {
    std::string decode(Variable *v) {
        string s; 
        return v->toString() + "_new"; 
    }
    std::string decode(Number *v) {
        int64_t n = v->get(); 
        n >>= 1; 
        return to_string(n); 
    }

    // std::string decode(string v) {
    //     return v + " <- " + v + " >> 1\n";
    // }

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

    void CodeGenerator::visit(Instruction_ret_not *i) {
        outputFile << "\treturn" << endl;
    }

    void CodeGenerator::visit(Instruction_ret_t *i) {
        outputFile << "\treturn"  << i->arg->toString() << endl;
    }

    void CodeGenerator::visit(Instruction_assignment *i) {
        outputFile << "\t" << i->toString() << endl;
    }

    void CodeGenerator::visit(Instruction_load* i) {
        vector<Item*> todecode = toDecode(i); 
        vector<string> decoded; 
        for(Item* item : todecode) {
            Variable* v = dynamic_cast<Variable*>(item); 
            if(v != nullptr) {
                decoded.push_back(decode(v)); 
                outputFile << "\tint64 " << v->toString() << "_new\n";  
                outputFile << "\t" << v->toString() << "_new" << " <- " << v->toString() << ">> 1\n";
            }
            else {
                decoded.push_back(decode(dynamic_cast<Number*>(item))); 
            }
        }
        string s; 
        s += "\t" + i->dst->toString() + " <- " + i->src->toString(); 
        for(Item* item : i->indices) {
            if(item->getType() == item_variable) {
                s += "[" + decoded[0] + "]"; 
                decoded.erase(decoded.begin()); 
            }
            else {
                s += "[" + item->toString() + "]"; 
            }
        }
        s += '\n'; 
        outputFile << s;
    }

    void generate_code(Program p) {
        /* 
        * Open the output file.
        */ 

        std::ofstream outputFile;
        outputFile.open("prog.IR");
        for (auto f : p.functions) {
            CodeGenerator cg(f, outputFile);
            outputFile << "define " << f->name << "(";
            for (int i = f->arguments.size() - 1; i >= 0; i--) {
                outputFile << f->arguments[i]->toString();
                if (i != 0) {
                    outputFile << ", ";
                }
            }
            outputFile << ") {" << endl;
            // outputFile << "\t" << LL << "_to_entry" << endl;
            outputFile << "\t" << "br " << f->entry_label << endl;
            for(Instruction* i : f->instructions) {
                i->accept(&cg); 
            }
            outputFile << "}" << endl;
        }
    }
}
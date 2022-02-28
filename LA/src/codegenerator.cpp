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
    // std::string decode(Variable *v, CodeGenerator& g) {
    //     string s; 
    //     return g.newVar(v); 
    // }
    // std::string decode(Number *v, CodeGenerator& g) {
    //     int64_t n = v->get(); 
    //     n >>= 1; 
    //     return to_string(n); 
    // }

    // std::string decode(string v) {
    //     return v + " <- " + v + " >> 1\n";
    // }

    std::string encode(Variable *v) {
        ::string res = "\t" + v->toString() + " <- " + v->toString() + " << 1\n";
        res += "\t" + v->toString() + " <- " + v->toString() + " + 1\n";
        return res;
    }

    // std::string encode(string s) {
    //     ::string res = s + " <- " + s + " << 1\n";
    //     res += "\t" + s + " <- " + s + " + 1\n";
    //     return res;
    // }
    CodeGenerator::CodeGenerator(Function *f, std::ofstream &ofs) :f(f), outputFile(ofs) {}

    std::string CodeGenerator::newVar(Variable* v) {
        string s = v->toString() + "_new_" + to_string(counter); 
        counter++; 
        return s; 
    }

    void CodeGenerator::visit(Instruction_ret_not *i) {
        outputFile << "\treturn" << endl;
    }

    void CodeGenerator::visit(Instruction_ret_t *i) {
        outputFile << "\treturn "  << i->arg->toString() << endl;
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
                string t = newVar(v); 
                decoded.push_back(t); 
                outputFile << "\tint64 " << t << "\n";  
                outputFile << "\t" << t << " <- " << v->toString() << " >> 1\n";
            }
            else {
                Number* n = dynamic_cast<Number*>(item); 
                string t = to_string(n->get() >> 1) ;
                decoded.push_back(t); 
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


    void CodeGenerator::visit(Instruction_store* i) {
        vector<Item*> todecode = toDecode(i); 
        vector<string> decoded; 
        for(Item* item : todecode) {
            Variable* v = dynamic_cast<Variable*>(item); 
            if(v != nullptr) {
                string t = newVar(v);  
                outputFile << "\tint64 " << t << "\n";  
                outputFile << "\t" << t << " <- " << v->toString() << " >> 1\n";
                decoded.push_back(t);
            }
            else {
                Number* n = dynamic_cast<Number*>(item); 
                string t = to_string(n->get() >> 1) ;
                decoded.push_back(t); 
            }
        }
        string s; 
        s += "\t" + i->dst->toString(); 
        for(Item* item : i->indices) {
            if(item->getType() == item_variable) {
                s += "[" + decoded[0] + "]"; 
                decoded.erase(decoded.begin()); 
            }
            else {
                s += "[" + item->toString() + "]"; 
            }
        }
        s += i->src->toString() + '\n'; 
        outputFile << s;
    }


    void CodeGenerator::visit(Instruction_br_t* i) {
        Item* todecode = toDecode(i)[0]; 
        std::string decoded; 
        Variable* v = dynamic_cast<Variable*>(todecode); 
            if(v != nullptr) {
                decoded = newVar(v); 
                outputFile << "\tint64 " << decoded << "\n";  
                outputFile << "\t" << decoded << " <- " << v->toString() << " >> 1\n";
            }
            else {
                Number* n = dynamic_cast<Number*>(todecode); 
                decoded = to_string(n->get() >> 1) ;
            }
        string s = "\tbr " + decoded + " " + i->label1->toString() + " " + i->label2->toString() + '\n'; 
        outputFile << s;
    }

    void CodeGenerator::visit(Instruction_length* i) {
        Item* todecode = toDecode(i)[0]; 
        std::string decoded; 
        Variable* v = dynamic_cast<Variable*>(todecode); 
            if(v != nullptr) {
                decoded = newVar(v); 
                outputFile << "\tint64 " << decoded << "\n";  
                outputFile << "\t" << decoded << " <- " << v->toString() << " >> 1\n";
            }
            else {
                Number* n = dynamic_cast<Number*>(todecode); 
                decoded = to_string(n->get() >> 1) ;
            }
        string s = "\t" + i->dst->toString() + " <- length " + i->src->toString() + " " + decoded + '\n'; 
        outputFile << s;
    }
    void CodeGenerator::visit(Instruction_op* i) {
        // if(is_debug) cout << "codegen: op " << i->toString() << endl;
        Item* todecode1 = toDecode(i)[0]; 
        Item* todecode2 = toDecode(i)[1]; 
        std::string decoded1; 
        std::string decoded2; 
        Variable* v1 = dynamic_cast<Variable*>(todecode1); 
        if(v1 != nullptr) {
            decoded1 = newVar(v1); 
            outputFile << "\tint64 " << decoded1 << "\n";  
            outputFile << "\t" << decoded1 << " <- " << v1->toString() << " >> 1\n";
        }
        else {
            Number* n = dynamic_cast<Number*>(todecode1); 
            decoded1 = to_string(n->get() >> 1) ;
        }
        Variable* v2 = dynamic_cast<Variable*>(todecode2); 
        if(v2 != nullptr) {
            decoded2 = newVar(v2); 
            outputFile << "\tint64 " << decoded2 << "\n";  
            outputFile << "\t" << decoded2 << " <- " << v2->toString() << " >> 1\n";
        }
        else {
            Number* n = dynamic_cast<Number*>(todecode2); 
            decoded2 = to_string(n->get() >> 1) ;
        }
        string s = "\t" + i->dst->toString() + " <- " + decoded1 + " " + i->op->toString() + " " + decoded2 + '\n';
        s += encode(i->dst);  
        outputFile << s;
    }

    void CodeGenerator::visit(Instruction_declare *i) {} ;
    void CodeGenerator::visit(Instruction_br_label *i) {} ;
    void CodeGenerator::visit(Instruction_call_noassign *i) {} ;
    void CodeGenerator::visit(Instruction_call_assignment *i) {} ;
    void CodeGenerator::visit(Instruction_label *i) {} ;
    void CodeGenerator::visit(Instruction_array *i) {} ;
    void CodeGenerator::visit(Instruction_tuple *i) {} ;
    void CodeGenerator::visit(Instruction_print *i) {} ; 
    void CodeGenerator::visit(Instruction_input *i) {} ; 

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
            // outputFile << "\t" << "br " << f->entry_label << endl;
            for(Instruction* i : f->instructions) {
                i->accept(&cg); 
            }
            outputFile << "}" << endl;
        }
    }
}
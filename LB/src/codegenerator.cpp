#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <LB.h> 
#include <codegenerator.h> 

using namespace std; 

extern bool is_debug; 


namespace LB {

    CodeGenerator::CodeGenerator(Function *f, map<Instruction*, WhileLoop*>& loop, std::ofstream &ofs) : 
    f(f), loop(loop), outputFile(ofs) {}

    // std::string CodeGenerator::newVar(Variable* v) {
    //     string s = v->toString() + "_new_" + to_string(counter); 
    //     counter++; 
    //     return s; 
    // }
    std::string CodeGenerator::newVar() {
        string s = this->f->longest_name + "_new_" + to_string(counter); 
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
        string dst = i->dst->toString(); 
        string src; 
        if(i->src->getType() == item_function) {
            src = ":";
        }
        src += i->src->toString(); 
        outputFile << "\t" << dst << " <- " << src << endl;
    }

    void CodeGenerator::visit(Instruction_load* i) {
        string s; 
        s += "\t" + i->dst->toString() + " <- " + i->src->toString(); 
        for(Item* item : i->indices) {
            s += "[" + item->toString() + "]";      
        }
        s += '\n'; 
        outputFile << s;
    }


    void CodeGenerator::visit(Instruction_store* i) {
        string s; 
        s += "\t" + i->dst->toString(); 
        for(Item* item : i->indices) {
            s += "[" + item->toString() + "]"; 
        }
        s += " <- ";
        s += i->src->toString() + '\n'; 
        outputFile << s;
    }

    void CodeGenerator::visit(Instruction_length* i) {
        string s = "\t" + i->dst->toString() + " <- length " + i->src->toString() + " " + i->dimID->toString() + '\n';
        outputFile << s; 
    }
    void CodeGenerator::visit(Instruction_op* i) {
        string s = "\t" + i->dst->toString() + " <- " + i->oprand1->toString() + " " + i->op->toString() + " " + i->oprand2->toString() + '\n';
        outputFile << s;
    }

    void CodeGenerator::visit(Instruction_declare *i) {
        string s;
        s += "\t";
        if(i->type == var_int64) {
            s += "int64"; 
        }
        else if(i->type == var_tuple) {
            s += "tuple"; 
        }
        else if(i->type == var_code) {
            s += "code"; 
        }
        else {
            ArrayVar* a = dynamic_cast<ArrayVar*>(i->declared[0]); 
            if(a == nullptr) cout << "bug Instruction declare"; 
            s += "int64"; 
            for(int c = 0; c < a->dimension; c++){
                s += "[]"; 
            }
        }
        for(Variable* v : i->declared) {
            outputFile << s << " " << v->toString() << endl; 
        }
    }
    void CodeGenerator::visit(Instruction_call_noassign *i) {
        string s = "\t";
        if(i->callee->getType() == item_function) s += ":";
        s += i->callee->toString() + "("; 
        if(i->args.size() > 0) {
            for(int idx = 0; idx < i->args.size() - 1; idx++){
                s += i->args[idx]->toString() + ", "; 
            } 
            s += i->args.back()->toString() + ")\n"; 
        }
        else s += ")\n";
        outputFile << s; 
    }
    void CodeGenerator::visit(Instruction_call_assignment *i) {
        string s = "\t" + i->dst->toString() + " <- ";
        if(i->callee->getType() == item_function) s += ":";
        s += i->callee->toString() + "("; 
        if(i->args.size() > 0) {
            for(int idx = 0; idx < i->args.size() - 1; idx++){
                s += i->args[idx]->toString() + ", "; 
            } 
            s += i->args.back()->toString() + ")\n"; 
        }
        else s += ")\n";
        outputFile << s; 
    }
    void CodeGenerator::visit(Instruction_label *i) {
        outputFile << "\t" << i->toString() << endl;
    }
    void CodeGenerator::visit(Instruction_array *i) {
        string s; 
        s += "\t" + i->dst->toString() + " <- new Array("; 
        for(int idx = 0; idx < i->args.size() - 1; idx++){
            s += i->args[idx]->toString() + ", "; 
        } 
        s += i->args.back()->toString() + ")\n"; 
        outputFile << s; 
    }
    void CodeGenerator::visit(Instruction_tuple *i) {
        string s; 
        s += "\t" + i->dst->toString() + " <- new Tuple(";
        s += i->arg->toString() + ")\n"; 
        outputFile << s;        
    }
    void CodeGenerator::visit(Instruction_print *i) {
        string s; 
        s += "\tprint(";
        s += i->src->toString() + ")\n"; 
        outputFile << s;        
    }
    void CodeGenerator::visit(Instruction_input *i) {
        outputFile << "\tinput()\n";        
    }
    void CodeGenerator::visit(Instruction_input_assignment *i) {
        string s; 
        s += "\t" + i->dst->toString() + " <- input()\n"; 
        outputFile << s;        
    }
    void CodeGenerator::visit(Instruction_if *i) {
        string s; 
        string newV = newVar(); 
        s += "\tint64 " + newV + "\n";
        outputFile << s;     
        s = "\t" + newV + " <- " + i->condition->oprand1->toString() + " " + i->condition->op->toString() + " " + i->condition->oprand2->toString() + "\n";
        outputFile << s;    
        s = "\tbr " + newV + " " + i->true_label->toString() + " " + i->false_label->toString() + '\n'; 
        outputFile << s;   
    }
    void CodeGenerator::visit(Instruction_while *i) {
        string s; 
        string newV = newVar(); 
        s += "\tint64 " + newV + "\n";
        outputFile << s;     
        s = "\t" + newV + " <- " + i->condition->oprand1->toString() + " " + i->condition->op->toString() + " " + i->condition->oprand2->toString() + "\n";
        outputFile << s;    
        s = "\tbr " + newV + " " + i->true_label->toString() + " " + i->false_label->toString() + '\n'; 
        outputFile << s;        
    }
    void CodeGenerator::visit(Instruction_goto *i) {
        string s; 
        s += "\tbr " + i->label->toString(); 
        outputFile << s;        
    }
    void CodeGenerator::visit(Instruction_break *i) {
        string s; 
        WhileLoop* w = loop[i]; 
        Label* I_exit = w->end; 
        s = "\tbr " + I_exit->toString() + "\n"; 
        outputFile << s;        
    }
    void CodeGenerator::visit(Instruction_continue *i) {
        string s; 
        WhileLoop* w = loop[i]; 
        Label* I_cond = w->cond; 
        s = "\tbr " + I_cond->toString() + "\n"; 
        outputFile << s;        
    }
    void generate_code(Program p) {
        /* 
        * Open the output file.
        */ 

        std::ofstream outputFile;
        outputFile.open("prog.LA");
        for (auto f : p.functions) {
            map<Instruction*, WhileLoop*> loop = processLoop(p, f); 
            CodeGenerator cg(f, loop, outputFile);
            cout << "new Function: " << endl; 
            outputFile << "define " << f->type << " :" << f->name << "(";
            for (int i = f->arguments.size() - 1; i >= 0 ; i--) {
                string s;
                if(f->arguments[i]->getVariableType() == var_int64) {
                    s += "int64"; 
                }
                else if(f->arguments[i]->getVariableType() == var_tuple) {
                    s += "tuple"; 
                }
                else if(f->arguments[i]->getVariableType() == var_code) {
                    s += "code"; 
                }
                else {
                    s += "int64"; 
                    ArrayVar* av = dynamic_cast<ArrayVar*>(f->arguments[i]); 
                    for(int idx = 0; idx < av->dimension; idx++){
                        s += "[]";
                    }
                }
                s += " " + f->arguments[i]->toString();
                if (i != 0) {
                    s += ", ";
                }
                outputFile << s;
            }
            outputFile << ") {" << endl;
            for(Instruction* i : f->instructions) {
                cout << i->toString() << endl;
                i->accept(&cg); 
            }
            outputFile << "}" << endl;
        }
    }
}
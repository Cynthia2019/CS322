
#include "memory_check.h"

extern bool is_debug;
namespace LA {
    
    const std::string temp_lineno = "temp_lineno";
    const std::string temp_arr_dimension = "temp_arr_dimension";
    const std::string temp_arr_length = "temp_arr_lengh";
    const std::string temp_arr_index = "temp_arr_index";
    const std::string temp_condition = "temp_condition";

    MemoryCheck::MemoryCheck(Program &p):p(p) {
        LL = p.getLongestLabel();
    }

    void MemoryCheck::doProgram() {
        for (auto f : p.functions) {
            doFunction(f);
        }
    }

    void MemoryCheck::doFunction(Function *f) {
        index = 0;
        auto insts = f->instructions;
        declare_variables(f);
        bool checked = false;

        for (auto i : insts) {
            if (is_debug) cout << "reading instruction: " << i->toString() << endl;
            Instruction_declare *dec = dynamic_cast<Instruction_declare *>(i);
            if (dec) {
                Instruction_assignment *assign = new Instruction_assignment();
                switch(dec->type) {
                    case var_int64:
                        assign->dst = dec->dst;
                        assign->src = new Number(0);
                        insertInstruction(f, assign, 1);
                        break;
                    case var_int64_multi:
                    case var_tuple:
                    case var_code:
                        assign->dst = dec->dst;
                        assign->src = new Number(0);
                        assign->do_not_encode = true;
                        insertInstruction(f, assign, 1);
                        break;
                    default:
                        break;
                }
            }

            Instruction_load *load = dynamic_cast<Instruction_load *>(i);
            if (load) {
                cout << "loading" << endl;
                check_initialize(f, load);
                check_access(f, load);
                cout << "load after" << endl;
                checked = true;
            }
            Instruction_store *store = dynamic_cast<Instruction_store *>(i);
            if (store) {
                cout << "storing" << endl;
                check_initialize(f, store);
                cout << "storing" << endl;
                check_access(f, store);
                cout << "storing" << endl;
                checked = true;
            }
            index++;
        }

        if (checked) {
            addTensorError(f);
        }
    }

    void MemoryCheck::insertInstruction(Function *f, Instruction *i) {
        f->instructions.insert(f->instructions.begin() + index, i);
        index++;
    }

    void MemoryCheck::insertInstruction(Function *f, Instruction *i, int64_t offset) {
        f->instructions.insert(f->instructions.begin() + index + offset, i);
        index++;
    }

    inline std::string MemoryCheck::tempVariable() {
        return "temp_" + to_string(counter++);
    }

    void MemoryCheck::check_initialize_help(Function *f, Item *arrayvar, int64_t lineno) {
        auto assign = new Instruction_assignment();
        assign->src = new Number(lineno);
        assign->dst = f->getVariable(temp_lineno);
        insertInstruction(f, assign);

        // auto dec_temp = new Instruction_declare();
        // dec_temp->type = var_int64;
        // dec_temp->dst = f->newVariable(tempVariable(), VarTypes::var_int64, 0);
        // insertInstruction(f, dec_temp);

        auto condition = new Instruction_op();
        condition->dst = f->getVariable(temp_condition);
        condition->op = new Operation("=");
        condition->oprand1 = arrayvar;
        condition->oprand2 = new Number(0);
        insertInstruction(f, condition);
        //auto condition = new Instruction_br_label();

        auto br = new Instruction_br_t();
        br->condition = condition->dst;
        br->label2 = new Label(LL + "_legal" + to_string(counter++));
        br->label1 = new Label(LL + "_error_uninitialize");
        insertInstruction(f, br);
        
        auto label = new Instruction_label();
        label->label = br->label2;
        insertInstruction(f, label);
    }

    void MemoryCheck::check_initialize(Function *f, Instruction_load *inst) {
        check_initialize_help(f, inst->src, inst->lineno);
    }

    void MemoryCheck::check_initialize(Function *f, Instruction_store *inst) {
        check_initialize_help(f, inst->dst, inst->lineno);
    }

    void MemoryCheck::check_single_help(Function *f, Variable *array, vector<Item *> indices) {
        for (int i = 0; i < indices.size(); i++) {
            Instruction_length *ilength = new Instruction_length();
            ilength->dimID = new Number(i);
            ilength->src = array;
            ilength->dst = f->getVariable(temp_arr_length);
            insertInstruction(f, ilength);

            auto assign = new Instruction_assignment();
            assign->src = indices[i];
            assign->dst = f->getVariable(temp_arr_index);
            insertInstruction(f, assign);

            auto condition = new Instruction_op();
            condition->dst = f->getVariable(temp_condition);
            condition->op = new Operation("<");
            condition->oprand1 = indices[i];
            condition->oprand2 = f->getVariable(temp_arr_length);
            insertInstruction(f, condition);

            auto br = new Instruction_br_t();
            br->condition = f->getVariable(temp_condition);
            br->label1 = new Label(LL + "_legal" + to_string(counter++));
            br->label2 = new Label(LL + "_error_single_dimension");
            insertInstruction(f, br);

            auto label = new Instruction_label();
            label->label = br->label1;
            insertInstruction(f, label);
        }
    }

    void MemoryCheck::check_multi_help(Function *f, Variable *array, vector<Item *> indices) {
        for (int i = 0; i < indices.size(); i++) {
            Instruction_length *ilength = new Instruction_length();
            ilength->dimID = new Number(i);
            ilength->src = array;
            ilength->dst = f->getVariable(temp_arr_length);
            insertInstruction(f, ilength);

            auto assign = new Instruction_assignment();
            assign->src = indices[i];
            assign->dst = f->getVariable(temp_arr_index);
            insertInstruction(f, assign);

            auto assign_dim = new Instruction_assignment();
            assign_dim->src = new Number(i);
            assign_dim->dst = f->getVariable(temp_arr_dimension);
            insertInstruction(f, assign_dim);

            auto condition = new Instruction_op();
            condition->dst = f->getVariable(temp_condition);
            condition->op = new Operation("<");
            condition->oprand1 = indices[i];
            condition->oprand2 = f->getVariable(temp_arr_length);
            insertInstruction(f, condition);

            auto br = new Instruction_br_t();
            br->condition = f->getVariable(temp_condition);
            br->label1 = new Label(LL + "_legal" + to_string(counter++));
            br->label2 = new Label(LL + "_error_multi_dimension");
            insertInstruction(f, br);

            auto label = new Instruction_label();
            label->label = br->label1;
            insertInstruction(f, label);
        }
    }

    void MemoryCheck::check_access(Function *f, Instruction_load *inst) {
        if (dynamic_cast<TupleVar *>(inst->src)) {
            return;
        }
        if (inst->indices.size() == 1) {
            check_single_help(f, inst->src, inst->indices);
        } else if (inst->indices.size() > 1) {
            check_multi_help(f, inst->src, inst->indices);
        }
    }

    void MemoryCheck::check_access(Function *f, Instruction_store *inst) {
        if (dynamic_cast<TupleVar *>(inst->dst)) {
            return;
        }
        if (inst->indices.size() == 1) {
            check_single_help(f, inst->dst, inst->indices);
        } else if (inst->indices.size() > 1) {
            check_multi_help(f, inst->dst, inst->indices);
        }
    }

    void MemoryCheck::declare_variables(Function *f) {
        auto iter = f->instructions.begin();
        auto dec_lineno = new Instruction_declare();
        dec_lineno->type = var_int64;
        dec_lineno->dst = f->newVariable(temp_lineno, VarTypes::var_int64, 0);
        insertInstruction(f, dec_lineno);
        //f->instructions.insert(f->instructions.begin(), dec_lineno);

        auto dec_dimension = new Instruction_declare();
        dec_dimension->type = var_int64;
        dec_dimension->dst = f->newVariable(temp_arr_dimension, VarTypes::var_int64, 0);
        insertInstruction(f, dec_dimension);

        auto dec_length = new Instruction_declare();
        dec_length->type = var_int64;
        dec_length->dst = f->newVariable(temp_arr_length, VarTypes::var_int64, 0);
        insertInstruction(f, dec_length);

        auto dec_index = new Instruction_declare();
        dec_index->type = var_int64;
        dec_index->dst = f->newVariable(temp_arr_index, VarTypes::var_int64, 0);
        insertInstruction(f, dec_index);

        auto dec_condi = new Instruction_declare();
        dec_condi->type = var_int64;
        dec_condi->dst = f->newVariable(temp_condition, VarTypes::var_int64, 0);
        insertInstruction(f, dec_condi);
    }
    
    
    void MemoryCheck::addTensorError(Function *f) {
        Variable *lineno = f->getVariable(temp_lineno);
        Variable *arr_dimension = f->getVariable(temp_arr_dimension);
        Variable *arr_length = f->getVariable(temp_arr_length);
        Variable *arr_index = f->getVariable(temp_arr_index);

        auto label = new Instruction_label();
        label->label = new Label(LL + "_error_uninitialize");
        auto error_uninitialized = new Instruction_call_noassign();
        error_uninitialized->callee = new String("tensor-error");
        error_uninitialized->args.push_back(lineno);


        auto label_single = new Instruction_label();
        label_single->label = new Label(LL + "_error_single_dimension");
        auto error_single = new Instruction_call_noassign();
        error_single->callee = new String("tensor-error");
        error_single->args.push_back(lineno);
        error_single->args.push_back(arr_length);
        error_single->args.push_back(arr_index);

        auto label_multi = new Instruction_label();
        label_multi->label = new Label(LL + "_error_multi_dimension");
        auto error_multi = new Instruction_call_noassign();
        error_multi->callee = new String("tensor-error");
        error_multi->args.push_back(lineno);
        error_multi->args.push_back(arr_dimension);
        error_multi->args.push_back(arr_length);
        error_multi->args.push_back(arr_index);

        f->instructions.push_back(label);
        f->instructions.push_back(error_uninitialized);
        f->instructions.push_back(label_single);
        f->instructions.push_back(error_single);
        f->instructions.push_back(label_multi);
        f->instructions.push_back(error_multi);
    }

    void generateBasicBlock(Function *f, std::string ll) {
        auto insts = f->instructions;
        bool startBB = true;
        vector<Instruction *> res;
        int64_t counter = 0;

        for (auto i : insts) {
            Instruction_label *label = dynamic_cast<Instruction_label *>(i);
            if (startBB) {
                if (!label) {
                    auto newl = new Instruction_label();
                    newl->label = new Label(ll + "label_bb_temp_" + to_string(counter++));
                    res.push_back(newl);
                }
                startBB = false;
            } else if (label) {
                auto br = new Instruction_br_label();
                br->label = label->label;
                res.push_back(br);
            }
            res.push_back(i);
            
            auto rtrn = dynamic_cast<Instruction_ret *>(i);
            auto br = dynamic_cast<Instruction_br *>(i);
            // bool istensor = false;
            // auto tensor = dynamic_cast<Instruction_call *>(i);
            // if (tensor && tensor->callee->toString() == "tensor-error") {
            //     istensor = true;
            // }
            if (rtrn || br) {
                startBB = true;
            }
        }
        if (!startBB) {
            Instruction *ret;
            if (f->type == "void") {
                ret = new Instruction_ret_not();
            } else {
                auto t = new Instruction_ret_t();
                t->arg = new Number(0);
                ret = t;
            }
            res.push_back(ret);
        }

        f->instructions = res;
    }
}
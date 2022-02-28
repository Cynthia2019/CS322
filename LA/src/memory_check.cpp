
#include "memory_check.h"

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
        declare_variables(f);
        auto insts = f->instructions;

        for (auto i : insts) {
            Instruction_load *load = dynamic_cast<Instruction_load *>(i);
            if (load) {
                check_initialize(f, load);
            }
            Instruction_store *store = dynamic_cast<Instruction_store *>(i);
            if (store) {
                check_initialize(f, store);
            }
            index++;
        }

        addTensorError(f);
    }

    void MemoryCheck::insertInstruction(Function *f, Instruction *i) {
        f->instructions.insert(f->instructions.begin() + index, i);
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

        auto dec_temp = new Instruction_declare();
        dec_temp->type = var_int64;
        dec_temp->dst = f->newVariable(tempVariable(), VarTypes::var_int64, 0);
        insertInstruction(f, dec_temp);

        auto condition = new Instruction_op();
        condition->dst = dec_temp->dst;
        condition->op = new Operation("=");
        condition->oprand1 = arrayvar;
        condition->oprand2 = new Number(0);
        insertInstruction(f, condition);
        //auto condition = new Instruction_br_label();

        auto br = new Instruction_br_t();
        br->condition = dec_temp->dst;
        br->label1 = new Label(LL + "_legal" + to_string(counter++));
        br->label2 = new Label(LL + "_error_uninitialize");
        insertInstruction(f, br);
        
        auto label = new Instruction_label();
        label->label = br->label1;
        insertInstruction(f, label);
    }

    void MemoryCheck::check_initialize(Function *f, Instruction_load *inst) {
        check_initialize_help(f, inst->src, inst->lineno);
    }

    void MemoryCheck::check_initialize(Function *f, Instruction_store *inst) {
        check_initialize_help(f, inst->dst, inst->lineno);
    }

    void MemoryCheck::check_single_help(Function *f, ArrayVar *array, vector<Item *> indices) {
        for (int i = 0; i < array->dimension; i++) {
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

    void MemoryCheck::check_multi_help(Function *f, ArrayVar *array, vector<Item *> indices) {
        for (int i = 0; i < array->dimension; i++) {
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
            br->label2 = new Label(LL + "_error_multi_dimension");
            insertInstruction(f, br);

            auto label = new Instruction_label();
            label->label = br->label1;
            insertInstruction(f, label);
        }
    }

    void MemoryCheck::check_single(Function *f, Instruction_load *inst) {
        if (inst->indices.size() == 1) {
            check_single_help(f, dynamic_cast<ArrayVar *>(inst->src), inst->indices);
        } else if (inst->indices.size() > 1) {
            check_multi_help(f, dynamic_cast<ArrayVar *>(inst->src), inst->indices);
        }
    }

    void MemoryCheck::check_single(Function *f, Instruction_store *inst) {
        if (inst->indices.size() == 1) {
            check_single_help(f, dynamic_cast<ArrayVar *>(inst->dst), inst->indices);
        } else if (inst->indices.size() > 1) {
            check_multi_help(f, dynamic_cast<ArrayVar *>(inst->dst), inst->indices);
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
}
#include "LA.h"

namespace LA {
    
    const std::string temp_lineno = "temp_lineno";
    const std::string temp_arr_dimension = "temp_arr_dimension";
    const std::string temp_arr_length = "temp_arr_lengh";
    const std::string temp_arr_index = "temp_arr_index";

    void declare_variables(Program &p, Function *f) {
        auto iter = f->instructions.begin();
        auto dec_lineno = new Instruction_declare();
        dec_lineno->type = var_int64;
        dec_lineno->dst = f->newVariable(temp_lineno, VarTypes::var_int64, 0);
        f->instructions.insert(f->instructions.begin(), dec_lineno);

        auto dec_dimension = new Instruction_declare();
        dec_dimension->type = var_int64;
        dec_dimension->dst = f->newVariable(temp_arr_dimension, VarTypes::var_int64, 0);
        f->instructions.insert(f->instructions.begin(), dec_dimension);

        auto dec_length = new Instruction_declare();
        dec_length->type = var_int64;
        dec_length->dst = f->newVariable(temp_arr_length, VarTypes::var_int64, 0);
        f->instructions.insert(f->instructions.begin(), dec_length);

        auto dec_index = new Instruction_declare();
        dec_index->type = var_int64;
        dec_index->dst = f->newVariable(temp_arr_index, VarTypes::var_int64, 0);
        f->instructions.insert(f->instructions.begin(), temp_arr_index);
    }
    
    
    void addTensorError(Program &p, Function *f) {
        std::string LL = p.getLongestLabel();
        Variable *lineno = f->getVariable(temp_lineno);
        Variable *arr_dimension = f->getVariable(arr_dimension);
        Variable *arr_length = f->getVariable(arr_length);
        Variable *arr_index = f->getVariable(arr_index);

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
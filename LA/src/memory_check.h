#pragma once
#include "LA.h"

namespace LA
{
    class MemoryCheck {
        public:
        MemoryCheck(Program &);
        void doProgram();
        void doFunction(Function *f);
        std::string LL;

        private:
        int64_t counter = 0;
        void check_initialize(Function *f, Instruction_load *inst);
        void check_initialize(Function *f, Instruction_store *inst);
        void declare_variables(Function *f);
        inline std::string tempVariable();
        Program &p;
        int64_t index;
        void insertInstruction(Function *f, Instruction *i);
        void addTensorError(Function *f);
        void check_initialize_help(Function *f, Item *arrayvar, int64_t);
        void check_access(Function *f, Instruction_load *inst);
        void check_access(Function *f, Instruction_store *inst);
        void check_single_help(Function *f, ArrayVar *array, vector<Item *> indices);
        void check_multi_help(Function *f, ArrayVar *array, vector<Item *> indices);

    };

    void generateBasicBlock(Function *f, std::string ll);
    
} // namespace LA

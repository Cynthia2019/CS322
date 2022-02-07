#pragma once

#include "L2.h"

namespace L2 {
    bool spillOne(Program *, Function *, Variable *,
        std::string,
        std::unordered_map<Variable *, bool> &,
        int64_t
    );
    int64_t spillMultiple(Program *, Function *, vector<Variable *>,
        std::unordered_map<Variable *, bool> &,
        std::string,
        int64_t);
    void spillAll();
}
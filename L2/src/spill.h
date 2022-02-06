#pragma once

#include "L2.h"

namespace L2 {
    void spill(Program *p, Function *f, Variable *, ::string);
    int64_t spillMultiple(Program *p, Function *, vector<Variable *>);
    void spillAll();
}
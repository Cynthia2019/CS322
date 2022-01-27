#pragma once

#include <vector>
#include "L2.h"

using namespace L2;

namespace L2 {
    vector<Item *> get_caller_saved_regs();

    vector<Item *> get_callee_saved_regs();

    vector<Item *> get_argument_regs(size_t n);
}
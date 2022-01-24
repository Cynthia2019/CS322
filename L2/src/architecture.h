#pragma once

#include <vector>
#include "L2.h"

using namespace L2;

vector<Item *> get_caller_saved_regs() {
    vector<std::string> all = {
        "r10",
        "r11",
        "r8",
        "r9",
        "rax",
        "rcx",
        "rdi",
        "rdx",
        "rsi"
    };

    vector<Item *> res;
    for (auto s: all) {
        Item_register *r = new Item_register();
        r->register_name = s;
        res.push_back(r);
    }

    return res;
}

vector<Item *> get_callee_saved_regs() {
    vector<std::string> all = {
        "r12",
        "r13",
        "r14",
        "r15",
        "rbp",
        "rbx"
    };

    vector<Item *> res;
    for (auto s: all) {
        Item_register *r = new Item_register();
        r->register_name = s;
        res.push_back(r);
    }

    return res;
}
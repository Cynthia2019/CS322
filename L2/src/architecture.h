#pragma once
#include <string> 
#include <vector>
#include <unordered_map>

using namespace std;

namespace L2 {
    class Architecture {
        public: 
        enum RegisterID {rdi, rax, rbx, rbp, r10, r11, r12, r13, r14, r15, rsi, rdx, rcx, r8, r9, rsp};
        enum Color {red, orange, yellow, green, blue, purple, pink, black, white, grey, 
                    brown, lime, bronze, golden, silver};
        static Color fromRegisterToColor(RegisterID r);
        static const int64_t GPRegisters = 15; 
        static string fromRegisterToString(RegisterID r); 
        static bool isGeneralPurpose(RegisterID r); 
        static bool isCallerSaved(RegisterID r); 
        static bool isCalleeSaved(RegisterID r); 
        static bool isArgument(RegisterID r); 
        static vector<Architecture::RegisterID> get_caller_saved_regs();
        static vector<Architecture::RegisterID> get_callee_saved_regs();
        static vector<Architecture::RegisterID> get_argument_regs();
    };
}
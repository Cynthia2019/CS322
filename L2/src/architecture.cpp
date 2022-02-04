#include <architecture.h>
#include <string> 
#include <unordered_map>

using namespace std;

namespace L2 {
    unordered_map<Architecture::RegisterID, string> register_to_string = 
    { 
        {Architecture::rax, "rax"}, 
        {Architecture::rbx, "rbx"}, 
        {Architecture::rcx, "rcx"}, 
        {Architecture::rdx, "rdx"}, 
        {Architecture::rdi, "rdi"}, 
        {Architecture::rsi, "rsi"}, 
        {Architecture::rbp, "rbp"}, 
        {Architecture::r8, "r8"}, 
        {Architecture::r9, "r9"}, 
        {Architecture::r10, "r10"}, 
        {Architecture::r11, "r11"}, 
        {Architecture::r12, "r12"}, 
        {Architecture::r13, "r13"}, 
        {Architecture::r14, "r14"}, 
        {Architecture::r15, "r15"}, 
        {Architecture::rsp, "rsp"}
    }; 
    vector<Architecture::RegisterID> argument_regs = {
        Architecture::rdi, 
        Architecture::rsi, 
        Architecture::rdx,
        Architecture::rcx,
        Architecture::r8,
        Architecture::r9 };
    vector<Architecture::RegisterID> caller_saved_regs = {
        Architecture::rdi, 
        Architecture::rsi, 
        Architecture::rdx,
        Architecture::rcx,
        Architecture::r8,
        Architecture::r9,
        Architecture::r10,
        Architecture::r11,
        Architecture::rax
         };
    vector<Architecture::RegisterID> callee_saved_regs = {
        Architecture::rbp, 
        Architecture::rbx, 
        Architecture::r12,
        Architecture::r13,
        Architecture::r14,
        Architecture::r15
         };
    static std::string fromRegisterToString(Architecture::RegisterID r) {
        return register_to_string[r]; 
    };
    bool Architecture::isArgument(Architecture::RegisterID r){
        if(find(argument_regs.begin(), argument_regs.end(), r) != argument_regs.end()) return true; 
        return false;
    }
    bool Architecture::isCallerSaved(Architecture::RegisterID r){
        if(find(caller_saved_regs.begin(), caller_saved_regs.end(), r) != caller_saved_regs.end()) return true; 
        return false;
    } 
    bool Architecture::isCalleeSaved(Architecture::RegisterID r){
        if(find(callee_saved_regs.begin(), callee_saved_regs.end(), r) != callee_saved_regs.end()) return true; 
        return false;
    }      
    bool Architecture::isGeneralPurpose(Architecture::RegisterID r){
        if(find(argument_regs.begin(), argument_regs.end(), r) != argument_regs.end()) return true; 
        if(find(callee_saved_regs.begin(), callee_saved_regs.end(), r) != callee_saved_regs.end()) return true; 
        return false;
    }
    static vector<Architecture::RegisterID> get_caller_saved_regs() {
        return caller_saved_regs;
    }
    static vector<Architecture::RegisterID> get_callee_saved_regs() {
        return callee_saved_regs;
    }
    static vector<Architecture::RegisterID> get_argument_regs() {
        return argument_regs;
    }
}
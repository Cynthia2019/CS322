#include "L2.h"
#include "architecture.h"

namespace L2 {
  vector<Item *> Instruction_assignment::get_gen_set() {
    vector<Item *> v;
    if (src->get_type() == item_variable) {
      v.push_back( (src));
    }
    return v;
  }

  vector<Item *> Instruction_assignment::get_kill_set() {
    vector<Item *> v;
    if (dst->get_type() == item_variable) {
      v.push_back( (src));
    }
    return v;
  }

  vector<Item *> Instruction_load::get_gen_set() { return {}; }

  vector<Item *> Instruction_load::get_kill_set() {
    vector<Item *> v;
    if (dst->get_type() == item_variable) {
      v.push_back( (src));
    }
    return v;
  }

  vector<Item *> Instruction_store::get_gen_set() {
    vector<Item *> v;
    if (src->get_type() == item_variable) {
      v.push_back( (src));
    }
    return v;
  }

  vector<Item *> Instruction_store::get_kill_set() { return {}; }

  vector<Item *> Instruction_ret::get_gen_set() {
    Item_register *i = new Item_register();
    i->register_name = "rax";
    auto v = get_callee_saved_regs();
    v.push_back(i);
    return v;
  }

  vector<Item *> Instruction_ret::get_kill_set() { return {}; }

  vector<Item *> Instruction_shift::get_gen_set() { 
    vector<Item *> v;
    if (src->get_type() == item_variable) {
      v.push_back( (src));
    }
    return v;
  }

  vector<Item *> Instruction_shift::get_kill_set() {
    vector<Item *> v;
    if (dst->get_type() == item_variable) {
      v.push_back( (src));
    }
    return v;
  }


  vector<Item *> Instruction_aop::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_aop::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_store_aop::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_store_aop::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_compare::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_compare::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_cjump::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_cjump::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_call::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_call::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_call_print::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_call_print::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_call_input::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_call_input::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_call_allocate::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_call_allocate::get_kill_set() {
    return {};
  }


  vector<Item *> Instruction_call_error::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_call_error::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_increment::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_increment::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_decrement::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_decrement::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_at::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_at::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_load_aop::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_load_aop::get_kill_set() {
    return {};
  }
  vector<Item *> Instruction_goto::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_goto::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_label::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_label::get_kill_set() {
    return {};
  }

}
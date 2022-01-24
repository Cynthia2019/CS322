#include "L2.h"
#include "architecture.h"
#include <iostream>

namespace L2 {

  vector<Item *> Instruction_assignment::get_gen_set() {
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back((src));
    }
    return v;
  }

  vector<Item *> Instruction_assignment::get_kill_set() {
    vector<Item *> v;
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back((dst));
    }
    return v;
  }

  vector<Item *> Instruction_load::get_gen_set() { 
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back((src));
    }
    if (src->get_type() == item_register && src->get_content() == "rsp") v.pop_back();
    return v;
   }

  vector<Item *> Instruction_load::get_kill_set() {
    vector<Item *> v;
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back((dst));
    }
    return v;
  }

  vector<Item *> Instruction_store::get_gen_set() {
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back( (src));
    }
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back( (dst));
    }
    if (dst->get_type() == item_register && dst->get_content() == "rsp") v.pop_back();
    return v;
  }

  vector<Item *> Instruction_store::get_kill_set() {
    return {}; 
  }

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
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back((src));
    }
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back((dst));
    }
    return v;
  }

  vector<Item *> Instruction_shift::get_kill_set() {
    vector<Item *> v;
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back((dst));
    }
    return v;
  }


  vector<Item *> Instruction_aop::get_gen_set() { 
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back((src));
    }
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back((dst));
    }
    return v;
  }

  vector<Item *> Instruction_aop::get_kill_set() {
    vector<Item *> v;
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back((dst));
    }
    return v;
  }

  vector<Item *> Instruction_store_aop::get_gen_set() { 
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back((src));
    }
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back((dst));
    }
    if (dst->get_type() == item_register && dst->get_content() == "rsp") v.pop_back();
    return v;
  }

  vector<Item *> Instruction_store_aop::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_stack::get_gen_set() { 
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back((src));
    }
    return v;
  }

  vector<Item *> Instruction_stack::get_kill_set() {
    return {}; 
  }
  vector<Item *> Instruction_compare::get_gen_set() { 
    vector<Item *> v;
    if (oprand1->get_type() == item_variable || oprand1->get_type() == item_register) {
      v.push_back((oprand1));
    }
    if (oprand2->get_type() == item_variable || oprand2->get_type() == item_register) {
      v.push_back((oprand2));
    }
    return v;
  }

  vector<Item *> Instruction_compare::get_kill_set() {
    vector<Item *> v;
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back((dst));
    }
    return v;
  }

  vector<Item *> Instruction_cjump::get_gen_set() { 
    vector<Item *> v;
    if (oprand1->get_type() == item_variable || oprand1->get_type() == item_register) {
      v.push_back((oprand1));
    }
    if (oprand2->get_type() == item_variable || oprand2->get_type() == item_register) {
      v.push_back((oprand2));
    }
    return v;
  }

  vector<Item *> Instruction_cjump::get_kill_set() {
    return {};
  }

  vector<Item *> Instruction_call::get_gen_set() { 
    vector<Item *> v;
    if (constant->get_type() == item_number) {
      Item_number *c = dynamic_cast<Item_number *>(constant);
      v = get_argument_regs(c->get_nb());
    }
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back(dst);
    }
    return v;
  }

  vector<Item *> Instruction_call::get_kill_set() {
    vector<Item *> v = get_caller_saved_regs();
    return v;
  }

  vector<Item *> Instruction_call_print::get_gen_set() { 
    vector<Item *> v;
    v = get_argument_regs(1);
    return v;
  }

  vector<Item *> Instruction_call_print::get_kill_set() {
    vector<Item *> v = get_caller_saved_regs();
    return v;
  }

  vector<Item *> Instruction_call_input::get_gen_set() { 
    return {};
  }

  vector<Item *> Instruction_call_input::get_kill_set() {
    vector<Item *> v = get_caller_saved_regs();
    return v;
  }

  vector<Item *> Instruction_call_allocate::get_gen_set() { 
    vector<Item *> v;
    v = get_argument_regs(2);
    return v;
  }

  vector<Item *> Instruction_call_allocate::get_kill_set() {
    vector<Item *> v = get_caller_saved_regs();
    return v;
  }


  vector<Item *> Instruction_call_error::get_gen_set() { 
    vector<Item *> v;
    if (constant->get_type() == item_number) {
      Item_number *c = dynamic_cast<Item_number *>(constant);
      v = get_argument_regs(c->get_nb());
    }
    return v;
  }

  vector<Item *> Instruction_call_error::get_kill_set() {
    vector<Item *> v = get_caller_saved_regs();
    return v;
  }

  vector<Item *> Instruction_increment::get_gen_set() { 
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back(src);
    }
    return v;
  }

  vector<Item *> Instruction_increment::get_kill_set() {
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back(src);
    }
    return v;
  }

  vector<Item *> Instruction_decrement::get_gen_set() { 
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back(src);
    }
    return v;
  }

  vector<Item *> Instruction_decrement::get_kill_set() {
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back(src);
    }
    return v;
  }

  vector<Item *> Instruction_at::get_gen_set() { 
    vector<Item *> v;
    if (src_mult->get_type() == item_variable || src_mult->get_type() == item_register) {
      v.push_back(src_mult);
    }
    if (src_add->get_type() == item_variable || src_add->get_type() == item_register) {
      v.push_back(src_add);
    }
    return v;
  }

  vector<Item *> Instruction_at::get_kill_set() {
    vector<Item *> v;
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back(dst);
    }
    return v;
  }

  vector<Item *> Instruction_load_aop::get_gen_set() { 
    vector<Item *> v;
    if (src->get_type() == item_variable || src->get_type() == item_register) {
      v.push_back(src);
    }
    if (src->get_type() == item_register && src->get_content() == "rsp") v.pop_back();
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back(dst);
    }
    return v;
  }

  vector<Item *> Instruction_load_aop::get_kill_set() {
    vector<Item *> v;
    if (dst->get_type() == item_variable || dst->get_type() == item_register) {
      v.push_back(dst);
    }
    return v;
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
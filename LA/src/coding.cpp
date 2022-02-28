#include <coding.h> 
#include <LA.h> 

using namespace std;

extern bool is_debug; 
namespace LA
{
    vector<Item*> toDecode(Instruction* i) {
        Instruction_br_t* a = dynamic_cast<Instruction_br_t*>(i); 
        if(a != nullptr) {
            return {a->condition}; 
        }
        Instruction_length* b = dynamic_cast<Instruction_length*>(i); 
        if(b != nullptr && b->dimID->getType() == item_variable) {
            return {b->dimID}; 
        }
        Instruction_load* c = dynamic_cast<Instruction_load*>(i); 
        if(c != nullptr) {
            vector<Item*> ans; 
            for(Item* index : c->indices) {
                if(index->getType() == item_variable) ans.push_back(index); 
            }
            return ans; 
        }
        Instruction_store* d = dynamic_cast<Instruction_store*>(i); 
        if(d != nullptr) {
            vector<Item*> ans; 
            for(Item* index : d->indices) {
                if(index->getType() == item_variable) ans.push_back(index); 
            }
            return ans;             
        }
        Instruction_op* e = dynamic_cast<Instruction_op*>(i); 
        if(e != nullptr) {
            return {e->oprand1, e->oprand2};
        }
        return {};
    }
    vector<Item*> toEncode(Instruction* i) {
        Instruction_op* a = dynamic_cast<Instruction_op*>(i); 
        if(a != nullptr) {
            return {a->dst}; 
        }        
        return {};
    }
    int64_t encodeConstant(int64_t n) {
        n <<= 1; 
        n += 1; 
        return n; 
    }

    Encoder::Encoder(Function* f) {
        this->f = f;
    }
    void Encoder::visit(Instruction_ret_not* i) {}
    void Encoder::visit(Instruction_ret_t *i) {
        Number* a = dynamic_cast<Number*>(i->arg); 
        if(a != nullptr) {
            i->arg = new Number(encodeConstant(a->get())); 
        }
    }
    void Encoder::visit(Instruction_assignment *i) {
        Number* a = dynamic_cast<Number*>(i->src); 
        if(a != nullptr && (!i->do_not_encode)) {
            i->src = new Number(encodeConstant(a->get())); 
        }
    }
    void Encoder::visit(Instruction_load *i) {}
    void Encoder::visit(Instruction_op *i) {
        Number* a = dynamic_cast<Number*>(i->oprand1); 
        if(a != nullptr) {
            i->oprand1 = new Number(encodeConstant(a->get())); 
        }
        Number* b = dynamic_cast<Number*>(i->oprand1); 
        if(b != nullptr) {
            i->oprand2 = new Number(encodeConstant(b->get())); 
        }
    }
    void Encoder::visit(Instruction_store *i) {}
    void Encoder::visit(Instruction_declare *i) {}
    void Encoder::visit(Instruction_br_label *i) {}
    void Encoder::visit(Instruction_br_t *i) {
        Number* a = dynamic_cast<Number*>(i->condition); 
        if(a != nullptr) {
            i->condition = new Number(encodeConstant(a->get())); 
        }
    }
    void Encoder::visit(Instruction_call_noassign *i) {
        for(Item* item : i->args){
            Number* a = dynamic_cast<Number*>(item); 
            if(a != nullptr) {
                item = new Number(encodeConstant(a->get())); 
            }
        }
    }
    void Encoder::visit(Instruction_call_assignment *i) {
        for(Item* item : i->args){
            Number* a = dynamic_cast<Number*>(item); 
            if(a != nullptr) {
                item = new Number(encodeConstant(a->get())); 
            }
        }
    }
    void Encoder::visit(Instruction_label *i) {}
    void Encoder::visit(Instruction_length *i) {}
    void Encoder::visit(Instruction_array *i) {
        for(Item* item : i->args){
            Number* a = dynamic_cast<Number*>(item); 
            if(a != nullptr) {
                item = new Number(encodeConstant(a->get())); 
            }
        }
    }
    void Encoder::visit(Instruction_tuple *i) {
       Number* a = dynamic_cast<Number*>(i->arg); 
        if(a != nullptr) {
            i->arg = new Number(encodeConstant(a->get())); 
        }
    }
    void encodeAll(Program &p, Function* f) {
        Encoder encoder(f);
        for(Instruction* i : f->instructions) {
            i->accept(&encoder); 
        }
    }
}

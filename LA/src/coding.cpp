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
    }
}

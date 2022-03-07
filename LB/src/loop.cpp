#include <LB.h>
#include <loop.h> 
#include <stack>
#include <map>

using namespace std; 
namespace LB {
    vector<WhileLoop*> recordLoop(Program& p, Function* f) {
        vector<WhileLoop*> loops; 
        int counter = 0; 
        for(Instruction* i : f->instructions) {
            Instruction_while* wi = dynamic_cast<Instruction_while*>(i); 
            if(wi) {
                WhileLoop* w = new WhileLoop(); 
                w->begin = wi->true_label; 
                w->end = wi->false_label; 
                w->cond = new Label(p.getLongestLabel() + "_label_" + to_string(counter)); 
                counter++;
                w->i = wi;
                loops.push_back(w); 
            }
        }
        return loops; 
    }
    map<Instruction*, WhileLoop*> processLoop(Program &p, Function* f){
        vector<WhileLoop*> loops = recordLoop(p, f); 
        map<Instruction*, WhileLoop*> loop;
        stack<WhileLoop*> loopStack; 
        for(int i = 0; i < f->instructions.size(); i++){
            Instruction* inst = f->instructions[i]; 
            if(loopStack.size() > 0){
                WhileLoop* w = loopStack.top();
                loop[inst] = w;
            }
            Instruction_label* label_i = dynamic_cast<Instruction_label*>(inst); 
            if(label_i) {
                Label* label = label_i->label; 
                for(WhileLoop* w : loops) {
                    if(label->toString() == w->begin->toString()){
                        loop[w->i] = w;
                        loopStack.push(w);
                    }
                    else if(label->toString() == w->end->toString()) {
                        loopStack.pop(); 
                    }
                }
            }
        }
        // for(auto m : loop) {
        //     cout << "instruction: " << m.first->toString() << endl;
        //     cout << "loop begin: " << m.second->begin->toString() << endl;
        //     cout << "loop end: " << m.second->end->toString() << endl;
        //     cout << "loop new label: " << m.second->cond->toString() << endl;
        // }
        return loop;
    }
}
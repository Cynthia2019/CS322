#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <map>
#include <iterator>
#include <queue>
#include <stack>
#include <interference.h>
#include <liveness.h>
#include <coloring.h>
#include <spill.h>

extern bool is_debug;

namespace L2 {
    stack<Node*> st; 

    Colorer::Colorer(Program p, Function* f) {
        this->graph = computeInterference(p, f).first;
        this->prog = &p;
    }
    Graph* Colorer::getGraph() {
        return this->graph;
    }

    void Colorer::registerAllocate(Function *f) {
        bool success = false;
        unordered_map<Variable *, bool> spilled_variables;
        while (true) {
            removeNodeToStack();
            if (st.size() != neighborst.size()) {
                cerr << "graph stack size not matched" << endl;
                abort();
            }

            while (!st.empty()) {
                Node *v = st.top();
                st.pop();
                set<Node *> neighbor = neighborst.top();
                neighborst.pop();

                graph->addNode(v, neighbor);
                v->color = selectColor(v);
                if(is_debug) {
                    cout << "Node: " << v->get()->toString() << " Color: " << v->color << endl;
                }
            }

            if (areAllNodesColored()) {
                success = true;
                break;
            }

            auto toSpill = selectNodeToSpill(spilled_variables);
            if (toSpill.size() == 0) {
                // nothing we can spill, fail
                break;
            }

            int64_t spilled = spillMultiple(prog, f, toSpill, spilled_variables);
            if (spilled == 0) {
                cerr << "BUG, this should not happen, please look at coloring.cpp" << endl;
                break;
            }
        }

        if (!success) {
            spillAll();
        }
        
        //f->format_function();
    }

    static bool cmp(Node* a, Node* b){
        return a->degree < b->degree; 
    }

    bool Colorer::areAllNodesColored() {
        auto nodes = graph->getNodes();
        for (auto n : nodes) {
            if (n->color == Architecture::nocolor) {
                return false;
            }
        }
        return true;
    }

    void Colorer::removeNodeToStack() {
        vector<Node*> allNodes = graph->getNodes();
        priority_queue<Node*, vector<Node*>, decltype(&cmp)> pq(cmp); 
        for(Node* n : allNodes){
            if(n->degree < Architecture::GPRegisters && n->isVariable){
                pq.push(n); 
            }
        }
        while(!pq.empty()){
            Node* curr = pq.top(); 
            pq.pop(); 
            neighborst.push(graph->removeNode(curr)); 
            st.push(curr);
        }
        allNodes = graph->getNodes();
        for(Node* n : allNodes){
            if(n->isVariable){
                pq.push(n);
            }
        }
        while(!pq.empty()){
            Node* curr = pq.top(); 
            pq.pop(); 
            neighborst.push(graph->removeNode(curr));
            st.push(curr);
        }
    }


    Architecture::Color Colorer::selectColor(Node *node) {
        auto neighbor = graph->g[node];
        set<Architecture::Color> used;
        for (auto n : neighbor) {
            used.insert(n->color);
        }
        // if(is_debug) {
        //     cout << "used: ";
        //     for(auto i : used){
        //         cout << i << " "; 
        //     }
        //     cout << endl;
        // }
        auto caller = Architecture::get_caller_saved_regs();
        auto callee = Architecture::get_callee_saved_regs();
        vector<Architecture::Color> color_ordering;
        for (auto r : caller) {
            color_ordering.push_back(Architecture::fromRegisterToColor(r));
        }

        for (auto r : callee) {
            color_ordering.push_back(Architecture::fromRegisterToColor(r));
        }        
        // if(is_debug) {
        //     cout << "ordering: ";
        //     for(auto i : color_ordering){
        //         cout << i << " "; 
        //     }
        //     cout << endl;
        // }
        
        for (Architecture::Color c : color_ordering) {
            if (used.count(c) == 0) {
                return c;
            }
        }
        return Architecture::nocolor;
    }

    vector<Variable *> Colorer::selectNodeToSpill(unordered_map<Variable *, bool> spilled) {
        auto nodes = graph->getNodes();
        vector<Variable *> res;
        for (auto n : nodes) {
            if (n->color == Architecture::nocolor) {
                if (spilled.count(n->get()) != 0 && spilled[n->get()] == true) {
                    continue;
                }
                res.push_back(n->get());
            }
        }
        return res;
    }
}
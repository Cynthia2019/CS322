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

            auto toSpill = selectNodeToSpill();
            if (toSpill.size() == 0) {
                success = true;
                break;
            }

            int64_t spilled = spillMultiple(prog, f, toSpill);
            if (spilled == 0) {
                break;
            }
        }

        if (!success) {
            spillAll();
        }
        
        f->format_function();
    }

    static bool cmp(Node* a, Node* b){
        return a->degree < b->degree; 
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
        cout << "stack size: " << st.size() << endl;
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
        cout << "stack size second: " << st.size() << endl;
    }


    Architecture::Color Colorer::selectColor(Node *node) {
        auto neighbor = graph->g[node];
        vector<Architecture::Color> used;
        for (auto n : neighbor) {
            used.push_back(n->color);
        }

        auto caller = Architecture::get_caller_saved_regs();
        auto callee = Architecture::get_callee_saved_regs();
        vector<Architecture::Color> color_ordering;
        for (auto r : caller) {
            color_ordering.push_back(Architecture::fromRegisterToColor(r));
        }

        for (auto r : callee) {
            color_ordering.push_back(Architecture::fromRegisterToColor(r));
        }

        for (auto c : color_ordering) {
            if (find(used.begin(), used.end(), c) != used.end()) {
                return c;
            }
        }
        return Architecture::nocolor;
    }

    vector<Variable *> Colorer::selectNodeToSpill() {
        auto nodes = graph->getNodes();
        vector<Variable *> res;
        for (auto n : nodes) {
            if (n->color == Architecture::nocolor) {
                res.push_back(n->get());
            }
        }
        return res;
    }
}
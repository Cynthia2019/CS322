#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <map>
#include <iterator>

#include <interference.h>
#include <liveness.h>
#include <architecture.h>

using namespace std;

extern bool is_debug;

namespace L2
{
    Node::Node(Variable* var) {
        this->var = var; 
    } 
    string Node::variableToString() {
        return this->var->get();
    }
    Variable* Node::get() {
        return this->var; 
    }
    void Graph::addNode(Node* node){
        g[node] = {};
        nodes[node->get()] = node; 
        node->degree++;
        Graph::size++;
    }

    void Graph::addNode(Node *node, set<Node *> neighbor) {
        g[node] = neighbor;
        for (Node* i : neighbor) {
            i->degree++;
        }
        nodes[node->get()] = node;
        Graph::size++;
    }

    void Graph::addEdge(Node* n1, Node* n2){
        g[n1].insert(n2); 
        g[n2].insert(n1);
    }
    bool Graph::doesNodeExist(Node* node){
        if(g.find(node) == g.end()){
            return false; 
        }
        return true;
    }
    // we return node set for reconstructing
    set<Node *> Graph::removeNode(Node* node){
        set<Node*> connected = g[node]; 
        for(Node* i : connected){
            i->degree--;
        }
        g.erase(node); 
        nodes.erase(node->get());
        Graph::size--;
        return connected;
    }

    vector<Node*> Graph::getNodes() {
        vector<Node*> all; 
        for(auto m : g){
            all.push_back(m.first); 
        }
        return all;
    }
    void print_map(map<Item*, set<Item*>>& m){
        for(auto i : m){
            cout << i.first->toString() << " "; 
            for(auto s : i.second){
                cout << s->toString() << " ";
            }
            cout << endl;
             cout << endl;
        }
    }
    pair<Graph*, map<Item*, set<Item*>>> computeInterference(Program p, Function* f){
        AnalysisResult* res = computeLiveness(p, f).first;
        std::map<Instruction*, std::set<Item*>> in = res->ins; 
        std::map<Instruction*, std::set<Item*>> out = res->outs; 
        std::map<Instruction*, std::set<Item*>> gens = res->gens; 
        std::map<Instruction*, std::set<Item*>> kills = res->kills; 
        map<Item*, set<Item*>> edges; 
        Graph* g = new Graph();
        int length = f->instructions.size(); 
        //get all gp registers
        vector<Architecture::RegisterID> caller = Architecture::get_caller_saved_regs(); 
        vector<Architecture::RegisterID> callee = Architecture::get_callee_saved_regs(); 
        vector<Architecture::RegisterID> gp = caller; 
        gp.insert(gp.end(), callee.begin(), callee.end());  
        //connect gp to all other registers
        for(auto item1 : gp){
            for(auto item2 : gp){
                if(item1 != item2) {
                    Register* r1 = p.getRegister(item1); 
                    Register* r2 = p.getRegister(item2); 
                    edges[r1].insert(r2); 
                    edges[r2].insert(r1); 
                }
            }
        }
        for(Instruction* inst : f->instructions) {
            //connect each pair of variables in the same IN set 
            for(auto s1 : in[inst]){
                for(auto s2 : in[inst]){
                    if(s1 != s2) {
                        edges[s1].insert(s2); 
                        edges[s2].insert(s1); 
                    }
                }
            }
            //if in has only one element, add this element into graph
            if(in[inst].size() == 1) {
                set<Item*> ele = in[inst];
                Item* s = *ele.begin();
                edges[s].insert(s);
            }
            //if out has only one element, add this element into graph
            if(out.size() == 1) {
                set<Item*> ele = out[inst];
                Item* s = *ele.begin();
                edges[s].insert(s);
            }
            //connect each pair of variables in the same out set 
            for(auto s1 : out[inst]){
                for(auto s2 : out[inst]){
                    if(s1 != s2) {
                        edges[s1].insert(s2); 
                        edges[s2].insert(s1); 
                    }
                }
            }
            //connect variables in kill with those in out
            for(auto s1 : kills[inst]){
                for(auto s2 : out[inst]){
                    if(s1 != s2){
                        edges[s1].insert(s2); 
                        edges[s2].insert(s1);
                    }
                }
            }
            //check restrictions for each instruction
            //sop 
            Instruction_shift* a = dynamic_cast<Instruction_shift*>(inst); 
            if(a != nullptr) {
                //connect every register with dst except rcx
                Variable* src = dynamic_cast<Variable*>(a->src); 
                if(src != nullptr){
                    for(Architecture::RegisterID item : gp) {
                        if(item != Architecture::rcx && p.getRegister(item) != src) {
                            edges[src].insert(p.getRegister(item)); 
                            edges[p.getRegister(item)].insert(src); 
                        }
                    }
                }
                
            }
        }
        //complete graph
        Graph* graph = new Graph();
        //contains all the nodes in the graph
        for(auto m : edges){
            Variable* v = dynamic_cast<Variable*>(m.first); 
            Node* n = new Node(v);
            Register* r = dynamic_cast<Register*>(v); 
            if(r != nullptr) {
                n->color = Architecture::fromRegisterToColor(r->get());
                n->isVariable = false;
                // if(is_debug) {
                //     cout << "register: " << r->toString() << " color: " << n->color<< endl;
                // }
            }
            else {
                n->color = Architecture::nocolor;
            }
            graph->addNode(n); 
        }
        for(auto m : edges){
            Variable* v = dynamic_cast<Variable*>(m.first); 
            Node* nv = graph->nodes[v];
            for(Item* i : m.second){
                Variable* u = dynamic_cast<Variable*>(i);
                if(i == v) continue;
                Node* nu = graph->nodes[u]; 
                graph->addEdge(nv, nu); 
            }
        }
        if(is_debug) {
            cout << endl;
            cout << "print interference graph: " << endl; 
            print_map(edges); 
            cout << endl;
        }
        return {graph, edges};
    }
     
    void interference(Program p, Function* f) {
        pair<Graph*, map<Item*, set<Item*>>> graphRes = computeInterference(p, f); 
        Graph* g = graphRes.first; 
        map<Item*, set<Item*>> edges = graphRes.second; 
        if(is_debug) {
            cout << endl;
            cout << "print interference graph: " << endl; 
            print_map(edges); 
            cout << endl;
        }
    }
}
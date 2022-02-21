#include <CFG.h>
#include <IR.h>
#include <iostream>
#include <map> 
#include <set> 

using namespace std; 

extern bool is_debug;

namespace IR {
    Node::Node(BasicBlock* bb){
        this->bb = bb; 
    }
    BasicBlock* Node::getBasicBlock() {
        return this->bb;
    }
    void Graph::addNode(Node* n){
        this->nodes.insert(n); 
    }
    void Graph::addEdge(Node* n1, Node* n2){
        if(this->edges.count(n1) == 0) {  
            this->edges[n1] = {n2};
        }
        else {
            this->edges[n1].insert(n2);
        } 
    }
    void Graph::printGraph() {
        for(auto e : this->edges){
            cout << e.first->getFirst()->toString() << " "; 
            for(auto v : e.second) {
                cout << v->getFirst()->toString() << " "; 
            }
            cout << endl;
        }
    }
    set<Node*> Graph::getSuccessor(Node* n){
        return this->edges[n];
    }
    set<Node*> Graph::getAllNodes() {
        return this->nodes; 
    }
    map<Node*, set<Node*>> Graph::getAllEdges() {
        return this->edges;
    }
    Label* Node::getFirst() {
        return this->bb->label; 
    }
    Terminator* Node::getLast() {
        return this->bb->te; 
    }

    Graph* generateCFG(Program& p, Function* f) {
        //generate control flow graph from p
        Graph* graph = new Graph();
        //print each basic block 
        bool entry = false; 
        for(BasicBlock* bb : f->basicBlocks){
            // if(is_debug) 
            // {
            //     cout << endl << "label: " << endl ;
            //     cout << "instructions: " << bb->instructions.size()  << endl;
            //     for(Instruction* i : bb->instructions){
            //         cout << i->toString() << endl; 
            //     }
            //     cout << "terminator: " << bb->te->toString() << endl;
            //     cout << endl;
            // }
            Node* node = new Node(bb); 
            graph->addNode(node); 
            if(!entry){
                graph->entryNode = node;
                f->entry_label = node->getFirst()->toString();
                entry = true; 
            }
            if(node->getLast()->op->toString() == "return"){
                graph->exitNodes.insert(node);
            }
        }
        //
        set<Node*> allNodes = graph->getAllNodes();
        cout << "nodes size: " << allNodes.size() << endl;
        //find successor of x
        for(Node* x : allNodes){
            for(Node* y : allNodes){
                //check label match
                Instruction_br_label* l = dynamic_cast<Instruction_br_label*>(x->getLast());
                if(l != nullptr && l->label == nullptr) cout << "bug br_label\n";  
                if(l && y->getFirst()->toString() == l->label->toString()){
                    cout << y->getFirst()->toString() << endl;
                    graph->addEdge(x, y);
                }
                Instruction_br_t* t = dynamic_cast<Instruction_br_t*>(x->getLast()); 
                if(t != nullptr && (t->label1 == nullptr || t->label2 == nullptr)) cout << "bug br_t\n";
                if(t && (y->getFirst()->toString() == t->label1->toString() || y->getFirst()->toString() == t->label2->toString())){
                    graph->addEdge(x, y); 
                }
            }
        }
        graph->printGraph();
        return graph;
    }

    //fetch and remove the first bb from a list 
    Node* fetch_and_remove(set<Node*>& list){
        auto it = list.begin(); 
        Node* bb = *it; 
        list.erase(it);
        return bb;
    }
    void dfs(Node* node1, Node* node2, map<Node*, bool>& visited, map<Node*, set<Node*>>& edges, map<pair<Node*, Node*>, bool>& isProfitable){
        if(edges[node2].size() == 0 || visited[node2]) return ; 
        visited[node2] = true;
        for(Node* next : edges[node2]){
            isProfitable[{node1, next}] = true; 
            dfs(node1, next, visited, edges, isProfitable); 
        }
    }
    map<pair<Node*, Node*>, bool> checkRecheable(map<Node*, set<Node*>>& edges, set<Node*> nodes) {
        map<pair<Node*, Node*>, bool> isProfitable; 
        map<Node*, bool> visited; 
        for(Node* n : nodes){
            visited[n] = false;
            for(Node* v : nodes){
                if(n == v) continue; 
                isProfitable[{n, v}] = false;
            }
        }
        for(auto e : edges) {
            Node* node1 = e.first; 
            for(Node* node2 : edges[node1]) {
                isProfitable[{node1, node2}] = true;
                visited[node1] = true;
                dfs(node1, node2, visited, edges, isProfitable); 
            }
        }
        return isProfitable;
    } 
    vector<Trace*> linearize(Program &p, Function* f){
        vector<Trace*> traces;
        //step 1 get all basic blocks in a function 
        Graph* graph = generateCFG(p, f); 
        set<Node*> list = graph->getAllNodes(); 
        map<Node*, set<Node*>> edges = graph->getAllEdges();
        map<pair<Node*, Node*>, bool> profitable = checkRecheable(edges, list);
        // set<BasicBlock*> list; 
        // for(Node* n : nodes) {
        //     list.insert(n->getBasicBlock()); 
        // }

        do {
            Trace* tr = new Trace();
            //select a bb from list to remove 
            Node* bb = fetch_and_remove(list);
            while(!bb->isMarked) {
                bb->isMarked = true; 
                tr->traces.push_back(bb); 
                set<Node*> successors = edges[bb]; 
                for(Node* c : successors) {
                    cout << "profitable: " << profitable[{bb, c}] << endl;
                    if(!c->isMarked && profitable[{bb, c}]){
                        bb = c; 
                    }
                }
            }
            if(tr->traces.size() != 0) traces.push_back(tr);
        }
        while(!list.empty());

        //print trace
        for(Trace* t : traces) {
            cout << "new trace: " << endl;
            for(Node* n: t->traces) {
                cout << n->getFirst()->toString() << endl;
            }
        }
        return traces; 
    }
}
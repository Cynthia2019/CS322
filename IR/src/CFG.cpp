#include <CFG.h>
#include <IR.h>
#include <iostream>

using namespace std; 

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
        this->edges[n1].insert(n2);
    }
    set<Node*> Graph::getSuccessor(Node* n){
        return this->edges[n];
    }
    set<Node*> Graph::getAllNodes() {
        return this->nodes; 
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
            // cout << "label: " << bb->label->toString() << endl ;
            // cout << "instructions: " << endl;
            // for(Instruction* i : bb->instructions){
            //     cout << i->toString() << endl; 
            // }
            // cout << "terminator: " << bb->te->toString() << endl;
            Node* node = new Node(bb); 
            graph->addNode(node); 
            if(!entry){
                graph->entryNode = node;
                entry = true; 
            }
            if(node->getLast()->op->toString() == "return"){
                graph->exitNodes.insert(node);
            }
        }
        //
        set<Node*> allNodes = graph->getAllNodes();
        //find successor of x
        for(Node* x : allNodes){
            for(Node* y : allNodes){
                //check label match
                Instruction_br_label* l = dynamic_cast<Instruction_br_label*>(x->getLast()); 
                if(l && y->getFirst() == l->label){
                    graph->addEdge(x, y);
                }
                Instruction_br_t* t = dynamic_cast<Instruction_br_t*>(x->getLast()); 
                if(t && (y->getFirst() == t->label1 || y->getFirst() == t->label2)){
                    graph->addEdge(x, y); 
                }
            }
        }
        return graph;
    }

    //fetch and remove the first bb from a list 
    Node* fetch_and_remove(set<Node*> list){
        auto it = list.begin(); 
        Node* bb = *it; 
        list.erase(it);
        return bb;
    }
    set<Trace*> linearize(Program &p, Function* f){
        set<Trace*> traces;
        //step 1 get all basic blocks in a function 
        Graph* graph = generateCFG(p, f); 
        set<Node*> list = graph->getAllNodes(); 
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
                tr.insert(bb); 
                set<Node*> successors = graph.getSuccessor(bb); 
                for(Node* c : successors) {
                    if(!c->isMarked && profitable(bb, c)){
                        bb = c; 
                    }
                }
            }
        }
        while(!list.empty())
        return traces; 
    }
}
#pragma once
#include <IR.h> 
#include <iostream>
#include <vector>
#include <map> 
#include <set> 

using namespace std;

namespace IR {
    struct Node {
        public: 
        Node(BasicBlock* bb);
        Label* getFirst(); 
        Terminator* getLast();
        BasicBlock* getBasicBlock();
        bool isMarked = false; 
        private: 
        BasicBlock* bb; 
    };
    struct Graph {
        public: 
        Node* entryNode; 
        set<Node*> exitNodes; 
        set<Node*> getAllNodes(); 
        map<Node*, set<Node*>> getAllEdges();
        void addNode(Node* node); 
        void addEdge(Node* n1, Node* n2); 
        void printGraph();
        set<Node*> getSuccessor(Node* n);
        private: 
        set<Node*> nodes; 
        map<Node*, set<Node*>> edges; 
    };
    struct Trace {
        vector<Node*> traces; 
    };
    vector<Trace*> linearize(Program& p, Function* f); 
    // void generateCFG(Program& p, Function* f);
}
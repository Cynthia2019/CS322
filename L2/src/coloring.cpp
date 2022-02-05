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
#include <architecture.h>

namespace L2 {
    stack<Node*> st; 
    static bool cmp(Node* a, Node* b){
        return a->degree < b->degree; 
    }
    void coloring(Program p) {
        Graph* graph = computeInterference(p).first;
        //remove nodes with edges < 15
        vector<Node*> allNodes = graph->getNodes();
        priority_queue<Node*, vector<Node*>, decltype(&cmp)> pq(cmp); 
        for(Node* n : allNodes){
            if(n->degree < 15 && n->isVariable){
                pq.push(n); 
            }
        }
        while(!pq.empty()){
            Node* curr = pq.top(); 
            pq.pop(); 
            graph->removeNode(curr); 
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
            graph->removeNode(curr); 
            st.push(curr);
        }
    };
}
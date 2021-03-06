#pragma once

#include <L2.h>
#include <map> 
#include <set> 
#include <liveness.h>

namespace L2
{
  void interference(Program p, Function* f);
  class Node {
    public: 
      int32_t degree; 
      Node(Variable* v); 
      Variable* get(); 
      Architecture::Color color;
      bool isVariable = true;
      string variableToString(); 
    private: 
      Variable* var; 
  };
  class Graph {
    public: 
     std::map<Node*, std::set<Node*>> g; 
     std::map<Variable*, Node*> nodes; 
     void addNode(Node *n); 
     void addNode(Node *, set<Node *>);
     void addEdge(Node *n1, Node *n2); 
     bool doesNodeExist(Node *n); 
     set<Node *> removeNode(Node *n); 
     std::vector<Node*> getNodes(); 
     int32_t size; 
  };
  pair<Graph*, map<Item*, set<Item*>>> computeInterference(Program p, Function* f);
}
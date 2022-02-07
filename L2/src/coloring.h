#pragma once

#include <L2.h>
#include <map> 
#include <set> 
#include <stack>
#include <liveness.h>
#include <interference.h>
#include "architecture.h"

namespace L2
{
    class Colorer {
        public:
        Colorer(Program p, Function *f);
        void registerAllocate(Function *f);
        Graph* getGraph();

        private:
        Graph *graph;
        Program *prog;
        stack<Node *> st;
        stack<set<Node *>> neighborst;
        void removeNodeToStack();
        Architecture::Color selectColor(Node *);
        vector<Variable *> selectNodeToSpill(unordered_map<Variable *, bool>);
        bool areAllNodesColored();
    };
}
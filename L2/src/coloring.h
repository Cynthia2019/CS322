#pragma once

#include <L2.h>
#include <map> 
#include <set> 
#include <liveness.h>
#include <interference.h>
#include "architecture.h"

namespace L2
{
    class Colorer {
        public:
        Colorer(Program *p);
        void registerAllocate(Function *);

        private:
        Graph *graph;
        Program *prog;
        stack<Node *> st;
        stack<set<Node *>> neighborst;
        void removeNodeToStack();
        Architecture::Color selectColor(Node *);
        vector<Variable *> selectNodeToSpill();
    };
}
#pragma once

#include <L3.h>
#include <tiling.h>

namespace L3{
  void generate_code(Program p);
  class CodeGen {
    public:
    CodeGen(Tree* tree, std::ofstream &outputFile); 
     void visit(Tile_return_t *t) ;
     void visit(Tile_return *t) ;
     void visit(Tile_br *t) ;
     void visit(Tile_br_t *t) ;
     void visit(Tile_math *t) ;
     void visit(Tile_math_specialized *t) ;
     void visit(Tile_load *t) ;
     void visit(Tile_store *t) ;
     void visit(Tile_assign *t) ;
    private: 
     Tree* tree;
     std::ofstream& outputFile;
  };
}

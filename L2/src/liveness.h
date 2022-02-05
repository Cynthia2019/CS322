#pragma once

#include <L2.h>
#include <map>
#include <analysis.h>

namespace L2
{

  void liveness(Program p);
 // vector<int> get_successor(vector<Instruction *> &instructions, int idx); s

  struct AnalysisResult {
   std::map<Instruction*, std::set<Item*>> gens; 
   std::map<Instruction*, std::set<Item*>> kills; 
   std::map<Instruction*, std::set<Item*>> ins; 
   std::map<Instruction*, std::set<Item*>> outs; 
 };
}
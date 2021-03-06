#pragma once

#include <L2.h>
#include <map>

namespace L2
{

  void liveness(Program p, Function* f);
 // vector<int> get_successor(vector<Instruction *> &instructions, int idx); s
  struct AnalysisResult {
   std::map<Instruction*, std::set<Item*>> gens; 
   std::map<Instruction*, std::set<Item*>> kills; 
   std::map<Instruction*, std::set<Item*>> ins; 
   std::map<Instruction*, std::set<Item*>> outs; 
 };
  std::pair<AnalysisResult*, vector<vector<set<Item*>>>> computeLiveness(Program p, Function* f);
}
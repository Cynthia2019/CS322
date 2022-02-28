#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>
#include <iostream>

#include <LA.h>
#include <parser.h>
#include "coding.h"
#include "codegenerator.h"
#include "memory_check.h"

using namespace std;

void print_help(char *progName)
{
  std::cerr << "Usage: " << progName << " [-v] [-g 0|1] [-O 0|1|2] [-s] [-l] [-i] SOURCE" << std::endl;
  return;
}

bool is_debug = false;

int main(
    int argc,
    char **argv)
{
  auto enable_code_generator = false;
  int32_t optLevel = 3;

  bool memory_check_only = false;


  /*
   * Check the compiler arguments.
   */
  auto verbose = false;
  if (argc < 2)
  {
    print_help(argv[0]);
    return 1;
  }
  int32_t opt;
  while ((opt = getopt(argc, argv, "vg:O:slmt")) != -1)
  {
    switch (opt)
    {

    case 'O':
      optLevel = strtoul(optarg, NULL, 0);
      break;

    case 'g':
      enable_code_generator = (strtoul(optarg, NULL, 0) == 0) ? false : true;
      break;
    
    case 'm':
      memory_check_only = true;  
      break;

    case 'v':
      verbose = true;
      is_debug = true;
      break;
    
    default:
      print_help(argv[0]);
      return 1;
    }
  }
  /*
   * Parse the input file.
   */
  LA::Program p;
/*
    * Parse the L3 program.
    */
  p = LA::parse_file(argv[optind]);

  if (memory_check_only) {
    LA::MemoryCheck mc(p);
    mc.doProgram();
    for (auto f : p.functions) {
      for (auto i : f->instructions) {
        cout << i->toString() << endl;
      }
    }
  } else {
    LA::MemoryCheck mc(p);
    mc.doProgram();
    for (auto f : p.functions) {
      LA::generateBasicBlock(f, mc.LL);
    }
    for (auto f : p.functions) {
      for (auto i : f->instructions) {
        cout << i->toString() << endl;
      }
    }
    for(auto f : p.functions) {
      LA::encodeAll(p, f); 
    }
    LA::generate_code(p);
  }

  return 0;
}

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

#include "LB.h"
#include "parser.h"
#include "scope.h"
#include "codegenerator.h"
#include "loop.h"

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

  bool parse_only = false;


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
  while ((opt = getopt(argc, argv, "vg:O:spt")) != -1)
  {
    switch (opt)
    {

    case 'O':
      optLevel = strtoul(optarg, NULL, 0);
      break;

    case 'g':
      enable_code_generator = (strtoul(optarg, NULL, 0) == 0) ? false : true;
      break;
    
    case 'p':
      parse_only = true;
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
  LB::Program p;
  if (parse_only) {
    p = LB::parse_file(argv[optind]);
    cout << "======parsed========" << endl;
    p.printProgram();
  } else {
    p = LB::parse_file(argv[optind]);
    for (auto f : p.functions) {
      LB::unscope(f);
    }
    // cout << "======parsed========" << endl;
    // p.printProgram();
    cout << "======start codegen=====" << endl;
    LB::generate_code(p);
  }

  return 0;
}

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

#include <L2.h>
#include <parser.h>
#include <liveness.h>
#include <interference.h>
#include "spill.h"

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
  auto enable_code_generator = true;
  auto spill_only = false;
  auto interference_only = false;
  auto liveness_only = false;
  int32_t optLevel = 3;

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
  while ((opt = getopt(argc, argv, "vg:O:sli")) != -1)
  {
    switch (opt)
    {

    case 'l':
      liveness_only = true;
      break;

    case 'i':
      interference_only = true;
      break;

    case 's':
      spill_only = true;
      break;

    case 'O':
      optLevel = strtoul(optarg, NULL, 0);
      break;

    case 'g':
      enable_code_generator = (strtoul(optarg, NULL, 0) == 0) ? false : true;
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
  L2::Program p;
  std::map<L2::Architecture::RegisterID, L2::Register*> registersPtr = {
    {L2::Architecture::rax, new L2::Register(L2::Architecture::rax)},
    {L2::Architecture::rbx, new L2::Register(L2::Architecture::rbx)},
    {L2::Architecture::rcx, new L2::Register(L2::Architecture::rcx)},
    {L2::Architecture::rdx, new L2::Register(L2::Architecture::rdx)},
    {L2::Architecture::rdi, new L2::Register(L2::Architecture::rdi)},
    {L2::Architecture::rsi, new L2::Register(L2::Architecture::rsi)},
    {L2::Architecture::r8, new L2::Register(L2::Architecture::r8)},
    {L2::Architecture::r9, new L2::Register(L2::Architecture::r9)},
    {L2::Architecture::r10, new L2::Register(L2::Architecture::r10)},
    {L2::Architecture::r11, new L2::Register(L2::Architecture::r11)},
    {L2::Architecture::r12, new L2::Register(L2::Architecture::r12)},
    {L2::Architecture::r13, new L2::Register(L2::Architecture::r13)},
    {L2::Architecture::r14, new L2::Register(L2::Architecture::r14)},
    {L2::Architecture::r15, new L2::Register(L2::Architecture::r15)},
    {L2::Architecture::rsp, new L2::Register(L2::Architecture::rsp)}
  };
    p.registers = registersPtr;
  if (spill_only)
  {

    /*
     * Parse an L2 function and the spill arguments.
     */
    p = L2::parse_spill_file(argv[optind]);
  }
  else if (liveness_only)
  {

    /*
     * Parse an L2 function.
     */
    p = L2::parse_function_file(argv[optind]);
  }
  else if (interference_only)
  {

    /*
     * Parse an L2 function.
     */
    p = L2::parse_function_file(argv[optind]);
  }
  else
  {

    /*
     * Parse the L2 program.
     */
    p = L2::parse_file(argv[optind]);
  }
  /*
   * Special cases.
   */
  if (spill_only)
  {
    // TODO
    L2::spill(p);
    return 0;
  }

  /*
   * Liveness test.
   */
  if (liveness_only)
  {
    // TODO
    L2::liveness(p);
    return 0;
  }

  /*
   * Interference graph test.
   */
  if (interference_only)
  {
    // TODO
    //L2::interference(p); 
    return 0;
  }

  /*
   * Generate the target code.
   */
  if (enable_code_generator)
  {
    // TODO
  }

  return 0;
}

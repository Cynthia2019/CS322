/*
 * SUGGESTION FROM THE CC TEAM:
 * double check the order of actions that are fired.
 * You can do this in (at least) two ways:
 * 1) by using gdb adding breakpoints to actions
 * 2) by adding printing statements in each action
 *
 * For 2), we suggest writing the code to make it straightforward to enable/disable all of them 
 * (e.g., assuming shouldIPrint is a global variable
 *    if (shouldIPrint) std::cerr << "MY OUTPUT" << std::endl;
 * )
 */
#include <sched.h>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <assert.h>

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

#include <L1.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;
using namespace std;

namespace L1 {

  /* 
   * Data required to parse
   */ 
  std::vector<Item> parsed_items;

  /*
   *map from string to register
  */
  std::map<std::string, Register> string_to_r; 

  /* 
   * Grammar rules from now on.
   */
  struct name:
    pegtl::seq<
      pegtl::plus< 
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >
        >
      >,
      pegtl::star<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >,
          pegtl::digit
        >
      >
    > {};

  /* 
   * Keywords.
   */
  struct str_return : TAOCPP_PEGTL_STRING( "return" ) {};
  struct str_mem : TAOCPP_PEGTL_STRING( "mem" ) {};
  struct str_arrow : TAOCPP_PEGTL_STRING( "<-" ) {};

  struct comment: 
    pegtl::disable< 
      TAOCPP_PEGTL_STRING( "//" ), 
      pegtl::until< pegtl::eolf > 
    > {};

  struct label:
    pegtl::seq<
      pegtl::one<':'>,
      name
    > {};

  struct register_rule:
    pegtl::sor<
      TAOCPP_PEGTL_STRING( "rdi" ),
      TAOCPP_PEGTL_STRING( "rax" ),
      TAOCPP_PEGTL_STRING( "rdx" ),
      TAOCPP_PEGTL_STRING( "rsi" ),
      TAOCPP_PEGTL_STRING( "rbx" ),
      TAOCPP_PEGTL_STRING( "rcx" ),
      TAOCPP_PEGTL_STRING( "rbp" ),
      TAOCPP_PEGTL_STRING( "r8" ),
      TAOCPP_PEGTL_STRING( "r9" ),
      TAOCPP_PEGTL_STRING( "r10" ),
      TAOCPP_PEGTL_STRING( "r11" ),
      TAOCPP_PEGTL_STRING( "r12" ),
      TAOCPP_PEGTL_STRING( "r13" ),
      TAOCPP_PEGTL_STRING( "r14" ),
      TAOCPP_PEGTL_STRING( "r15" ),
      TAOCPP_PEGTL_STRING( "rsp" )
    > {};

  struct number:
    pegtl::seq<
      pegtl::opt<
        pegtl::sor<
          pegtl::one< '-' >,
          pegtl::one< '+' >
        >
      >,
      pegtl::plus< 
        pegtl::digit
      >
    >{};

  struct function_name:
    label {};

  struct argument_number:
    number {};

  struct local_number:
    number {} ;

  struct seps: 
    pegtl::star< 
      pegtl::sor< 
        pegtl::ascii::space, 
        comment 
      > 
    > {};

  struct Label_rule:
    label {};

  struct Instruction_return_rule:
    pegtl::seq<
      str_return
    > { };

  struct Instruction_assignment_rule:
    pegtl::seq<
      register_rule,
      seps,
      str_arrow,
      seps,
      register_rule
    > {};
  
  struct load_offset_rule: 
    number {};

  struct Instruction_load_rule:
     pegtl::seq<
        register_rule,
        seps,
        str_arrow,
        seps,
	      str_mem,
        seps,
        register_rule, 
        seps, 
        load_offset_rule
        >
    {};

  struct Instruction_store_rule:
     pegtl::seq<
	      str_mem,
        seps,
        register_rule, 
        seps, 
        load_offset_rule, 
        seps, 
        str_arrow, 
        seps, 
        register_rule
        >
    {};
  // += -= *= &=
  struct aops_rule: 
    pegtl::sor<
      TAOCPP_PEGTL_STRING( "+=" ),
      TAOCPP_PEGTL_STRING( "-=" ),
      TAOCPP_PEGTL_STRING( "*=" ),
      TAOCPP_PEGTL_STRING( "&=" )
    > {};
  struct aops_value_rule: 
    number {};
  // w aop t
  struct Instruction_arithmetic_rule: 
    pegtl::seq<
      register_rule,
      seps,  
      aops_rule, 
      seps, 
      pegtl::sor<
        aops_value_rule, 
        register_rule
      >
    >{};

  
  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_return_rule>            , Instruction_return_rule             >,
      pegtl::seq< pegtl::at<Instruction_assignment_rule>        , Instruction_assignment_rule         >,
      pegtl::seq< pegtl::at<Instruction_load_rule>        , Instruction_load_rule        >,
      pegtl::seq< pegtl::at<Instruction_store_rule>        , Instruction_store_rule        >,
      pegtl::seq< pegtl::at<Instruction_arithmetic_rule>        , Instruction_arithmetic_rule        >
    > { };

  struct Instructions_rule:
    pegtl::plus<
      pegtl::seq<
        seps,
        Instruction_rule,
        seps
      >
    > { };

  struct Function_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      function_name,
      seps,
      argument_number,
      seps,
      local_number,
      seps,
      Instructions_rule,
      seps,
      pegtl::one< ')' >
    > {};

  struct Functions_rule:
    pegtl::plus<
      seps,
      Function_rule,
      seps
    > {};

  struct entry_point_rule:
    pegtl::seq<
      seps,
      pegtl::one< '(' >,
      seps,
      label,
      seps,
      Functions_rule,
      seps,
      pegtl::one< ')' >,
      seps
    > { };

  struct grammar : 
    pegtl::must< 
      entry_point_rule
    > {};

  /* 
   * Actions attached to grammar rules.
   */
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};

  template<> struct action < label > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if (p.entryPointLabel.empty()){
        p.entryPointLabel = in.string();
        string_to_r["rax"] = rax; 
        string_to_r["rbx"] = rbx; 
        string_to_r["rcx"] = rcx; 
        string_to_r["rdx"] = rdx;         
        string_to_r["rdi"] = rdi; 
        string_to_r["rsi"] = rsi; 
        string_to_r["rbp"] = rbp; 
        string_to_r["r8"] = r8; 
        string_to_r["r9"] = r9; 
        string_to_r["r10"] = r10; 
        string_to_r["r11"] = r11; 
        string_to_r["r12"] = r12; 
        string_to_r["r13"] = r13; 
        string_to_r["r14"] = r14; 
        string_to_r["r15"] = r15; 
        string_to_r["rsp"] = rsp;
      } else {
        abort();
      }
    }
  };

  template<> struct action < function_name > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto newF = new Function();
      newF->name = in.string();
      p.functions.push_back(newF);
    }
  };

  template<> struct action < argument_number > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      currentF->arguments = std::stoll(in.string());
    }
  };

  template<> struct action < local_number > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      currentF->locals = std::stoll(in.string());
    }
  };

  template<> struct action < str_return > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction_ret();
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      Item i;
      i.isARegister = false;
      i.labelName = in.string();
      parsed_items.push_back(i);
    }
  };

  template<> struct action < register_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      Item i;
      i.isARegister = true;
      i.r = string_to_r[in.string()];
      parsed_items.push_back(i);
    }
  };

  template<> struct action < load_offset_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      Item i;
      i.num = std::stoll(in.string());
      i.isARegister = false;
      i.isAConstant = true;
      parsed_items.push_back(i);
    }
  };
  //action for += -= *= &= 
  template<> struct action < aops_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      Item i; 
      i.isAnOp = true; 
      i.op = in.string(); 
      cout << "op: " << i.op << endl; 
      parsed_items.push_back(i);
    }
  }; 
  //action for aops value when value is a number
  template<> struct action < aops_value_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      Item i; 
      i.isAConstant = true; 
      i.num = std::stoll(in.string()); 
      cout << "aops number: " << i.num << endl; 
      parsed_items.push_back(i);
    }
  }; 
  //action for w aop t
  template<> struct action < Instruction_arithmetic_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_aop(); 
      i->src = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->op = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->dst = parsed_items.back(); 
      parsed_items.pop_back(); 

      currentF->instructions.push_back(i); 
    }
  }; 

  template<> struct action < Instruction_assignment_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      /* 
       * Fetch the current function.
       */ 
      auto currentF = p.functions.back();

      /* 
       * Create the instruction.
       */ 
      auto i = new Instruction_assignment();
      i->src = parsed_items.back();
      parsed_items.pop_back();
      i->dst = parsed_items.back();
      parsed_items.pop_back();

      /* 
       * Add the just-created instruction to the current function.
       */ 
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_load_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){

      auto currentF = p.functions.back();

      auto i = new Instruction_load();
      i->constant = parsed_items.back();
      parsed_items.pop_back(); 
      i->src = parsed_items.back();
      parsed_items.pop_back();
      i->dst = parsed_items.back();
      parsed_items.pop_back();

      currentF->instructions.push_back(i);
    }
  };
  template<> struct action < Instruction_store_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){

      auto currentF = p.functions.back();

      auto i = new Instruction_store();
      i->src = parsed_items.back();
      parsed_items.pop_back(); 
      i->constant = parsed_items.back();
      parsed_items.pop_back();
      i->dst = parsed_items.back();
      parsed_items.pop_back();

      currentF->instructions.push_back(i);
    }
  };


  Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< grammar >();

    /*
     * Parse.
     */   
    file_input< > fileInput(fileName);
    Program p;
    parse< grammar, action >(fileInput, p);

    return p;
  }

}

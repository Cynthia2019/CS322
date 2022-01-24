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

#include <L2.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;
using namespace std;

extern bool is_debug;
namespace L2 {


  /* 
   * Data required to parse
   */ 
  std::vector<Item *> parsed_items;

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
  struct str_call : TAOCPP_PEGTL_STRING( "call" ) {};
  struct str_arrow : TAOCPP_PEGTL_STRING( "<-" ) {};
  struct str_shift_left : TAOCPP_PEGTL_STRING( "<<=" ) {};
  struct str_shift_right : TAOCPP_PEGTL_STRING( ">>=" ) {};
  struct str_stack_arg : TAOCPP_PEGTL_STRING( "stack-arg" ) {};

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
  struct function_name:
    label {};

  struct argument_number:
    number {};

  struct number_rule: 
    number {};

  struct variable_rule:
    pegtl::seq<
      pegtl::one<'%'>,
      name
    >{};

  struct Label_rule:
    label {};


  struct seps: 
    pegtl::star< 
      pegtl::sor< 
        pegtl::ascii::space, 
        comment 
      > 
    > {};

  struct Instruction_return_rule:
    pegtl::seq<
      str_return
    > { };

  struct Instruction_assignment_rule:
    pegtl::seq<
      pegtl::sor<register_rule, variable_rule>,
      seps,
      str_arrow,
      seps,
      pegtl::sor<
        register_rule,
        number_rule, 
        Label_rule, 
        variable_rule
      >
    > {};

  struct shift_op_rule:
  pegtl::sor< str_shift_left, str_shift_right>{};
  
  struct Instruction_shift_rule:
      pegtl::seq<
        pegtl::sor<variable_rule, register_rule>,
        seps,
        shift_op_rule,
        seps,
        pegtl::sor<number_rule, register_rule>
    > {};

  struct Instruction_load_rule:
     pegtl::seq<
        pegtl::sor<variable_rule, register_rule>,
        seps,
        str_arrow,
        seps,
	      str_mem,
        seps,
        pegtl::sor<variable_rule, register_rule>,
        seps, 
        number_rule
        >
    {};

  struct Instruction_store_rule:
     pegtl::seq<
	      str_mem,
        seps,
        pegtl::sor<variable_rule, register_rule>,
        seps, 
        number_rule, 
        seps, 
        str_arrow, 
        seps, 
        pegtl::sor<
          register_rule, 
          number_rule,
          Label_rule>
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
  // w aop t
  struct Instruction_arithmetic_rule: 
    pegtl::seq<
      pegtl::sor<variable_rule, register_rule>,
      seps,  
      aops_rule, 
      seps, 
      pegtl::sor<
        number_rule, 
        register_rule,
        variable_rule
      >
    >{};

  /*
   Memory arithmetic operations
  */
 //mem x M += t
  struct Instruction_store_aop_rule: 
    pegtl::seq<
      str_mem, 
      seps, 
      pegtl::sor<variable_rule, register_rule>, 
      seps, 
      number_rule, 
      seps, 
      aops_rule, 
      seps, 
      pegtl::sor<
        number_rule, 
        register_rule,
        variable_rule
      >
    > {};
  //w += mem x M 
  struct Instruction_load_aop_rule: 
    pegtl::seq<
      pegtl::sor<variable_rule, register_rule>, 
      seps, 
      aops_rule, 
      seps, 
      str_mem, 
      seps, 
      pegtl::sor<variable_rule, register_rule>, 
      seps, 
      number_rule 
    > {};
  
  struct compare_op_rule:
    pegtl::sor<
      TAOCPP_PEGTL_STRING( "<=" ),
      TAOCPP_PEGTL_STRING( "<" ),
      TAOCPP_PEGTL_STRING( "=" )
    > {};

  struct Instruction_compare_rule:
    pegtl::seq<
      pegtl::sor<variable_rule, register_rule>,
      seps,
      str_arrow,
      seps,
      pegtl::sor<number_rule, register_rule, variable_rule>,
      seps,
      compare_op_rule,
      seps,
      pegtl::sor<number_rule, register_rule, variable_rule>
    > {};

  struct Instruction_cjump_rule:
    pegtl::seq<
      TAOCPP_PEGTL_STRING( "cjump" ),
      seps,
      pegtl::sor<number_rule, register_rule, variable_rule>,
      seps,
      compare_op_rule,
      seps,
      pegtl::sor<number_rule, register_rule, variable_rule>,
      seps,
      Label_rule
    > {};

  //w <- stack_arg M 
  struct Instruction_stack_rule: 
    pegtl::seq<
      pegtl::sor<variable_rule, register_rule>,
      seps, 
      str_stack_arg,
      seps, 
      number_rule
    > {};
  /*
  call 
  */
 struct Instruction_call_rule: 
  pegtl::seq<
    str_call, 
    seps,
    pegtl::sor<
      Label_rule,
      register_rule, 
      variable_rule
    >,
    seps, 
    number_rule
  > {}; 

  struct Instruction_call_print_rule: 
    TAOCPP_PEGTL_STRING( "call print 1" ) {}; 

  struct Instruction_call_input_rule: 
    TAOCPP_PEGTL_STRING( "call input 0" ) {}; 

  struct Instruction_call_allocate_rule: 
    TAOCPP_PEGTL_STRING( "call allocate 2" ) {}; 

  struct Instruction_call_error_rule: 
    pegtl::seq<
      TAOCPP_PEGTL_STRING( "call tensor-error" ),
      seps,
      number_rule
    > {}; 
  
  /*
  misc
  */
  struct Instruction_increment_rule: 
    pegtl::seq<
      pegtl::sor<variable_rule, register_rule>, 
      seps,
      TAOCPP_PEGTL_STRING( "++" )
    > {}; 
  struct Instruction_decrement_rule: 
    pegtl::seq<
      pegtl::sor<variable_rule, register_rule>,
      seps,
      TAOCPP_PEGTL_STRING( "--" )
    > {}; 

  struct Instruction_at_rule: 
    pegtl::seq<
      pegtl::sor<variable_rule, register_rule>,
      seps,
      TAOCPP_PEGTL_STRING( "@" ), 
      seps, 
      pegtl::sor<variable_rule, register_rule>,
      seps, 
      pegtl::sor<variable_rule, register_rule>,
      seps, 
      number_rule
    > {}; 
  struct Instruction_goto_rule:
    pegtl::seq<
      TAOCPP_PEGTL_STRING( "goto" ),
      seps, 
      Label_rule
    > 
  {};
  struct Instruction_label_rule: 
   label {}; 
  struct Instruction_rule:
    pegtl::sor<
      pegtl::seq< pegtl::at<Instruction_return_rule>            , Instruction_return_rule           >,
      pegtl::seq< pegtl::at<Instruction_compare_rule>           , Instruction_compare_rule          >,
      pegtl::seq< pegtl::at<Instruction_store_aop_rule>         , Instruction_store_aop_rule        >,
      pegtl::seq< pegtl::at<Instruction_load_aop_rule>          , Instruction_load_aop_rule         >,
      pegtl::seq< pegtl::at<Instruction_assignment_rule>        , Instruction_assignment_rule       >,
      pegtl::seq< pegtl::at<Instruction_load_rule>              , Instruction_load_rule             >,
      pegtl::seq< pegtl::at<Instruction_store_rule>             , Instruction_store_rule            >,
      pegtl::seq< pegtl::at<Instruction_arithmetic_rule>        , Instruction_arithmetic_rule       >,
      pegtl::seq< pegtl::at<Instruction_shift_rule>             , Instruction_shift_rule            >,
      pegtl::seq< pegtl::at<Instruction_stack_rule>             , Instruction_stack_rule            >,
      pegtl::seq< pegtl::at<Instruction_cjump_rule>             , Instruction_cjump_rule            >,
      pegtl::seq< pegtl::at<Instruction_call_rule>              , Instruction_call_rule             >,
      pegtl::seq< pegtl::at<Instruction_call_print_rule>        , Instruction_call_print_rule       >,
      pegtl::seq< pegtl::at<Instruction_call_input_rule>        , Instruction_call_input_rule       >,
      pegtl::seq< pegtl::at<Instruction_call_allocate_rule>     , Instruction_call_allocate_rule    >,
      pegtl::seq< pegtl::at<Instruction_call_error_rule>        , Instruction_call_error_rule       >,
      pegtl::seq< pegtl::at<Instruction_increment_rule>         , Instruction_increment_rule        >,
      pegtl::seq< pegtl::at<Instruction_decrement_rule>         , Instruction_decrement_rule        >,
      pegtl::seq< pegtl::at<Instruction_goto_rule>              , Instruction_goto_rule             >,
      pegtl::seq< pegtl::at<Instruction_at_rule>                , Instruction_at_rule               >, 
      pegtl::seq< pegtl::at<Instruction_label_rule>             , Instruction_label_rule            >
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
  
  struct function_only :
    pegtl::must<
      Function_rule
    > {};

  /* 
   * Actions attached to grammar rules.
   */
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};

  template<> struct action < label > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if (is_debug) cout << "firing label, str: " << in.string() << endl;
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
      if (is_debug) cout << "firing function_name, str: " << in.string() << endl;
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

  template<> struct action < str_return > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back();
      auto i = new Instruction_ret();
      // i->instructionName = "return";
      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Label_rule > {
    template< typename Input >
	static void apply( const Input & in, Program & p){
      if (is_debug) cout << "firing Label_rule, str: " << in.string() << endl;
      Item_label *i = new Item_label();
      i->labelName = in.string();
      parsed_items.push_back(i);
    }
  };

  template<> struct action < register_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (is_debug) cout << "firing register_rule, str: " << in.string() << endl;
      Item_register *i = new Item_register();
      i->register_name= in.string(); 
      parsed_items.push_back(i);
    }
  };

  template<> struct action < variable_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (is_debug) cout << "firing variable_rule, str: " << in.string() << endl;
      Item_variable *i = new Item_variable();
      i->variable_name= in.string(); 
      parsed_items.push_back(i);
    }
  };

  //action for += -= *= &= 
  template<> struct action < aops_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      Item_op *i = new Item_op(); 
      i->op = in.string(); 
      parsed_items.push_back(i);
    }
  }; 
  //action when value is a number
  template<> struct action < number_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      Item_number *i = new Item_number(); 
      i->num = std::stoll(in.string()); 
      parsed_items.push_back(i);
    }
  }; 

  //action for :label
  template<> struct action < Instruction_label_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_label(); 
      Item_label *item = new Item_label();
      item->labelName = in.string();  
      // i->instructionName = "label";
      i->label = item;

      currentF->instructions.push_back(i); 
    }
  }; 
  //action for w aop t
  template<> struct action < Instruction_arithmetic_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_aop(); 
      // i->instructionName = "aop";
      i->src = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->op = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->dst = parsed_items.back(); 
      parsed_items.pop_back(); 

      currentF->instructions.push_back(i); 
    }
  }; 

  //action for mem x M += t and mem x M -= t
  template<> struct action < Instruction_store_aop_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_store_aop(); 
      // i->instructionName = "store_aop"; 
      i->src = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->op = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->constant = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->dst = parsed_items.back(); 
      parsed_items.pop_back(); 

      currentF->instructions.push_back(i); 
    }
  }; 

  //action for w += mem x M and w -= mem x M
  template<> struct action < Instruction_load_aop_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_load_aop(); 
      // i->instructionName = "load_aop"; 
      i->constant = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->src = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->op = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->dst = parsed_items.back(); 
      parsed_items.pop_back(); 

      currentF->instructions.push_back(i); 
    }
  }; 

  //action for w <- stack-arg M 
  template<> struct action < Instruction_stack_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_stack(); 
      i->src = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->dst = parsed_items.back(); 
      parsed_items.pop_back(); 

      currentF->instructions.push_back(i); 
    }
  }; 
  /*
   call actions 
  */
  //action for call u N
  template<> struct action < Instruction_call_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_call(); 
      // i->instructionName = "call"; 
      i->constant = parsed_items.back(); 
      parsed_items.pop_back(); 
      i->dst = parsed_items.back(); 
      parsed_items.pop_back(); 

      currentF->instructions.push_back(i); 
    }
  }; 

  //action for call print 1
  template<> struct action < Instruction_call_print_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_call_print(); 
      // i->instructionName = "call_print"; 
      currentF->instructions.push_back(i); 
    }
  };
  //action for call input 0 
  template<> struct action < Instruction_call_input_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_call_input(); 
      // i->instructionName = "call_input"; 
      currentF->instructions.push_back(i); 
    }
  };
  //action for call allocate 2
  template<> struct action < Instruction_call_allocate_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_call_allocate(); 
      // i->instructionName = "call_allocate"; 
      currentF->instructions.push_back(i); 
    }
  };

  //action for call tensor-error F
  template<> struct action < Instruction_call_error_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_call_error(); 
      // i->instructionName = "call_error"; 
      i->constant = parsed_items.back(); 
      parsed_items.pop_back();
      currentF->instructions.push_back(i); 
    }
  };

  //action for increment w++
  template<> struct action < Instruction_increment_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_increment(); 
      // i->instructionName = "increment"; 
      i->src = parsed_items.back(); 
      parsed_items.pop_back();
      currentF->instructions.push_back(i); 
    }
  };
  //action for increment w--
  template<> struct action < Instruction_decrement_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_decrement(); 
      // i->instructionName = "decrement"; 
      i->src = parsed_items.back(); 
      parsed_items.pop_back();
      currentF->instructions.push_back(i); 
    }
  };
 //action for w @ w w E 
  template<> struct action < Instruction_at_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_at(); 
      // i->instructionName = "at"; 
      i->constant = parsed_items.back(); 
      parsed_items.pop_back();
      i->src_mult = parsed_items.back();
      parsed_items.pop_back();
      i->src_add = parsed_items.back();
      parsed_items.pop_back();
      i->dst = parsed_items.back();
      parsed_items.pop_back();
      currentF->instructions.push_back(i); 
    }
  };

  //action for goto label 
  template<> struct action < Instruction_goto_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto currentF = p.functions.back(); 
      auto i = new Instruction_goto(); 
      // i->instructionName = "goto"; 
      i->label = parsed_items.back(); 
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
      // i->instructionName = "assignment"; 
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
      // i->instructionName = "load"; 
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
      if (is_debug) cout << "firing Instruction_store_rule, str: " << in.string() << endl;

      auto currentF = p.functions.back();

      auto i = new Instruction_store();
      // i->instructionName = "store"; 
      i->src = parsed_items.back();
      parsed_items.pop_back(); 
      i->constant = parsed_items.back();
      parsed_items.pop_back();
      i->dst = parsed_items.back();
      parsed_items.pop_back();

      currentF->instructions.push_back(i);
    }
  };
  
  template<> struct action < shift_op_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (is_debug) cout << "firing shift_op_rule, str: " << in.string() << endl;
      Item_op *i = new Item_op();
      i->op = in.string();
      parsed_items.push_back(i);
    }
  };

  
  template<> struct action < Instruction_shift_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (is_debug) cout << "firing Instruction_shift_rule, str: " << in.string() << endl;

      auto currentF = p.functions.back();

      auto i = new Instruction_shift();
      // i->instructionName = "shift"; 
      i->src = parsed_items.back();
      parsed_items.pop_back();
      i->op = parsed_items.back();
      parsed_items.pop_back();
      i->dst = parsed_items.back();
      parsed_items.pop_back();

      currentF->instructions.push_back(i);
    }
  };
  
  template<> struct action < compare_op_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (is_debug) cout << "firing compare_op_rule, str: " << in.string() << endl;
      Item_op *i = new Item_op();
      i->op = in.string();
      parsed_items.push_back(i);
    }
  };
  
  template<> struct action < Instruction_compare_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (is_debug) cout << "firing Instruction_compare_rule, str: " << in.string() << endl;

      auto currentF = p.functions.back();

      auto i = new Instruction_compare();
      // i->instructionName = "compare"; 
      i->oprand2 = parsed_items.back();
      parsed_items.pop_back();
      i->op = parsed_items.back();
      parsed_items.pop_back();
      i->oprand1 = parsed_items.back();
      parsed_items.pop_back();
      i->dst = parsed_items.back();
      parsed_items.pop_back();

      currentF->instructions.push_back(i);
    }
  };

  template<> struct action < Instruction_cjump_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      if (is_debug) cout << "firing Instruction_cjump_rule, str: " << in.string() << endl;
      auto currentF = p.functions.back();

      auto i = new Instruction_cjump();
      // i->instructionName = "cjump"; 
      i->label = parsed_items.back();
      parsed_items.pop_back();
      i->oprand2 = parsed_items.back();
      parsed_items.pop_back();
      i->op = parsed_items.back();
      parsed_items.pop_back();
      i->oprand1 = parsed_items.back();
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

  Program parse_spill_file(char *fileName) {
    Program p;
    return p;
  }

  Program parse_function_file(char *fileName) {
    /* 
     * Check the grammar for some possible issues.
     */
    pegtl::analyze< function_only >();

    /*
     * Parse.
     */   
    file_input< > fileInput(fileName);
    Program p;
    parse< function_only, action >(fileInput, p);
    return p;
  }
}

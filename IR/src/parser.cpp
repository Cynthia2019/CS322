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
// #include <sched.h>
// #include <string>
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
#include <map>

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

#include <IR.h>
#include <parser.h>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

using namespace pegtl;
using namespace std;

extern bool is_debug;
namespace IR
{
  /*
   * Data required to parse
   */
  std::vector<Item *> parsed_items = {};
  std::vector<Item *> list_of_args = {}; 
  std::vector<Instruction *> instructions = {};
  VarTypes curr_var_type;

  /*
   *map from string to registerid
   */

  /*
   * Grammar rules from now on.
   */
  struct name : pegtl::seq<
                    pegtl::plus<
                        pegtl::sor<
                            pegtl::alpha,
                            pegtl::one<'_'>>>,
                    pegtl::star<
                        pegtl::sor<
                            pegtl::alpha,
                            pegtl::one<'_'>,
                            pegtl::digit>>>
  {
  };

  /*
   * Keywords.
   */
  struct str_return : TAOCPP_PEGTL_STRING("return") {};
  struct str_call : TAOCPP_PEGTL_STRING("call") {};
  struct str_br : TAOCPP_PEGTL_STRING("br") {};
  struct str_void : TAOCPP_PEGTL_STRING("void") {};
  struct str_tuple : TAOCPP_PEGTL_STRING("tuple") {};
  struct str_code : TAOCPP_PEGTL_STRING("code") {};
  struct str_int64 : TAOCPP_PEGTL_STRING("int64") {};
  struct str_length : TAOCPP_PEGTL_STRING("length") {}; 
  struct str_new : TAOCPP_PEGTL_STRING("new") {}; 
  struct str_Array : TAOCPP_PEGTL_STRING("Array") {}; 
  struct str_Tuple : TAOCPP_PEGTL_STRING("Tuple") {}; 
  struct str_arrow : TAOCPP_PEGTL_STRING("<-") {};

  struct comment : pegtl::disable<
                       TAOCPP_PEGTL_STRING("//"),
                       pegtl::until<pegtl::eolf>> {};

  struct label : pegtl::seq<
                     pegtl::one<':'>,
                     name> {};
  struct seps : pegtl::star<
                    pegtl::sor<
                        pegtl::ascii::space,
                        comment>> {};
  struct number : pegtl::seq<
                      pegtl::opt<
                          pegtl::sor<
                              pegtl::one<'-'>,
                              pegtl::one<'+'>>>,
                      pegtl::plus<
                          pegtl::digit>> {};
  struct variable : pegtl::seq<
                             pegtl::one<'%'>,
                             name> {};
  struct function_name : label {};


  struct number_rule : number {};

  struct type_rule : pegtl::sor<
                      str_void, 
                      str_tuple, 
                      str_code, 
                      pegtl::seq<
                        str_int64, 
                        pegtl::star<
                          TAOCPP_PEGTL_STRING("[]")>
                          >
                      > {};
  struct function_type : type_rule {};
  struct variable_rule : variable {};
  struct type_variable_rule : pegtl::seq<
                              type_rule, 
                              seps,
                              variable> {};
  struct parameters_rule : pegtl::sor<
                            pegtl::seq<
                              type_variable_rule,
                              pegtl::star<
                                pegtl::seq<
                                  pegtl::one<','>,
                                  pegtl::sor<seps, pegtl::eol>,
                                  type_variable_rule>
                                >
                              >,
                              seps> {};
  struct args_rule :   pegtl::sor<
                                pegtl::seq<
                                    pegtl::sor<variable_rule, number_rule>,
                                    pegtl::star<
                                        pegtl::seq<
                                            pegtl::one<','>,
                                            pegtl::sor<seps, pegtl::eol>,
                                            pegtl::sor<variable_rule, number_rule>
                                                >
                                            >
                                        >,
                                seps
                                    > {};

  struct Label_rule : label {};
  
  struct op_rule : pegtl::sor<TAOCPP_PEGTL_STRING("<<"), 
                            TAOCPP_PEGTL_STRING(">>"),
                            pegtl::one<'+'>, 
                            pegtl::one<'-'>, 
                            pegtl::one<'*'>,
                            pegtl::one<'&'>,
                            TAOCPP_PEGTL_STRING(">="),
                            TAOCPP_PEGTL_STRING("<="),
                            TAOCPP_PEGTL_STRING("<"),
                            TAOCPP_PEGTL_STRING(">"),
                            TAOCPP_PEGTL_STRING("=")
                            >
  {
  };

  struct Instruction_return_rule : str_return {};
  struct Instruction_return_t_rule : pegtl::seq<
                                       str_return,
                                       seps,
                                       pegtl::sor<variable_rule, 
                                                  number_rule>> {};

  struct Instruction_declare_rule : type_variable_rule {};

  struct Instruction_assignment_rule : pegtl::seq<
                                           variable_rule,
                                           seps,
                                           str_arrow,
                                           seps,
                                           pegtl::sor<
                                               number_rule,
                                               Label_rule,
                                               variable_rule>>
  {
  };

  struct Instruction_op_rule : pegtl::seq<
                                      variable_rule,
                                      seps,
                                      str_arrow,
                                      seps, 
                                      pegtl::sor<variable_rule, number_rule>,
                                      seps,
                                      op_rule,
                                      seps,
                                      pegtl::sor<number_rule, variable_rule>>
  {
  };

  struct indices_rule : pegtl::plus<
                          pegtl::seq<
                            pegtl::one<'['>,
                            pegtl::sor<number_rule, variable_rule>, 
                            pegtl::one<']'>
                            >
                          >{};

  struct Instruction_load_rule : pegtl::seq<
                                     variable_rule,
                                     seps,
                                     str_arrow,
                                     seps,
                                     variable_rule,
                                     indices_rule
                                    >
  {
  };

  struct Instruction_store_rule : pegtl::seq<
                                    variable_rule,
                                     indices_rule,
                                      seps,
                                      str_arrow,
                                      seps,
                                      pegtl::sor<
                                          number_rule,
                                          Label_rule,
                                          variable_rule>>
  {
  };

  /*
  call
  */
  struct call_string_rule : pegtl::sor<
                                TAOCPP_PEGTL_STRING("print"), 
                                TAOCPP_PEGTL_STRING("allocate"), 
                                TAOCPP_PEGTL_STRING("input"), 
                                TAOCPP_PEGTL_STRING("tensor-error")> {};
  struct Instruction_call_rule : pegtl::seq<
                                     str_call,
                                     seps,
                                     pegtl::sor<
                                         Label_rule,
                                         variable_rule,
                                         call_string_rule>,
                                     seps,
                                     TAOCPP_PEGTL_STRING("("),
                                     pegtl::sor<seps, pegtl::eol>,
                                     args_rule,
                                     pegtl::sor<seps, pegtl::eol>,
                                     TAOCPP_PEGTL_STRING(")")>
  {
  };
  struct Instruction_call_assignment_rule : pegtl::seq<
                                    variable_rule, 
                                    seps, 
                                    str_arrow, 
                                    seps, 
                                    str_call,
                                    seps,
                                     pegtl::sor<
                                         Label_rule,
                                         variable_rule,
                                         call_string_rule>,
                                    seps,
                                     TAOCPP_PEGTL_STRING("("),
                                     pegtl::sor<seps, pegtl::eol>,
                                     args_rule,
                                     pegtl::sor<seps, pegtl::eol>,
                                     TAOCPP_PEGTL_STRING(")")>
  {
  };

  struct Instruction_br_label_rule : pegtl::seq<
                                     str_br,
                                     seps,
                                     Label_rule>
  {
  };
  struct Instruction_br_t_rule : pegtl::seq<
                                     str_br,
                                     seps,
                                     pegtl::sor<variable_rule, number_rule>,
                                     seps, 
                                     Label_rule,
                                     seps, 
                                     Label_rule>
  {
  };
  struct Instruction_length_rule : pegtl::seq<
                                     variable_rule,
                                     seps,
                                     str_arrow, 
                                     seps, 
                                     str_length, 
                                     seps, 
                                     variable_rule,
                                     seps, 
                                     pegtl::sor<variable_rule, number_rule>>
  {
  };
  struct Instruction_array_rule : pegtl::seq<
                                     variable_rule,
                                     seps,
                                     str_arrow, 
                                     seps, 
                                     str_new, 
                                     seps,
                                     str_Array, 
                                     seps, 
                                     TAOCPP_PEGTL_STRING("("),
                                     pegtl::sor<seps, pegtl::eol>,
                                     args_rule,
                                     pegtl::sor<seps, pegtl::eol>,
                                     TAOCPP_PEGTL_STRING(")")>
  {
  };
  struct Instruction_tuple_rule : pegtl::seq<
                                     variable_rule,
                                     seps,
                                     str_arrow, 
                                     seps, 
                                     str_new, 
                                     seps,
                                     str_Tuple, 
                                     seps, 
                                     TAOCPP_PEGTL_STRING("("),
                                     pegtl::sor<seps, pegtl::eol>,
                                     pegtl::sor<variable_rule, number_rule>,
                                     pegtl::sor<seps, pegtl::eol>,
                                     TAOCPP_PEGTL_STRING(")")>
  {
  };
  struct  Instruction_label_rule : label {};
  
  struct Instruction_rule : pegtl::sor<
                                // pegtl::seq<pegtl::at<Instruction_label_rule>, Instruction_label_rule>,
                                // pegtl::seq<pegtl::at<Instruction_br_label_rule>, Instruction_br_label_rule>,
                                // pegtl::seq<pegtl::at<Instruction_br_t_rule>, Instruction_br_t_rule>,
                                // pegtl::seq<pegtl::at<Instruction_return_t_rule>, Instruction_return_t_rule>,
                                // pegtl::seq<pegtl::at<Instruction_return_rule>, Instruction_return_rule>,
                                pegtl::seq<pegtl::at<Instruction_op_rule>, Instruction_op_rule>,
                                pegtl::seq<pegtl::at<Instruction_load_rule>, Instruction_load_rule>,
                                pegtl::seq<pegtl::at<Instruction_store_rule>, Instruction_store_rule>,
                                pegtl::seq<pegtl::at<Instruction_call_rule>, Instruction_call_rule>,
                                pegtl::seq<pegtl::at<Instruction_call_assignment_rule>, Instruction_call_assignment_rule>,
                                pegtl::seq<pegtl::at<Instruction_assignment_rule>, Instruction_assignment_rule>,
                                pegtl::seq<pegtl::at<Instruction_length_rule>, Instruction_length_rule>,
                                pegtl::seq<pegtl::at<Instruction_array_rule>, Instruction_array_rule>,
                                pegtl::seq<pegtl::at<Instruction_tuple_rule>, Instruction_tuple_rule>,
                                pegtl::seq<pegtl::at<Instruction_declare_rule>, Instruction_declare_rule>>
  {
  };

  struct Terminator_rule : pegtl::sor<
                        pegtl::seq<pegtl::at<Instruction_br_label_rule>, Instruction_br_label_rule>, 
                        pegtl::seq<pegtl::at<Instruction_br_t_rule>, Instruction_br_t_rule>,
                        pegtl::seq<pegtl::at<Instruction_return_t_rule>, Instruction_return_t_rule>,
                        pegtl::seq<pegtl::at<Instruction_return_rule>, Instruction_return_rule>> {};

  struct Instructions_rule : pegtl::star<
                                 pegtl::seq<
                                     seps,
                                     Instruction_rule,
                                     seps>>
  {
  };

  struct BasicBlock_rule : pegtl::seq<
                                Instruction_label_rule, 
                                seps, 
                                Instructions_rule,
                                seps,
                                Terminator_rule
                                > {};

  struct BasicBlocks_rule : pegtl::plus<
                                 pegtl::seq<
                                     seps,
                                     BasicBlock_rule,
                                     seps>> {};

  struct Function_rule : pegtl::seq<
                             TAOCPP_PEGTL_STRING("define"),
                             seps,
                             function_type, 
                             seps, 
                             function_name,
                             seps,
                             TAOCPP_PEGTL_STRING("("),
                             seps,
                             parameters_rule,
                             seps,
                             TAOCPP_PEGTL_STRING(")"),
                             seps,
                             TAOCPP_PEGTL_STRING("{"),
                             pegtl::sor<seps, pegtl::eol>,
                             BasicBlocks_rule,
                             pegtl::sor<seps, pegtl::eol>,
                             TAOCPP_PEGTL_STRING("}")>
  {
  };

  struct Functions_rule : pegtl::plus<
                              seps,
                              Function_rule,
                              seps>
  {
  };

  struct grammar : pegtl::must<
                       Functions_rule>
  {
  };
  /*
   * Actions attached to grammar rules.
   */
  template <typename Rule>
  struct action : pegtl::nothing<Rule>
  {
  };

  template <>
  struct action<Function_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug) cout << "firing Function_rule" << endl;
    }
  };
  template <>
  struct action<function_name>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug) cout << "new function: " << in.string() << endl;
      auto currentF = p.functions.back(); 
      currentF->name = in.string();
      currentF->isMain = in.string() == ":main";
    }
  };
  template <>
  struct action<function_type>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug) cout << "function type: " << in.string() << endl;
      auto newF = new Function();
      newF->type = in.string();
      p.functions.push_back(newF);
    }
  };

  template <>
  struct action<Instruction_return_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_ret: " << in.string() << endl;
      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back(); 
      auto i = new Instruction_ret_not();
      //i->op = dynamic_cast<Operation*>(parsed_items.back()); 
      i->op = new Operation("return");
      //parsed_items.pop_back();
      bb->te = i; 
      if(is_debug) cout << i->toString() << endl;
    }
  };
  template <>
  struct action<Instruction_return_t_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_ret_t: " << in.string() << endl;
      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back(); 
      auto i = new Instruction_ret_t();
      i->arg = parsed_items.back(); 
      parsed_items.pop_back();
     // i->op = dynamic_cast<Operation*>(parsed_items.back()); 
      i->op = new Operation("return");
      //parsed_items.pop_back();
      bb->te = i;
      if(is_debug) cout << i->toString() << endl;
    }
  };
  template <>
  struct action<Label_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      Label *i = new Label(in.string());
      cout << "pushing label " << endl;
      parsed_items.push_back(i);   
    }
  };

  template <>
  struct action<variable_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing variable_rule: " << in.string() << endl;
      auto currentF = p.functions.back();
      std::string var_name = in.string(); 
      Variable *i = currentF->variables[var_name];
      cout << "pushing variable" << endl;
      parsed_items.push_back(i);
    }
  }; 

  template <>
  struct action<type_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing type_rule: " << in.string() << endl;
      std::string type = in.string();
      if (type.find("]") != std::string::npos) {
        curr_var_type = var_int64_multi;
      } else if (type.find("int64") != std::string::npos) {
        curr_var_type = var_int64;
      } else if (type == "code") {
        curr_var_type = var_code;
      } else if (type == "tuple") {
        curr_var_type = var_tuple;
      }
      // String *i = new String(in.string());
      // parsed_items.push_back(i);
    }
  };      

  template <>
  struct action<type_variable_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing type_variable_rule: " << in.string() << endl;
      auto currentF = p.functions.back();
      std::string type_var = in.string(); 
      int n = type_var.find(" "); 
      std::string type = type_var.substr(0, n); 
      std::string var_name = type_var.substr(n+1); 
      if(is_debug) {
        cout << "type: " << type << " var: " << var_name << endl;
      }
      Variable *v;
      if (curr_var_type == var_int64_multi) {
        int dimension = count(type.begin(), type.end(), ']');
        v = currentF->newVariable(var_name, var_int64_multi, dimension);
      } else {
        v = currentF->newVariable(var_name, curr_var_type, 0);
      }
      cout << "pushing type variable" << endl;
      parsed_items.push_back(v);
    }
  };    

  // template <>
  // struct action<variable>
  // {
  //   template <typename Input>
  //   static void apply(const Input &in, Program &p)
  //   {
  //     cout << "firing variable rule" << endl;
  //   }

  // };   
template <>
  struct action<parameters_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing parameters_rule: " << in.string() << endl;
        auto currentF = p.functions.back();
        while(!parsed_items.empty()){
          cout <<parsed_items.back()->toString() << endl;
          Variable* v = dynamic_cast<Variable*>(parsed_items.back()); 
          if(v == nullptr) cerr << "bug\n"; 
          cout << "param variable: " << v->toString() << endl;
          currentF->arguments.push_back(v); 
          parsed_items.pop_back();
        }
    }
  };
template <>
  struct action<args_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing args_rule: " << in.string() << endl;
      auto currentF = p.functions.back();
      std::string args = in.string(); 
      if(args.size() == 0) return ;
      while(args.find(',') != args.npos){
          int n = args.find(','); 
          //eliminate any space in 0-n
          std::string temp = args.substr(0, n); 
          temp.erase(std::remove_if(temp.begin(), temp.end(), [](unsigned char x){return std::isspace(x);}), temp.end()); 
          if(is_debug) cout << "args: " << temp << endl;
          if(temp[0] == '%'){
            Variable *i = currentF->variables[temp];
            list_of_args.push_back(i);
          }
          else {
              Number* i = new Number(std::stoll(temp)); 
              list_of_args.push_back(i);
          }
          args = args.substr(n+1);
      }
      if(is_debug) cout << "args after parsed: " << args << endl;
      args.erase(std::remove_if(args.begin(), args.end(), [](unsigned char x){return std::isspace(x);}), args.end()); 
      if(args[0] == '%'){
          Variable* i = currentF->variables[args]; 
          list_of_args.push_back(i);
      }
      else {
         Number* i = new Number(std::stoll(args)); 
        list_of_args.push_back(i);
      }
    }
  };
  // action for + - & * << >> < <= > >= =
  template <>
  struct action<op_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing op_rule: " << in.string() << endl;
      Operation *i = new Operation(in.string());
      cout << "pushing op" << endl;
      parsed_items.push_back(i);
    }
  };
  // template <>
  // struct action<str_return>
  // {
  //   template <typename Input>
  //   static void apply(const Input &in, Program &p)
  //   {
  //     if (is_debug)
  //       cout << "firing return_string: " << in.string() << endl;
  //     Operation *i = new Operation(in.string());
  //     parsed_items.push_back(i);
  //   }
  // };
  template <>
  struct action<str_br>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug) cout << "firing str_br rule" << endl;
      Operation *i = new Operation(in.string());
      cout << "pushing br" << endl;
      parsed_items.push_back(i);
    }
  };
  // action when value is a number
  template <>
  struct action<number_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      // if (is_debug)
      //   cout << "firing number_rule: " << in.string() << endl;
      Number *i = new Number(std::stoll(in.string()));
      cout << "pushing number" << endl;
      parsed_items.push_back(i);
    }
  };

  //                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              action when value is a print, allocate, input, tensor-error
  template <>
  struct action<call_string_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      String *i = new String(in.string());
      cout << "pushing call" << endl;
      parsed_items.push_back(i);
    }
  };

  // action for :label
  template <>
  struct action<Instruction_label_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_label_rule: " << in.string() << endl;
      auto currentF = p.functions.back();
      Label *item = new Label(in.string());
      BasicBlock *bb = new BasicBlock(); 
      bb->label = item; 
      currentF->basicBlocks.push_back(bb); 
      if(is_debug) cout << item->toString() << endl;
    }
  };
  // action for var <- t op t
  template <>
  struct action<Instruction_op_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_op_rule: " << in.string() << endl;
      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_op();
      i->oprand2 = parsed_items.back();
      parsed_items.pop_back();
      i->op = parsed_items.back();
      parsed_items.pop_back();
      i->oprand1 = parsed_items.back();
      parsed_items.pop_back();
      i->dst = dynamic_cast<Variable*>(parsed_items.back());
      if(dynamic_cast<Variable*>(parsed_items.back()) == nullptr) cout << "NULL" <<endl;
      parsed_items.pop_back();
      if(is_debug) cout << i->toString() << endl;
      bb->instructions.push_back(i);
      cout << "oprule" << parsed_items.size() << endl;
    }
  };
  /*
   call actions
  */
  // action for call callee (args)
  template <>
  struct action<Instruction_call_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_call_rule: " << in.string() << endl;
      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_call_noassign();
      reverse(list_of_args.begin(), list_of_args.end());
      while(!list_of_args.empty()) {
          i->args.push_back(list_of_args.back());
          list_of_args.pop_back();
          parsed_items.pop_back();
      }
      i->callee = parsed_items.back();
      parsed_items.pop_back();
      if(is_debug) cout << i->toString() << endl;
      bb->instructions.push_back(i);
    }
  };

  template <>
  struct action<Instruction_call_assignment_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_call_assignment_rule: " << in.string() << endl;
      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_call_assignment();
      for(Item* item : list_of_args) {
         i->args.push_back(item);
         parsed_items.pop_back();
      }
      list_of_args = {};
      i->callee = parsed_items.back();
      parsed_items.pop_back();
      i->dst = dynamic_cast<Variable*>(parsed_items.back());;
       parsed_items.pop_back();
      bb->instructions.push_back(i);
    }
  };

  // action for br label
  template <>
  struct action<Instruction_br_label_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_br_rule: " << in.string() << endl;
      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_br_label();
      i->label = dynamic_cast<Label*>(parsed_items.back());;
      parsed_items.pop_back();
      i->op = dynamic_cast<Operation*>(parsed_items.back());;
      parsed_items.pop_back();
      bb->te = i;
      if(is_debug) cout << i->toString() << endl;
      cout << "brrrr" << parsed_items.size() << endl;
    }
  };
  // action for br t label
  template <>
  struct action<Instruction_br_t_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_br_t_rule: " << in.string() << endl;
      cout << "brttt" << parsed_items.size() << endl;
      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_br_t();
      i->label2 = dynamic_cast<Label*>(parsed_items.back());;
      parsed_items.pop_back();
      i->label1 = dynamic_cast<Label*>(parsed_items.back());;
      parsed_items.pop_back();
      i->condition = parsed_items.back();
      parsed_items.pop_back();
      i->op = dynamic_cast<Operation*>(parsed_items.back());;
      parsed_items.pop_back();
      bb->te = i;
      if(is_debug) cout << i->toString() << endl;
      cout << "brttt" << parsed_items.size() << endl;
    }
  };

  template <>
  struct action<Instruction_assignment_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_assignment_rule: " << in.string() << endl;
      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_assignment();
      cout << parsed_items.back()->toString() <<"\n";
      i->src = parsed_items.back();
      parsed_items.pop_back();
      i->dst = dynamic_cast<Variable*>(parsed_items.back());
      cout << parsed_items.back()->toString() <<"\n";
      parsed_items.pop_back();
      if(is_debug) cout << i->toString() << endl;
      bb->instructions.push_back(i);
      cout << "assignment " << parsed_items.size() << endl;
    }
  };

  template <>
  struct action<Instruction_declare_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_declare_rule: " << in.string() << endl;
      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      std::string type_var = in.string(); 
      int n = type_var.find(" "); 
      std::string type = type_var.substr(0, n); 
      std::string var_name = type_var.substr(n+1); 
      if(is_debug) {
        cout << "type: " << type << " var: " << var_name << endl;
      }
      Variable *v;

      if (curr_var_type == var_int64_multi) {
        int dimension = count(type.begin(), type.end(), ']');
        v = currentF->newVariable(var_name, var_int64_multi, dimension);
        cout << "declarering" << var_name << endl;
      } else {
        v = currentF->newVariable(var_name, curr_var_type, 0);
        cout << "declarering" << var_name << endl;
      }

      Instruction_declare* i = new Instruction_declare(); 
      i->dst = v; 
      i->type = v->getVariableType(); 
      bb->instructions.push_back(i);
    }
  };

  template <>
  struct action<indices_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program& p){
      if(is_debug)
        cout << "firing indices_rule: " << in.string() << endl;
      
      std::string indices = in.string(); 
      int n = count(indices.begin(), indices.end(), ']');
      cout << "pushing indice" << endl;
      parsed_items.push_back(new Number(n));
    }
  };

  template <>
  struct action<Instruction_load_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_load_rule: " << in.string() << endl;

      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_load();
      Number* size = dynamic_cast<Number*>(parsed_items.back()); 
      parsed_items.pop_back(); 
      for(int index = 0; index < size->get(); index++){
        i->indices.push_back(parsed_items.back()); 
        parsed_items.pop_back();
      }
      i->src = dynamic_cast<Variable*>(parsed_items.back());
      parsed_items.pop_back();
      i->dst = dynamic_cast<Variable*>(parsed_items.back());
      parsed_items.pop_back();
      if(is_debug) cout << i->toString() << endl;
      bb->instructions.push_back(i);
    }
  };

  template <>
  struct action<Instruction_store_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_store_rule: " << in.string() << endl;

      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_store();
      i->src = parsed_items.back();
      parsed_items.pop_back();
      Number* size = dynamic_cast<Number*>(parsed_items.back()); 
      parsed_items.pop_back(); 
      for(int index = 0; index < size->get(); index++){
        i->indices.push_back(parsed_items.back()); 
        parsed_items.pop_back();
      }
      i->dst = dynamic_cast<Variable*>(parsed_items.back());
      parsed_items.pop_back();
      if(is_debug) cout << i->toString() << endl;
      bb->instructions.push_back(i);
    }
  };


  template <>
  struct action<Instruction_length_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_length_rule: " << in.string() << endl;

      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_length();
      i->dimID = dynamic_cast<Number*>(parsed_items.back());
      parsed_items.pop_back();
      i->src = dynamic_cast<Variable*>(parsed_items.back());
      parsed_items.pop_back();
      i->dst = dynamic_cast<Variable*>(parsed_items.back());
      parsed_items.pop_back();
      if(is_debug) cout << i->toString() << endl;
      bb->instructions.push_back(i);
    }
  };  
 template <>
  struct action<Instruction_array_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_array_rule: " << in.string() << endl;

      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_array();
      reverse(list_of_args.begin(), list_of_args.end());
      while(!list_of_args.empty()) {
          i->args.push_back(list_of_args.back());
          list_of_args.pop_back();
          parsed_items.pop_back();
      }
      i->dst = dynamic_cast<Variable*>(parsed_items.back());
      parsed_items.pop_back();
      if(is_debug) cout << i->toString() << endl;
      bb->instructions.push_back(i);
    }
  };  

 template <>
  struct action<Instruction_tuple_rule>
  {
    template <typename Input>
    static void apply(const Input &in, Program &p)
    {
      if (is_debug)
        cout << "firing Instruction_tuple_rule: " << in.string() << endl;

      auto currentF = p.functions.back();
      BasicBlock* bb = currentF->basicBlocks.back();
      auto i = new Instruction_tuple();
      i->arg = parsed_items.back();
      parsed_items.pop_back();
      i->dst = dynamic_cast<Variable*>(parsed_items.back());
      parsed_items.pop_back();
      if(is_debug) cout << i->toString() << endl;
      bb->instructions.push_back(i);
    }
  };  

  Program parse_file(char *fileName)
  {

    /*
     * Check the grammar for some possible issues.
     */
    pegtl::analyze<grammar>();

    /*
     * Parse.
     */
    file_input<> fileInput(fileName);
    Program p;
    parse<grammar, action>(fileInput, p);

    return p;
  }

}

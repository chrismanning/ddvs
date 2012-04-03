/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <mainfunction.h>
#include <types.h>
#include <QStringList>
#include <QDebug>

typedef std::string::const_iterator iterator_type;

namespace interpreter {
    enum byte_code
    {
        op_neg,         //  negate the top stack entry
        op_add,         //  add top two stack entries
        op_sub,         //  subtract top two stack entries
        op_mul,         //  multiply top two stack entries
        op_div,         //  divide top two stack entries

        op_not,         //  boolean negate the top stack entry
        op_eq,          //  compare the top two stack entries for ==
        op_neq,         //  compare the top two stack entries for !=
        op_lt,          //  compare the top two stack entries for <
        op_lte,         //  compare the top two stack entries for <=
        op_gt,          //  compare the top two stack entries for >
        op_gte,         //  compare the top two stack entries for >=

        op_and,         //  logical and top two stack entries
        op_or,          //  logical or top two stack entries

        op_load,        //  load a variable
        op_store,       //  store a variable

        op_int,         //  push constant integer into the stack
        op_true,        //  push constant 0 into the stack
        op_false,       //  push constant 1 into the stack

        op_jump_if,     //  jump to a relative position in the code if top stack
                        //  evaluates to false
        op_jump,        //  jump to a relative position in the code

        op_stk_adj,     //  adjust the stack (for args and locals)
        op_call,        //  function call
        op_return,      //  return from function
        op_void
    };
    enum type_code
    {
        type_void,
        type_int,
        type_pointer,
        type_struct
    };

    struct cstruct
    {
        cstruct(std::string name, std::map<std::string, int> members)
            : name(name), members(members)
        {

        }
        std::string name;
        std::map<std::string, int> members;
        std::size_t size()
        {
            return members.size();
        }
    };

    struct function
    {
        function(std::map<std::string, int>& global_variables, std::map<std::string, int*>& global_pointers,
                 std::vector<int>& code, std::size_t nargs, std::size_t offset_) :
            global_variables(global_variables),
            global_pointers(global_pointers),
            code(code),
            offset(offset_),
            address(code.size()),
            size_(0),
            nargs_(nargs) {}

        void op(int a);
        void op(int a, int b);
        void op(int a, int b, int c);

        int& operator[](std::size_t i) { return code[address+i]; }
        int const& operator[](std::size_t i) const { return code[address+i]; }
        std::size_t size() const { return size_; }
        std::size_t get_address() const { return address; }

        std::size_t nargs() const { return nargs_; }
        std::size_t nvars() const { return variables.size(); }
        int const* find_var(std::string const& name) const;
        void add_var(std::string const& name);
        void link_to(std::string const& name, std::size_t address);
        std::size_t offset;

    protected:
        std::map<std::string, int> variables;
        std::map<std::string, int>& global_variables;
        std::map<std::string, int*> pointers;
        std::map<std::string, int*>& global_pointers;
        std::map<std::size_t, std::string> function_calls;
        std::vector<int>& code;
        std::size_t address;
        std::size_t size_;
        std::size_t nargs_;
    private:
        std::size_t return_type;
    };


    struct type_handler
    {
        type_handler(std::vector<cstruct>& structs) :
            structs(structs) {}

        void operator()(ast::struct_specifier ss) {
            return;
        }
        std::string operator()(int type_code) {
            return "";
        }
        std::vector<cstruct>& structs;
    };

    struct global : function, public boost::static_visitor<bool> {
        //typedef bool result_type;
        global(std::vector<int>& stack, std::vector<int>& code, error_handler& error__, parser::struct_types& structs) :
                function(variables, pointers, code, 0, 0),
                current_function(this),
                global_function(this),
                void_return(true),
                stack(stack),
                error_(error__),
                th(structs),
                structs_table(structs)
        {
            void_return = true;
        }
        void error(int id, std::string const& what)
        {
            error_("Error! ", what, error_.iters[id]);
        }

        bool operator()(ast::nil) { assert(0); return false; }
        bool operator()(unsigned int ast);
        bool operator()(bool ast);
        bool operator()(ast::identifier const& ast);
        bool operator()(ast::optoken const& ast);
        bool operator()(ast::unary const& ast);
        bool operator()(ast::function_call const& ast);
        bool operator()(ast::expression const& ast);
        bool operator()(ast::assignment const& ast);
        bool operator()(ast::declaration const& ast);
        bool operator()(ast::statement_list const& ast);
        bool operator()(ast::statement const& ast);
        bool operator()(ast::if_statement const& ast);
        bool operator()(ast::while_statement const& ast);
        bool operator()(ast::return_statement const& ast);
        bool operator()(ast::function const& ast);
        bool operator()(ast::function_list const& ast);
//        bool operator()(ast::struct_member_declaration const& ast);
        bool operator()(ast::struct_specifier const& ast);
        bool operator()(ast::struct_instantiation const& ast);
        bool operator()(ast::main_function const& ast);

        std::vector<int>& get_code() { return code; }
        std::map<std::string, int> const& get_vars() { return variables; }

    private:
        std::vector<cstruct> structs;
        typedef std::map<std::string, boost::shared_ptr<function> > function_table;
        function_table functions;
        function* current_function;
        function* global_function;
        error_handler& error_;
        parser::struct_types& structs_table;

        bool eval_expression(
            int min_precedence,
            std::list<ast::operation>::const_iterator& rbegin,
            std::list<ast::operation>::const_iterator rend);

        bool void_return;
        std::string current_function_name;
        std::vector<int>& stack;
        type_handler th;
    };

    class Interpreter
    {
    public:
        Interpreter(size_t size = 50) :
            stack(size),
            error(start, end, &error_buf),
            compiler(stack, code, error, structs)
        {}
        bool parse(std::string input);
        bool parse(QString input);
        int execute() {
            return execute(code, code.begin(), stack.begin());
        }
        std::vector<int> const& getStack()
        {
            return stack;
        }
        std::vector<int> const& getCode()
        {
            return compiler.get_code();
        }
        std::map<std::string,int> const& getGlobals()
        {
            return compiler.get_vars();
        }

    private:
        iterator_type start;
        iterator_type end;
        std::string src;
        std::vector<int> stack;
        std::vector<int> code;
        error_handler error;
        global compiler;
        ast::main_function ast;
        parser::struct_types structs;
        QString* error_buf;

        parser::skipper skip;

//        QList<ast::struct_declaration> global_structs;
        int execute(
            std::vector<int> const& code            // the program code
          , std::vector<int>::const_iterator pc     // program counter
          , std::vector<int>::iterator frame_ptr    // start of arguments and locals
        );
    };
}

#endif // INTERPRETER_H

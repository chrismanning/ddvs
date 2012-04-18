/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <boost/shared_ptr.hpp>
#include <memory>
#include <function.h>
#include <types.h>
#include <QStringList>
#include <QMessageBox>
#include <QDebug>

typedef std::string::const_iterator iterator_type;

namespace interpreter {
    enum byte_code
    {
        op_neg,         //0  negate the top stack entry
        op_pos,         //1  make the top stack entry positive
        op_add,         //2  add top two stack entries
        op_sub,         //3  subtract top two stack entries
        op_mul,         //4  multiply top two stack entries
        op_div,         //5  divide top two stack entries

        op_select_point,//6  select struct member after dereferencing with ->
        op_select_ref,  //7  select struct member with .
        op_increment,   //8  postfix increment ++
        op_decrement,   //9  postfix decrement --
        op_dereference, //10 dereference address with *
        op_address,     //11 get stack index with &

        op_not,         //12 boolean negate the top stack entry
        op_eq,          //13 compare the top two stack entries for ==
        op_neq,         //14 compare the top two stack entries for !=
        op_lt,          //15 compare the top two stack entries for <
        op_lte,         //16 compare the top two stack entries for <=
        op_gt,          //17 compare the top two stack entries for >
        op_gte,         //18 compare the top two stack entries for >=

        op_and,         //19 logical and top two stack entries
        op_or,          //20 logical or top two stack entries

        op_load,        //21 load a variable
        op_store,       //22 store a variable

        op_int,         //23 push constant integer into the stack
        op_true,        //24 push constant 0 into the stack
        op_false,       //25 push constant 1 into the stack

        op_jump_if,     //26 jump to a relative position in the code if top stack
                        //27 evaluates to false
        op_jump,        //28 jump to a relative position in the code

        op_stk_adj,     //29 adjust the stack (for args and locals)
        op_call,        //30 function call
        op_return,      //31 return from function
        op_void         //32 void (no return)
    };

    class null_ptr : public std::exception
    {
    public:
        virtual const char* what() const throw()
        {
            return "null pointer";
        }
    };

    enum type_code
    {
        type_void,
        type_int,
        type_pointer,
        type_struct
    };

    struct variable;

    //variable operators for VM
    //arithmetic
    variable operator+(variable const& a, variable const& b);
    variable operator-(variable const& a, variable const& b);
    variable operator*(variable const& a, variable const& b);
    variable operator/(variable const& a, variable const& b);
    //relational
    bool operator||(variable const& a, variable const& b);
    bool operator&&(variable const& a, variable const& b);
    bool operator==(variable const& a, variable const& b);
    bool operator!=(variable const& a, variable const& b);
    bool operator<(variable const& a, variable const& b);
    bool operator<=(variable const& a, variable const& b);
    bool operator>(variable const& a, variable const& b);
    bool operator>=(variable const& a, variable const& b);
    //unary
    variable operator+(variable const& a);
    variable operator-(variable const& a);
    variable operator!(variable const& a);
    variable& operator*(variable const& a);
    variable& operator&(variable const& a);
    //new & delete shall come later

    struct variable : ast::Typed
    {
        variable() : var(0) {}
        variable(int var, ast::Type type) : var(var)
        {
            this->type = type;
            type.lvalue = true;
        }
        int var;

        variable(variable const& b)
        {
            if(!type.is_set && b.type.is_set) {
                type = b.type;
            }
            var = b.var;
        }
        void operator=(int b)
        {
            var = b;
            if(b != 0) {
                type = ast::Int;
            }
        }
        void operator=(bool b)
        {
            var = b;
            //type = ast::Bool;
        }
        void operator+=(variable const& b)
        {
            operator=((*this) + b);
        }
        void operator-=(variable const& b)
        {
            operator=((*this) - b);
        }
        void operator*=(variable const& b)
        {
            operator=((*this) * b);
        }
        void operator/=(variable const& b)
        {
            operator=((*this) / b);
        }
        explicit operator bool()
        {
            return bool(var);
        }
        const int* dereference(std::vector<int> const& stack)
        {
            return &stack[var];
        }

        int size()
        {
            return type.width;
        }

        friend std::ostream& operator<<(std::ostream& out, variable const& var);
        friend QDebug& operator<<(QDebug& out, variable const& var);
    };

    struct cstruct : ast::Typed
    {
        typedef std::map<std::string, ast::Type> members_type;
        cstruct(ast::struct_specifier& ss, std::vector<variable>& stack)
            : name(ss.type_name.name), stack(stack)
        {
            qDebug() << "Creating new struct";
            int offset = 1;
            for(auto& member_ : ss.members) {
                auto& member = member_.get();
                member_specs[member.dec.name.name] = member.type;
                members[member.dec.name.name] = offset;
                //stack[offset].type = member.type;
                qDebug() << member.type.type_str.c_str();
                if(member.type.pointer) {
                    member.type.width = 1;
                }
                offset += member.type.width;
            }
            type = ast::Type("struct " + name, size(), false, true);
        }

        std::string name;
        members_type member_specs;
        std::map<std::string, int> members;
        std::vector<variable>& stack;

        int member_offset(std::string const& member, int offset)
        {
            auto i = members.find(member);
            if(i != members.end()) {
                int rv = i->second + offset;
                if(!stack[rv].type.is_set) {
                    stack[rv].type = member_specs[i->first];
                }
                return i->second + offset;
            }
            return -1;
        }

        int size()
        {
            int sum = 1;

            for(members_type::value_type a : member_specs) {
                sum += a.second.width;
            }

            return sum;
        }
    };

    struct Address
    {
        Address(size_t addr) : location(addr)
        {}
        Address operator&()
        {
            return Address(location);
        }
        size_t location;
    };

    typedef boost::variant<
          ast::Int_Value
        , ast::Bool_Value
        , cstruct
        , Address
      > variable_type;

    struct scope : QObject
    {
        scope(std::vector<int>& code, std::vector<variable>& stack, int& offset)
            : code(code), stack(stack), offset(offset), held_value(-1)
        {}
        scope(scope* parent)
            : parent(parent)
            , code(parent->code)
            , stack(parent->stack)
            , offset(parent->offset)
            , held_value(-1)
        {}

        void op(int a);
        void op(int a, int b);
        void op(int a, int b, int c);

        const int* lookup_var(std::string const& name);

        bool exists(std::string const& name)
        {
            return bool(lookup_var(name));
        }

        ast::Type lookup_struct_type(std::string const& name);
        void add_var(std::string const& name, ast::Type& type);

        std::map<std::string, int> table;
        boost::shared_ptr<scope> parent;
        std::map<std::string, cstruct> structs;
        std::vector<int>& code;
        std::vector<variable>& stack;
        int& offset;
        int held_value;
    public slots:
        void addStruct(std::string const& name, cstruct const& s)
        {
            qDebug() << "scope: addStruct";
            structs.insert(std::pair<std::string,cstruct>(name, s));
            emit newStructDefinition(s);
        }

    signals:
        void newStructDefinition(cstruct const& s);
    private:
        Q_OBJECT
    };

    struct type_resolver : boost::static_visitor<ast::Type>
    {
        type_resolver(error_handler& error_, scope* env)
            : env(env), error_(error_)
        {}

        ast::Type operator()(ast::Type& t)
        {
            return t;
        }

        ast::Type operator()(ast::struct_specifier& ss)
        {
            qDebug() << "Processing: ast::struct_specifier";
            auto t = env->lookup_struct_type(ss.type_name.name);
            if(ss.members.size() > 0) {
                if(t != ast::Error) {
                    error(ss.type_name.id, "Duplicate struct type");
                    return ast::Error;
                }
                for(auto& member_ : ss.members) {
                    auto& member = member_.get();
                    member.type = member.type_spec.apply_visitor(*this);
                    if(member.type == ast::Error) {
                        return ast::Error;
                    }
                    member.type.pointer = member.dec.pointer;
                }
                env->addStruct(ss.type_name.name, cstruct(ss, env->stack));
                return env->lookup_struct_type(ss.type_name.name);
            }
            else if(t == ast::Error) {
                error(ss.type_name.id, "No such struct");
                return t;
            }
            return t;
        }

        void error(int id, std::string const& what)
        {
            error_("Error! ", what, error_.iters[id]);
        }

        scope* env;
        error_handler& error_;
    };

    struct PrimaryExpressionTypeResolver : boost::static_visitor<ast::Type>
    {
        template <typename T>
        ast::Type& operator()(T& a, typename boost::enable_if<boost::is_base_of<ast::Typed, T> >::type* dummy = 0)
        {
            return a.type;
        }
        template <typename T>
        ast::Type& operator()(T& /*a*/, typename boost::disable_if<boost::is_base_of<ast::Typed, T> >::type* dummy = 0)
        {
            return ast::Error;
        }
    };

    struct Expression : boost::static_visitor<bool>
    {
        Expression(error_handler& error_, scope* env) : env(env), error_(error_)
        {
            BOOST_ASSERT(env);
        }

        Expression(error_handler& error_, scope* env, ast::Type type, bool lvalue = false)
            : type(type), lvalue(lvalue), env(env), error_(error_)
        {
            BOOST_ASSERT(env);
        }

        void error(int id, std::string const& what)
        {
            error_("Error! ", what, error_.iters[id]);
        }

        bool operator()(ast::assignment_expression& ast);
        bool operator()(ast::logical_OR_expression& ast);
        bool operator()(ast::logical_AND_expression& ast);
        bool operator()(ast::equality_expression& ast);
        bool operator()(ast::relational_expression& ast);
        bool operator()(ast::additive_expression& ast);
        bool operator()(ast::multiplicative_expression& ast);
        bool operator()(ast::unary_expression& ast);
        bool operator()(ast::struct_expr& ast);
        bool operator()(ast::postfix_expression& ast);
        bool operator()(ast::optoken& ast);
        bool operator()(ast::identifier& ast);
        bool operator()(bool ast);
        bool operator()(ast::Bool_Value ast);
        bool operator()(unsigned int ast);
        bool operator()(ast::Int_Value ast);
        bool operator()(ast::nil) { assert(0); return false; }

        ast::Type operator()(ast::primary_expression& ast);

        bool lvalue;
        ast::Type type;
        scope* env;
        PrimaryExpressionTypeResolver petr;
        error_handler& error_;
    };

    struct function
    {
        function(std::size_t nargs, std::size_t offset_, scope* current_scope) :
            offset(offset_),
            size_(0),
            nargs_(nargs),
            void_return(false)/*,
            env(new scope(current_scope,))*/
        {}

        int& operator[](std::size_t i) { return code[address+i]; }
        int const& operator[](std::size_t i) const { return code[address+i]; }
        std::size_t size() const { return size_; }
        std::size_t get_address() const { return address; }

        std::size_t nargs() const { return nargs_; }
//        std::size_t nvars() const { return variables.size(); }
        void link_to(std::string const& name, std::size_t address);
        std::size_t offset;
        bool void_return;
        ast::Type return_type;

    protected:
        std::map<std::size_t, std::string> function_calls;
        scope* env;
        std::vector<int> code;
        std::size_t address;
        std::size_t size_;
        std::size_t nargs_;
    };

#define DDVS_STACK_SIZE 500

    struct global : public QObject, public boost::static_visitor<bool> {
        global(std::vector<int>& code, error_handler& error__)
            :   stack(DDVS_STACK_SIZE),
                error_(error__),
                stack_offset(0),
                global_scope(code, stack, stack_offset),
                current_scope(&global_scope)
        {
            connect(&global_scope, SIGNAL(newStructDefinition(cstruct)), this, SLOT(structDefined(cstruct)));
        }

        void error(int id, std::string const& what)
        {
            error_("Error! ", what, error_.iters[id]);
        }

        bool operator()(ast::nil) { assert(0); return false; }
        bool operator()(ast::identifier& ast);
        bool operator()(ast::function_call& ast);
        bool operator()(ast::assignment_expression& ast);
        bool operator()(ast::logical_OR_expression& ast);
        bool operator()(ast::declaration& ast);
        bool operator()(ast::declarator& ast);
        bool operator()(ast::init_declarator& ast);
        bool operator()(ast::compound_statement& ast);
        bool operator()(ast::statement& ast);
        bool operator()(ast::if_statement& ast);
        bool operator()(ast::while_statement& ast);
        bool operator()(ast::return_statement& ast);
        bool operator()(ast::struct_member_declaration& ast);
        bool operator()(ast::struct_specifier& ast);
        bool operator()(ast::function_definition& ast);
        bool operator()(ast::translation_unit& ast);

        ast::Type operator()(ast::type_specifier& t);

        std::vector<int> const& get_code()
        {
            return global_scope.code;
        }

        std::vector<variable>& get_stack()
        {
            return stack;
        }

        std::vector<variable>::iterator get_stack_pos()
        {
            return stack.begin() + stack_offset;
        }

        std::map<std::string, int> const& get_vars()
        {
            return global_scope.table;
        }

        std::map<std::string, cstruct> const& get_structs() {
            return global_scope.structs;
        }

        int get_offset()
        {
            return stack_offset;
        }

    public slots:
        void structDefined(cstruct const& s)
        {
            qDebug() << "global: structDefined";
            emit newStructDefinition(s);
        }

    signals:
        void newStructDefinition(cstruct const& s);
    private:
        Q_OBJECT
        typedef std::map<std::string, boost::shared_ptr<function> > function_table;
        function_table functions;
        scope global_scope;
        scope* current_scope;
        error_handler& error_;
        std::vector<variable> stack;
        int stack_offset;
    };

    class Interpreter : public QObject
    {
        Q_OBJECT
    public:
        Interpreter() :
            error(start, end, error_buf),
            compiler(code, error)
        {
            connect(&compiler, SIGNAL(newStructDefinition(cstruct)), this, SLOT(structDefined(cstruct)));
        }
        bool parse(std::string input);
        bool parse(QString input);
        std::vector<variable>& getStack()
        {
            return compiler.get_stack();
        }
        std::vector<variable>::iterator getStackPos()
        {
            return compiler.get_stack_pos();
        }
        int getOffset()
        {
            return compiler.get_offset();
        }
        std::map<std::string, int> const& getGlobals()
        {
            return compiler.get_vars();
        }
        int execute() {
            int r = execute(code, code.begin(), getStack().begin(), getStackPos());
            code.clear();
            return r;
        }

    public slots:
        void structDefined(cstruct const& s)
        {
            qDebug() << "Interpreter: structDefined";
            emit newStructDefinition(s);
        }

    signals:
        void newStructDefinition(cstruct const& s);

    private:
        QMessageBox msg_box;
        iterator_type start;
        iterator_type end;
        std::string src;
        error_handler error;
        global compiler;
        ast::translation_unit ast;
        std::vector<int> code;
        boost::shared_ptr<QString> error_buf;

        parser::skipper skip;

        int execute(
            std::vector<int> const& code            // the program code
          , std::vector<int>::const_iterator pc     // program counter
          , std::vector<variable>::iterator frame_ptr    // start of whole stack
          , std::vector<variable>::iterator stack_ptr    // start of emptiness
        );
    };
}

#endif // INTERPRETER_H

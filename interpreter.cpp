/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "interpreter.h"
namespace interpreter {
    void function::op(int a)
    {
        code.push_back(a);
        size_ += 1;
    }

    void function::op(int a, int b)
    {
        code.push_back(a);
        code.push_back(b);
        size_ += 2;
    }

    void function::op(int a, int b, int c)
    {
        code.push_back(a);
        code.push_back(b);
        code.push_back(c);
        size_ += 3;
    }

    int const* function::find_var(std::string const& name) const
    {
        std::map<std::string, int>::const_iterator i = variables.find(name);
        if(i == variables.end())
            return 0;
        return &i->second;
    }

    void function::add_var(std::string const& name)
    {
        std::size_t n = offset;
        variables[name] = n;
        ++offset;
    }

    void function::link_to(std::string const& name, std::size_t address)
    {
        function_calls[address] = name;
    }

    bool global::operator()(ast::main_function const& ast)
    {
        foreach(ast::func_state const& fos, ast) {
            if(!boost::apply_visitor(*this, fos))
                return false;
            if(fos.type() == typeid(ast::function)) {
                current_function = global_function;
            }
        }
        return true;
    }
    bool global::operator()(unsigned int ast)
    {
        BOOST_ASSERT(current_function != 0);
        current_function->op(op_int, ast);
        return true;
    }

    bool global::operator()(bool ast)
    {
        BOOST_ASSERT(current_function != 0);
        current_function->op(ast ? op_true : op_false);
        return true;
    }

    bool global::operator()(ast::identifier const& ast)
    {
        BOOST_ASSERT(current_function != 0);
        int const* p = current_function->find_var(ast.name);
        if(p == 0)
        {
            error(ast.id, "Undeclared variable: " + ast.name);
            return false;
        }
        current_function->op(op_load, *p);
        return true;
    }

    bool global::operator()(ast::optoken const& ast)
    {
        BOOST_ASSERT(current_function != 0);
        switch(ast)
        {
            case ast::op_assign: current_function->op(op_store); break;
            case ast::op_plus: current_function->op(op_add); break;
            case ast::op_minus: current_function->op(op_sub); break;
            case ast::op_times: current_function->op(op_mul); break;
            case ast::op_divide: current_function->op(op_div); break;

            case ast::op_equal: current_function->op(op_eq); break;
            case ast::op_not_equal: current_function->op(op_neq); break;
            case ast::op_less: current_function->op(op_lt); break;
            case ast::op_less_equal: current_function->op(op_lte); break;
            case ast::op_greater: current_function->op(op_gt); break;
            case ast::op_greater_equal: current_function->op(op_gte); break;

            case ast::op_logical_or: current_function->op(op_or); break;
            case ast::op_logical_and: current_function->op(op_and); break;
            default: BOOST_ASSERT(0); return false;
        }
        return true;
    }

    bool global::operator()(ast::unary_expression const& ast)
    {
//        BOOST_ASSERT(current_function != 0);
//        if(!boost::apply_visitor(*this, ast.operand_))
//            return false;
//        switch(ast.operator_)
//        {
//            case ast::op_negative: current_function->op(op_neg); break;
//            case ast::op_not: current_function->op(op_not); break;
//            case ast::op_select_point:
//                current_function->op(op_select_point);
//                if(ast.operand_.type() == typeid(ast::identifier)) {

//                }
//                break;
//            case ast::op_select_ref:
//                current_function->op(op_select_ref);
//                break;
//            case ast::op_positive: break;
//            case ast::op_indirection: break;
//            case ast::op_address: break;
//            default: BOOST_ASSERT(0); return false;
//        }
        return true;
    }

    bool global::operator()(ast::postfix_expression const& ast)
    {
        return false;
    }

    bool global::operator()(ast::function_call const& ast)
    {
        BOOST_ASSERT(current_function != 0);
// FIXME
//        if(ast.function_name.name == std::string("writelns")) {
//            std::string arg = boost::get<ast::identifier>(ast.args.front().lhs).name;
//            foreach(ast::assignment_expression arg, ast.args) {
//                //qDebug() << stack[variables[arg.expression]];
//            }
//            return true;
//        }

        if(functions.find(ast.function_name.name) == functions.end()) {
            //qDebug() << ast.function_name.id;
            //qDebug() << "Function not found: " << ast.function_name.name.c_str();
            error(ast.function_name.id, "Function not found: " + ast.function_name.name);
            return false;
        }

        boost::shared_ptr<function> p = functions[ast.function_name.name];

        if(p->nargs() != ast.args.size()) {
            //qDebug() << ast.function_name.id;
            error(ast.function_name.id, "Wrong number of arguments: " + ast.function_name.name);
            return false;
        }

        foreach(ast::assignment_expression const& expr, ast.args)
        {
            if(!(*this)(expr))
                return false;
        }

        current_function->op(
            op_call,
            p->nargs(),
            p->get_address());
        current_function->link_to(ast.function_name.name, p->get_address());

        return true;
    }

    namespace
    {
        int precedence[] = {
            // precedence 1
            1, // op_comma

            // precedence 2
            2, // op_assign

            // precedence 3
            3, // op_logical_or

            // precedence 4
            4, // op_logical_and

            // precedence 8
            5, // op_equal
            5, // op_not_equal

            // precedence 9
            6, // op_less
            6, // op_less_equal
            6, // op_greater
            6, // op_greater_equal

            // precedence 11
            7, // op_plus
            7, // op_minus

            // precedence 12
            8, // op_times
            8, // op_divide

            // precedence 13
            9, // op_positive
            9, // op_negative
            9, // op_not
            9, // op_address
            9, // op_indirection

            // precedence 14
            10, // op_select_point
            10  // op_select_ref
        };
    }

    // The Shunting-yard algorithm
    bool global::eval_expression(
        int min_precedence,
        std::list<ast::operation>::const_iterator& rbegin,
        std::list<ast::operation>::const_iterator rend)
    {
        while((rbegin != rend) && (precedence[rbegin->operator_] >= min_precedence))
        {
            ast::optoken op = rbegin->operator_;
            if(!boost::apply_visitor(*this, rbegin->operand_))
                return false;
            ++rbegin;

            while((rbegin != rend) && (precedence[rbegin->operator_] > precedence[op]))
            {
                ast::optoken next_op = rbegin->operator_;
                eval_expression(precedence[next_op], rbegin, rend);
            }
            (*this)(op);
        }
        return true;
    }

    bool global::operator()(ast::expression const& ast)
    {
        BOOST_ASSERT(current_function != 0);
        if(!boost::apply_visitor(*this, ast.first))
            return false;
        std::list<ast::operation>::const_iterator rbegin = ast.rest.begin();
        if(!eval_expression(0, rbegin, ast.rest.end()))
            return false;
        return true;
    }

    bool global::operator()(ast::assignment_expression const& ast)
    {
        BOOST_ASSERT(current_function != 0);
        if(!(*this)(ast.rhs))
            return false;
        int const* p = 0;//current_function->find_var(ast.lhs.name);
        if(p == 0)
        {
            //error(ast.lhs.id, "Undeclared variable: " + ast.lhs.name);
            return false;
        }
        current_function->op(op_store, *p);
        return true;
    }

    bool global::operator()(ast::declaration const& ast)
    {
        BOOST_ASSERT(current_function != 0);
        //qDebug() << /*"type_code:" << ast.typ <<*/ "pointer:" << ast.dec.pointer;
//        if(boost::apply_visitor(th, ast.type) == "basic")
//        {
//            if(!ast.declarator) {
//                error(ast.id, "Expected <identifier> with basic type");
//                return false;
//            }
//        }
        ast::init_declarator const& dec = *ast.declarator;
        int const* p = current_function->find_var(dec.dec.name.name);
        if(p != 0)
        {
            error(ast.id, "Duplicate variable: " + dec.dec.name.name);
            return false;
        }

        if(dec.assign)
        {
            bool r = (*this)(*dec.assign);
            if(r) // don't add the variable if the RHS fails
            {
                current_function->add_var(dec.dec.name.name);
                current_function->op(op_store, *current_function->find_var(dec.dec.name.name));
            }
            return r;
        }
        else
        {
            current_function->add_var(dec.dec.name.name);
        }

        return true;
    }

    bool global::operator()(ast::statement const& ast)
    {
        BOOST_ASSERT(current_function != 0);
        return boost::apply_visitor(*this, ast);
    }

    bool global::operator()(ast::statement_list const& ast)
    {
        BOOST_ASSERT(current_function != 0);
        foreach(ast::statement const& s, ast)
        {
            if(!(*this)(s))
                return false;
        }
        return true;
    }

    bool global::operator()(ast::if_statement const& ast)
    {
        BOOST_ASSERT(current_function != 0);
        if(!(*this)(ast.condition))
            return false;
        current_function->op(op_jump_if, 0);                 // we shall fill this (0) in later
        std::size_t skip = current_function->size()-1;       // mark its position
        if(!(*this)(ast.then))
            return false;
        (*current_function)[skip] = current_function->size()-skip;    // now we know where to jump to (after the if branch)

        if(ast.else_)                                // We got an else
        {
            (*current_function)[skip] += 2;                  // adjust for the "else" jump
            current_function->op(op_jump, 0);                // we shall fill this (0) in later
            std::size_t exit = current_function->size()-1;   // mark its position
            if(!(*this)(*ast.else_))
                return false;
            (*current_function)[exit] = current_function->size()-exit;// now we know where to jump to (after the else branch)
        }

        return true;
    }

    bool global::operator()(ast::while_statement const& ast)
    {
        BOOST_ASSERT(current_function != 0);
        std::size_t loop = current_function->size();         // mark our position
        if(!(*this)(ast.condition))
            return false;
        current_function->op(op_jump_if, 0);                 // we shall fill this (0) in later
        std::size_t exit = current_function->size()-1;       // mark its position
        if(!(*this)(ast.body))
            return false;
        current_function->op(op_jump,
            int(loop-1) - int(current_function->size()));    // loop back
        (*current_function)[exit] = current_function->size()-exit;    // now we know where to jump to (to exit the loop)
        return true;
    }

    bool global::operator()(ast::return_statement const& ast)
    {
        if(current_function->void_return)
        {
            if(ast.expr)
            {
                //qDebug() << ast.id;
                error(ast.id, "'void' function returning a value: ");
                return false;
            }
        }
        else
        {
            if(!ast.expr)
            {
                //qDebug() << ast.id;
                error(ast.id, current_function_name + " function must return a value: ");
                return false;
            }
        }

        if(ast.expr)
        {
            if(!(*this)(*ast.expr))
                return false;
        }
        current_function->op(op_return);
        return true;
    }

    bool global::operator()(ast::function const& ast)
    {
        //void_return = ast.return_type == "void";
//        if(ast.return_type_code) {
//            return_type = 1;
//        }
        if(functions.find(ast.function_name.name) != functions.end())
        {
            //qDebug() << ast.function_name.id;
            error(ast.function_name.id, "Duplicate function: " + ast.function_name.name);
            return false;
        }
        //offset = this->variables.size();
//        foreach(function_table::value_type const& fun, functions) {
//            offset += fun.second.get()->nvars();
//        }

        boost::shared_ptr<function>& p = functions[ast.function_name.name];
        p.reset(new function(variables, pointers, code, ast.args.size(), offset));
        current_function = p.get();
        current_function_name = ast.function_name.name;

        // op_stk_adj 0 for now. we'll know how many variables
        // we'll have later and add them
        current_function->op(op_stk_adj, 0);
        foreach(ast::arg const& arg, ast.args)
        {
            current_function->add_var(arg.dec.name.name);
        }

        foreach(ast::statement const& state, ast.body) {
            if(!boost::apply_visitor(*this,state))
                return false;
        }
        (*current_function)[1] = current_function->nvars();   // now store the actual number of variables
                                            // this includes the arguments
        offset += current_function->nvars();
        return true;
    }

//    bool global::operator()(ast::struct_member_declaration const& /*ast*/)
//    {
//        return true;
//    }

    int sym_size;
    void incr(std::string s, int d) {
        sym_size++;
    }
    int count_symbols(parser::struct_types sym) {
        sym_size = 0;
        sym.for_each(&incr);
        return sym_size;
    }
    void print(std::string s, int d) {
        qDebug() << QString::fromStdString(s) << d;
    }

    bool global::operator()(ast::struct_specifier const& ast)
    {
        std::map<std::string, int> members;
        foreach(boost::recursive_wrapper<ast::struct_member_declaration> const& mem, ast.members) {
            //members[mem.get().dec.name.name] = members.size();
        }
        structs.push_back(cstruct(ast.type_name.name,members));
//        int num = count_symbols(structs_table);
//        structs_table.add(ast.type_name.name,num);
//        structs_table.for_each(&print);

        return true;
    }

    bool global::operator()(ast::function_list const& ast)
    {
        // Jump to the main function
        code.push_back(op_jump);
        code.push_back(0); // we will fill this in later when we finish compiling
                           // and we know where the main function is

        foreach(ast::function const& f, ast)
        {
            if(!(*this)(f))
            {
                code.clear();
                return false;
            }
        }

        return true;
    }

    bool Interpreter::parse(std::string input) {
        src = input;
        start = src.begin();
        end = src.end();
        iterator_type iter = start;
        parser::main_function parser(error);
        structs.for_each(&print);

        bool success = phrase_parse(iter, end, +parser, skip, ast);

        if(success) {
            success = compiler(ast);
            if(success) {
                qDebug() << "SUCCESS";
                ast.clear();
            }
            else {
                qDebug() << "FAIL";
                if(error_buf) {
                    qDebug() << *error_buf;
                    error_buf->clear();
                }
            }
        }
        else {
            qDebug() << "PARSE FAIL";
            if(error_buf) {
                qDebug() << *error_buf;
                error_buf->clear();
            }
        }

        return success;
    }

    bool Interpreter::parse(QString input) {
        return parse(input.toStdString());
    }

    int Interpreter::execute(std::vector<int> const& code,
                             std::vector<int>::const_iterator pc,
                             std::vector<int>::iterator frame_ptr)
    {
        std::vector<int>::iterator stack_ptr = frame_ptr;
        std::vector<int>::const_iterator start = pc;
        while(pc<code.end())
        {
            switch(*pc++)
            {
                case op_neg:
                    stack_ptr[-1] = -stack_ptr[-1];
                    break;

                case op_not:
                    stack_ptr[-1] = !bool(stack_ptr[-1]);
                    break;

                case op_add:
                    --stack_ptr;
                    stack_ptr[-1] += stack_ptr[0];
                    break;

                case op_sub:
                    --stack_ptr;
                    stack_ptr[-1] -= stack_ptr[0];
                    break;

                case op_mul:
                    --stack_ptr;
                    stack_ptr[-1] *= stack_ptr[0];
                    break;

                case op_div:
                    --stack_ptr;
                    stack_ptr[-1] /= stack_ptr[0];
                    break;

                case op_eq:
                    --stack_ptr;
                    stack_ptr[-1] = bool(stack_ptr[-1] == stack_ptr[0]);
                    break;

                case op_neq:
                    --stack_ptr;
                    stack_ptr[-1] = bool(stack_ptr[-1] != stack_ptr[0]);
                    break;

                case op_lt:
                    --stack_ptr;
                    stack_ptr[-1] = bool(stack_ptr[-1] < stack_ptr[0]);
                    break;

                case op_lte:
                    --stack_ptr;
                    stack_ptr[-1] = bool(stack_ptr[-1] <= stack_ptr[0]);
                    break;

                case op_gt:
                    --stack_ptr;
                    stack_ptr[-1] = bool(stack_ptr[-1] > stack_ptr[0]);
                    break;

                case op_gte:
                    --stack_ptr;
                    stack_ptr[-1] = bool(stack_ptr[-1] >= stack_ptr[0]);
                    break;

                case op_and:
                    --stack_ptr;
                    stack_ptr[-1] = bool(stack_ptr[-1]) && bool(stack_ptr[0]);
                    break;

                case op_or:
                    --stack_ptr;
                    stack_ptr[-1] = bool(stack_ptr[-1]) || bool(stack_ptr[0]);
                    break;

                case op_load:
                    *stack_ptr++ = frame_ptr[*pc++];
                    break;

                case op_store:
                    --stack_ptr;
                    frame_ptr[*pc++] = *stack_ptr++;
                    break;

                case op_int:
                    *stack_ptr++ = *pc++;
                    break;

                case op_true:
                    *stack_ptr++ = true;
                    break;

                case op_false:
                    *stack_ptr++ = false;
                    break;

                case op_jump:
                    pc += *pc;
                    break;

                case op_jump_if:
                    if(!bool(stack_ptr[-1]))
                        pc += *pc;
                    else
                        ++pc;
                    --stack_ptr;
                    break;

                case op_stk_adj:
                    stack_ptr += *pc++;
                    break;

                case op_call:
                    {
                        int nargs = *pc++;
                        int jump = *pc++;
                        std::vector<int>::const_iterator pos = start + jump;

                        // a function call is a recursive call to execute
                        int r = execute(code, pos, stack_ptr - nargs);

                        if(pos == code.begin()) {
                            qDebug() << r;
                        }

                        // cleanup after return from function
                        stack_ptr[-nargs] = r;      //  get return value
                        stack_ptr -= (nargs - 1);   //  the stack will now contain
                                                    //  the return value
                    }
                    break;

                case op_return:
                    return stack_ptr[-1];
                case op_void:
                    return 0;
            }
        }
    }
}

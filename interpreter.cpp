/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "interpreter.h"

#define EXPRESSION_TYPE_ERROR(T) \
    if(ast.lhs.type != T) {\
        error(ast.lhs.id, "type mismatch: should be " + T.type_str );\
        return false;\
    }\
    else if(expr.rhs.type != T) {\
        error(expr.rhs.id, "type mismatch: should be " + T.type_str );\
        return false;\
    }

namespace interpreter {
    void function::link_to(std::string const& name, std::size_t address)
    {
        function_calls[address] = name;
    }

    template<typename T>
    variable ignore(T a)
    {
        variable x;
        x.var = a;
        return x;
    }

    //arithmetic
    variable operator+(variable const& a, variable const& b)
    {
        return variable(a.var + b.var, a.type);
    }

    variable operator-(variable const& a, variable const& b)
    {
        return variable(a.var - b.var, a.type);
    }

    variable operator*(variable const& a, variable const& b)
    {
        return variable(a.var * b.var, a.type);
    }

    variable operator/(variable const& a, variable const& b)
    {
        return variable(a.var / b.var, a.type);
    }

    //relational
    bool operator||(variable const& a, variable const& b)
    {
        return a.var || b.var;
    }

    bool operator&&(variable const& a, variable const& b)
    {
        return a.var && b.var;
    }

    bool operator==(variable const& a, variable const& b)
    {
        return a.var == b.var;
    }

    bool operator!=(variable const& a, variable const& b)
    {
        return a.var != b.var;
    }

    bool operator<(variable const& a, variable const& b)
    {
        return a.var < b.var;
    }

    bool operator<=(variable const& a, variable const& b)
    {
        return a.var < b.var;
    }

    bool operator>(variable const& a, variable const& b)
    {
        return a.var > b.var;
    }

    bool operator>=(variable const& a, variable const& b)
    {
        return a.var >= b.var;
    }

    //unary
    variable operator+(variable const& a)
    {
        return variable(+a.var, a.type);
    }

    variable operator-(variable const& a)
    {
        return variable(-a.var, a.type);
    }

    variable operator!(variable const& a)
    {
        return variable(!a.var, a.type);
    }

    variable& operator*(variable const& a);
    variable& operator&(variable const& a);

    std::ostream& operator<< (std::ostream& out, variable const& var)
    {
        out << var.var;
        return out;
    }

    QDebug& operator<<(QDebug& out, variable const& var)
    {
        out << var.var;
        return out;
    }

    bool Expression::operator()(ast::assignment_expression& ast)
    {
        qDebug() << "Processing: ast::assignment_expression";
        if(!(*this)(ast.lhs)) {
            return false;
        }//FIXME
        if(ast.rhs) {
            if(ast.lhs.type.lvalue) {
                int tmp = env->held_value;
                if(tmp == -1 && ast.lhs.type != "struct") {
                    return false;
                }
                if(ast.rhs->operand_.which() == 0) {
                    ast::logical_OR_expression& operand = boost::get<ast::logical_OR_expression&>(ast.rhs->operand_);
                    if(!(*this)(operand)) {
                        return false;
                    }
                    if(!(*this)(ast.rhs->operator_)) {
                        return false;
                    }
                    if(tmp != -1) {
                        env->op(tmp);
                        env->held_value = -1;
                    }
                    return true;
                }
                else {
                    ast::allocation_expression& alloc_ = boost::get<ast::allocation_expression&>(ast.rhs->operand_);
                    type_resolver tr(error_, env);
                    ast::Type t = boost::apply_visitor(tr, alloc_.type_spec);
                    if(!ast.lhs.type.pointer) {
                        error(ast.lhs.id, "not a pointer");
                        return false;
                    }
                    ast.type = ast.lhs.type;
                    env->stack[env->offset].type = t;
                    for(unsigned int i=env->offset; i<env->offset+t.width; i++) {
                        env->stack[i] = 0;
                    }
                    env->op(op_int, env->offset);
                    if(!(*this)(ast.rhs->operator_)) {
                        return false;
                    }
                    if(env->held_value != -1) {
                        env->op(env->held_value);
                    }
                    else {
                        env->op(env->offset);
                    }
                    env->offset += t.width;
                    env->held_value = -1;
                    return true;
                }
            }
            else {
                error(ast.lhs.id, "not an lvalue");
                return false;
            }
        }
        return true;
    }

    bool Expression::operator()(ast::logical_OR_expression& ast)
    {
        qDebug() << "Processing: ast::logical_OR_expression";
        if(!(*this)(ast.lhs)) {
            return false;
        }
        else if(ast.rest.size() > 0) {
            for(ast::logical_OR_op& expr : ast.rest) {
                if(!(*this)(expr.rhs)) {
                    return false;
                }
                //if(expr.rhs.type != ast.lhs.type || expr.rhs.type != ast::Bool) {
                EXPRESSION_TYPE_ERROR(ast::Bool);
                    //return false;
                //}
                /*if(env->held_value != -1) {
                    if((*this)(expr.operator_)) {
                        env->op(env->held_value);
                        env->held_value = -1;
                    }
                    return false;
                }
                else
                    return */(*this)(expr.operator_);
            }
            ast.type = ast::Bool;
        }
        else {
            ast.type = ast.lhs.type;
        }
        return true;
    }

    bool Expression::operator()(ast::logical_AND_expression& ast)
    {
        qDebug() << "Processing: ast::logical_AND_expression";
        if(!(*this)(ast.lhs)) {
            return false;
        }
        else if(ast.rest.size() > 0) {
            for(ast::logical_AND_op& expr : ast.rest) {
                if(!(*this)(expr.rhs)) {
                    return false;
                }
                EXPRESSION_TYPE_ERROR(ast::Bool);
//                if(expr.rhs.type != ast.lhs.type || expr.rhs.type != ast::Bool) {
//                    error(expr.id,"Type mismatch: should be bool");
//                    return false;
//                }
                (*this)(expr.operator_);
            }
            ast.type = ast::Bool;
        }
        else {
            ast.type = ast.lhs.type;
        }
        return true;
    }

    bool Expression::operator()(ast::equality_expression& ast)
    {
        qDebug() << "Processing: ast::equality_expression";
        if(!(*this)(ast.lhs)) {
            return false;
        }
        else if(ast.rest.size() > 0) {
            for(ast::equality_op& expr : ast.rest) {
                if(!(*this)(expr.rhs) || expr.rhs.type != ast::Int) {
                    error(expr.id,"Type mismatch: should be int");
                    return false;
                }
                if(expr.rhs.type != ast.lhs.type) {
                    return false;
                }
                (*this)(expr.operator_);
            }
            ast.type = ast::Bool;
        }
        else {
            ast.type = ast.lhs.type;
        }
        return true;
    }

    bool Expression::operator()(ast::relational_expression& ast)
    {
        qDebug() << "Processing: ast::relational_expression";
        if(!(*this)(ast.lhs)) {
            return false;
        }
        else if(ast.rest.size() > 0) {
            for(ast::relational_op& expr : ast.rest) {
                if(!(*this)(expr.rhs) || expr.rhs.type != ast::Int) {
                    error(expr.id,"Type mismatch: should be int");
                    return false;
                }
                if(expr.rhs.type != ast.lhs.type) {
                    return false;
                }
                (*this)(expr.operator_);
            }
            ast.type = ast::Bool;
        }
        else {
            ast.type = ast.lhs.type;
        }
        return true;
    }

    bool Expression::operator()(ast::additive_expression& ast)
    {
        qDebug() << "Processing: ast::additive_expression";

        if(!(*this)(ast.lhs)) {
            return false;
        }
        if(ast.rest.size() > 0) {
            for(ast::additive_op& expr : ast.rest) {
                if(!(*this)(expr.rhs) || expr.rhs.type != ast::Int) {
                    error(expr.id,"Type mismatch: should be int");
                    return false;
                }
                (*this)(expr.operator_);
            }
            ast.type = ast::Int;
        }
        else {
            ast.type = ast.lhs.type;
        }
        return true;
    }

    bool Expression::operator()(ast::multiplicative_expression& ast)
    {
        qDebug() << "Processing: ast::multiplicative_expression";

        if(!(*this)(ast.lhs)) {
            return false;
        }
        else if(ast.rest.size() > 0) {
            for(ast::multiplicative_op& expr : ast.rest) {
                if(!(*this)(expr.rhs) || expr.rhs.type != ast::Int) {
                    error(expr.id,"Type mismatch: should be int");
                    return false;
                }
                (*this)(expr.operator_);
            }
            ast.type = ast::Int;
        }
        else {
            ast.type = ast.lhs.type;
        }
        return true;
    }

    bool Expression::operator()(ast::unary_expression& ast)
    {
        qDebug() << "Processing: ast::unary_expression";
        //        BOOST_ASSERT(current_function != 0);
        if(!(*this)(ast.operand_)) {
            return false;
        }
        if(ast.operator_) {
            switch(*ast.operator_)
            {
                case ast::op_positive: env->op(op_pos); break;
                case ast::op_negative: env->op(op_neg); break;
                case ast::op_not: env->op(op_not); break;
                case ast::op_indirection:
                    env->held_value = env->stack[env->code.back()].var;
                    env->op(op_dereference);
                    ast.type = ast.operand_.type;
                    ast.type.pointer = false;
                    break;
                case ast::op_address:
                    if(ast.operand_.type.lvalue) {
                        env->held_value = env->code.back();
                        env->op(op_address, env->code.back());
                        ast.type = ast.operand_.type;
                        ast.type.pointer = true;
                    }
                    else {
                        error(ast.operand_.id, "type error");
                        return false;
                    }
                    break;
                default: BOOST_ASSERT(0); return false;
            }
        }
        if(!ast.type.is_set)
            ast.type = ast.operand_.type;
        return true;
    }

    bool Expression::operator()(ast::struct_expr& /*ast*/)
    {
        qDebug() << "Processing: ast::struct_expr";
        return false;
    }

    bool Expression::operator()(ast::postfix_expression& ast)
    {
        qDebug() << "Processing: ast::postfix_expression";

        ast.type = (*this)(ast.first);
        if(ast.type == ast::Error) {
            if(!error_.error_buf && !error_.error_buf->size() > 0)
                error(ast.id, "primary expression type error");
            return false;
        }

        //reverse this? + all the above foreach loops
        int num = 0;
        std::string previous_name, previous_member_name;
        const int* addr = 0;
        int member_offset = 0;
        std::string err;
        int id = -1;
        for(ast::postfix_op& op : ast.rest) {
            if(op.which() == 0) {
                ast::struct_expr& so = boost::get<ast::struct_expr>(op);
                if(ast.type == "struct") {
                    if(num == 0) {
                        std::string name = boost::get<ast::identifier>(ast.first).name;
                        addr = env->lookup_var(name);
                        if(env->stack[*addr].type.pointer) {
                            addr = &env->stack[*addr].var;
                        }
                        ++num;
                    }
                    else {
                        auto i = env->lookup_struct(previous_name);
                        qDebug() << "struct type:" << previous_name.c_str();
                        if(i != 0) {
                            if(addr != 0) {
                                member_offset = i->member_offset(previous_member_name, *addr);
                            }
                            else {
                                err = "bad address";
                                break;
                            }
                            qDebug() << previous_name.c_str() << ":" << previous_member_name.c_str()
                                     << ":" << so.member.name.c_str() << ":" << member_offset;
                            if(member_offset == -1) {
                                error(ast.id, "member " + previous_member_name + "not found");
                                return false;
                            }
                        }
                    }
                    if(so.operator_ == ast::op_select_point && !ast.type.pointer) {
                        err = "not a pointer";
                        break;
                    }
                    else if(so.operator_ == ast::op_select_ref && ast.type.pointer) {
                        err = "is a pointer";
                        break;
                    }
                    if(addr != 0) {
                        ast::Type struct_type = env->stack[*addr + member_offset].type;
                        if(struct_type.pointer) {
                            member_offset = env->stack[member_offset].var;
                        }
                        qDebug() << struct_type.type_str.c_str();
                        qDebug() << ast.type.type_str.c_str();
                        std::string type_name;
                        if(struct_type == "struct") {
                            type_name = struct_type.type_str.substr(7);
                        }
                        else
                            type_name = struct_type.type_str;
                        previous_name = type_name;
                        //find struct spec
                        auto i = env->lookup_struct(type_name);
                        if(i != 0) {
                            cstruct& cs = *i;
                            previous_member_name = so.member.name;
                            //find member spec
                            auto j = cs.member_specs.find(so.member.name);
                            typedef std::pair<std::string, ast::Type> member_spec__;
                            for(member_spec__ v : cs.member_specs) {
                                qDebug() << v.first.c_str() << ":" << v.second.type_str.c_str();
                            }
                            if(j != cs.member_specs.end()) {
                                auto member_type = j->second;
                                if(member_type == "struct") {
                                    ast.type = member_type;
                                    qDebug() << "member type:" << member_type.type_str.c_str();
                                    env->held_value = cs.member_offset(previous_member_name, *addr + member_offset);
                                    continue;
                                }

                                //find member offset
                                qDebug() << "Loading member"
                                         << so.member.name.c_str()
                                         << "from struct"
                                         << cs.name.c_str();
                                auto member_addr = cs.member_offset(previous_member_name, *addr + member_offset);
                                if(member_addr != -1) {
                                    ast.type = member_type;
                                    ast.type.lvalue = true;
                                    env->op(op_load, member_addr);
                                    env->held_value = member_addr;
                                    continue;
                                }
                                else {
                                    id = so.member.id;
                                    err = cs.name + ": member " + so.member.name + " not found";
                                    break;
                                }
                            }
                            else {
                                id = so.member.id;
                                err = cs.name + ": member " + so.member.name + " spec not found";
                                break;
                            }
                        }
                        else {
                            err = "struct type " + struct_type.type_str + " not found";
                            break;
                        }
                    }
                    else {
                        err = "struct var " + previous_name + " not found";
                        break;
                    }
                }
            }
            else {
                op.apply_visitor(*this);
            }
        }
        if(err.size() > 0) {
            int id_t;
            if(id >= 0)
                id_t = id;
            else
                id_t = ast.id;
            error(id_t, err.c_str());
            return false;
        }

        return true;
    }

    ast::Type Expression::operator()(ast::primary_expression& ast)
    {
        qDebug() << "Processing: ast::primary_expression";

        if(!ast.apply_visitor(*this)) {
            return ast::Error;
        }

        ast::Type type = ast.apply_visitor(petr);
        if(ast.which() == 3) {
            type.lvalue = true;
        }

        if(type.is_set) {
            return type;
        }
        return ast::Error;
    }

    bool Expression::operator()(ast::optoken& ast)
    {
        qDebug() << "Processing: ast::optoken";
        BOOST_ASSERT(env);
        switch(ast)
        {
            case ast::op_assign: env->op(op_store); break;
            case ast::op_plus: env->op(op_add); break;
            case ast::op_minus: env->op(op_sub); break;
            case ast::op_times: env->op(op_mul); break;
            case ast::op_divide: env->op(op_div); break;

            case ast::op_equal: env->op(op_eq); break;
            case ast::op_not_equal: env->op(op_neq); break;
            case ast::op_less: env->op(op_lt); break;
            case ast::op_less_equal: env->op(op_lte); break;
            case ast::op_greater: env->op(op_gt); break;
            case ast::op_greater_equal: env->op(op_gte); break;

            case ast::op_logical_or: env->op(op_or); break;
            case ast::op_logical_and: env->op(op_and); break;
            default: BOOST_ASSERT(0); return false;
        }
        return true;
    }

    //returns false if not found, true if found
    bool Expression::operator()(ast::identifier& ast)
    {
        qDebug() << "Processing: ast::identifier";
        qDebug() << ast.name.c_str();
        auto var = env->lookup_var(ast.name);
        if(var != 0) {
            ast.type = env->stack[*var].type;
            qDebug() << "identifier type:" << ast.type.type_str.c_str();

            env->op(op_load, *var);
            env->held_value = *var;

            return true;
        }
        else {
            error(ast.id, "unknown identifier");
            return false;
        }
    }

    bool Expression::operator()(bool ast)
    {
        qDebug() << "Processing: bool";
        if(env) {
            env->op(ast ? op_true : op_false);
            return true;
        }
        else
            return false;
    }

    bool Expression::operator()(ast::Bool_Value ast)
    {
        qDebug() << "Processing: ast::Bool_Value";
        if(env) {
            env->op(ast.value ? op_true : op_false);
            return true;
        }
        else
            return false;
    }

    bool Expression::operator()(unsigned int ast)
    {
        qDebug() << "Processing: unsigned int";
        if(env) {
            env->op(op_int, ast);
            return true;
        }
        else
            return false;
    }

    bool Expression::operator()(ast::Int_Value ast)
    {
        qDebug() << "Processing: ast::Int_Value";
        if(env) {
            env->op(op_int, ast.value);
            return true;
        }
        else
            return false;
    }

    void scope::op(int a)
    {
        code.push_back(a);
        //size_ += 1;
    }

    void scope::op(int a, int b)
    {
        code.push_back(a);
        code.push_back(b);
        //size_ += 2;
    }

    void scope::op(int a, int b, int c)
    {
        code.push_back(a);
        code.push_back(b);
        code.push_back(c);
        //size_ += 3;
    }

    boost::shared_ptr<variable> _empty_;

    ast::Type scope::lookup_struct_type(std::string const& name)
    {
        auto i = lookup_struct(name);
        if(i != 0) {
            return i->type;
        }
        if(parent) {
            return parent->lookup_struct_type(name);
        }
        return ast::Error;
    }

    cstruct* scope::lookup_struct(std::string const& name)
    {
        auto i = structs.find(name);
        if(i != structs.end()) {
            return &(i->second);
        }
        if(parent) {
            return parent->lookup_struct(name);
        }
        return 0;
    }

    const int* scope::lookup_var(const std::string &name)
    {
        if(table.find(name) != table.end()) {
            return &table[name];
        }
        if(parent) {
            return parent->lookup_var(name);
        }
        return 0;
    }

    void scope::add_var(std::string const& name, ast::Type& type)
    {
        std::size_t n = offset;
        table[name] = n;
        stack[table[name]] = 0;
        if(type.pointer) {
            type.width = 1;
        }
        stack[table[name]].type = type;
        offset += type.width;
    }

//    bool global::operator()(ast::function_call const& ast)
//    {
//        BOOST_ASSERT(current_function != 0);
//// FIXME
////        if(ast.function_name.name == std::string("writelns")) {
////            std::string arg = boost::get<ast::identifier>(ast.args.front().lhs).name;
////            foreach(ast::assignment_expression arg, ast.args) {
////                //qDebug() << stack[variables[arg.expression]];
////            }
////            return true;
////        }

//        if(functions.find(ast.function_name.name) == functions.end()) {
//            //qDebug() << ast.function_name.id;
//            //qDebug() << "Function not found: " << ast.function_name.name.c_str();
//            error(ast.function_name.id, "Function not found: " + ast.function_name.name);
//            return false;
//        }

//        boost::shared_ptr<function> p = functions[ast.function_name.name];

//        if(p->nargs() != ast.args.size()) {
//            //qDebug() << ast.function_name.id;
//            error(ast.function_name.id, "Wrong number of arguments: " + ast.function_name.name);
//            return false;
//        }

//        foreach(ast::logical_OR_expression const& expr, ast.args)
//        {
//            if(!(*this)(expr))
//                return false;
//        }

//        current_function->op(
//            op_call,
//            p->nargs(),
//            p->get_address());
//        current_function->link_to(ast.function_name.name, p->get_address());

//        return true;
//    }

    bool global::operator()(ast::assignment_expression& ast)
    {
        qDebug() << "Processing: ast::assignment_expression";

        Expression expr(error_, current_scope);
        if(!expr(ast)) {
            return false;
        }

        return true;
    }

    bool global::operator()(ast::logical_OR_expression& ast)
    {
        qDebug() << "Processing: ast::logical_OR_expression";

        Expression expr(error_, current_scope);
        if(!expr(ast)) {
            return false;
        }

        return true;
    }

    ast::Type global::operator()(ast::type_specifier& t)
    {
        qDebug() << "Processing: ast::type_specifier";
        type_resolver tr(error_, current_scope);
        return t.apply_visitor(tr);
    }

    bool global::operator()(ast::declaration& ast)
    {
        qDebug() << "Processing: ast::declaration";
        BOOST_ASSERT(current_scope);

        ast.type = (*this)(ast.type_spec);
        if(ast.type == ast::Error) {
            return false;
        }

        if(ast.init_dec) {
            ast.init_dec->type = ast.type;
            if(!(*this)(*ast.init_dec)) {
                return false;
            }
            ast.type.pointer = ast.init_dec->type.pointer;
        }

        return true;
    }

    bool global::operator()(ast::declarator& ast)
    {
        qDebug() << "Processing: ast::declarator";

        if(current_scope->exists(ast.name.name)) {
            error(ast.name.id, "Duplicate variable: " + ast.name.name);
            return false;
        }

        return true;
    }

    bool global::operator()(ast::init_declarator& ast)
    {
        qDebug() << "Processing: ast::init_declarator";

        //eval lhs
        if(!(*this)(ast.dec)) {
            return false;
        }
        ast.type.pointer = ast.dec.pointer;

        //eval rhs if it exists
        if(ast.assign) {
            if(ast.assign->which() == 0) {
                Expression expr(error_, current_scope, ast.type);
                ast::logical_OR_expression& operand = boost::get<ast::logical_OR_expression&>(*ast.assign);
                if(!expr(operand)) {
                    return false;
                }
                current_scope->add_var(ast.dec.name.name, ast.type);
                current_scope->stack[current_scope->table[ast.dec.name.name]].type = ast.type;
                current_scope->op(op_store, *current_scope->lookup_var(ast.dec.name.name));
            }
            else {
                ast::allocation_expression& alloc_ = boost::get<ast::allocation_expression&>(*ast.assign);
                type_resolver tr(error_, current_scope);
                ast::Type t = boost::apply_visitor(tr, alloc_.type_spec);
                if(!ast.type.pointer) {
                    error(ast.dec.id, "not a pointer");
                    return false;
                }
                stack[stack_offset].type = t;
                for(unsigned int i=stack_offset; i<stack_offset+t.width; i++) {
                    stack[i] = 0;
                }
                current_scope->op(op_int, stack_offset);
                stack_offset += t.width;
                //current_scope->op(op_new, t.width);
                current_scope->add_var(ast.dec.name.name, ast.type);
                current_scope->op(op_store, *current_scope->lookup_var(ast.dec.name.name));
                return true;
            }
        }
        else {
            current_scope->add_var(ast.dec.name.name, ast.type);
            current_scope->stack[current_scope->table[ast.dec.name.name]].type = ast.type;
        }

        return true;
    }

    bool global::operator()(ast::statement& ast)
    {
        qDebug() << "Processing: ast::statement";
        BOOST_ASSERT(current_scope);
        return boost::apply_visitor(*this, ast);
    }

    bool global::operator()(ast::compound_statement& ast)
    {
        qDebug() << "Processing: ast::compound_statement";
        BOOST_ASSERT(current_scope);
        for(ast::statement& s : ast) {
            if(!(*this)(s))
                return false;
        }
        return true;
    }

    bool global::operator()(ast::if_statement& ast)
    {
        qDebug() << "Processing: ast::if_statement";
        BOOST_ASSERT(current_scope);

        Expression if_expr(error_, current_scope);
        if(!if_expr(ast.condition)) {
            return false;
        }

        current_scope->op(op_jump_if, 0);
        const int skip = current_scope->code.size() - 1;
        std::vector<int> code;
        scope* if_scope = new scope(code, current_scope->stack, current_scope->offset);
        if_scope->parent.reset(current_scope);
        current_scope = if_scope;
        if(!(*this)(ast.then)) {
            current_scope = if_scope->parent.get();
            return false;
        }
        current_scope->parent->code.insert(current_scope->parent->code.end(),code.begin(),code.end());
        current_scope->parent->code[skip] = current_scope->code.size();
        //skip = current_scope->code.size()-skip;// now we know where to jump to (after the if branch)

        current_scope = if_scope->parent.get();

        return true;
    }

    bool global::operator()(ast::while_statement& ast)
    {
        qDebug() << "Processing: ast::while_statement";
        BOOST_ASSERT(current_scope);

        const int loop = current_scope->code.size();// mark our position
        Expression while_expr(error_, current_scope);
        if(!while_expr(ast.condition)) {
            return false;
        }

        current_scope->op(op_jump_if, 0);// we shall fill this (0) in later
        const int exit = current_scope->code.size()-1;// mark its position

        std::vector<int> code;
        scope* while_scope = new scope(code, current_scope->stack, current_scope->offset);
        while_scope->parent.reset(current_scope);
        current_scope = while_scope;

        if(!(*this)(ast.body))
            return false;

        current_scope->parent->code.insert(current_scope->parent->code.end(),code.begin(),code.end());
        current_scope = while_scope->parent.get();
        current_scope->op(op_jump,
            (loop-1) - int(current_scope->code.size()));// loop back
        current_scope->code[exit] = current_scope->code.size();

        return true;
    }

    bool global::operator()(ast::return_statement& ast)
    {
        qDebug() << "Processing: ast::return_statement";
//        if(current_function->void_return)
//        {
//            if(ast.expr)
//            {
//                //qDebug() << ast.id;
//                error(ast.id, "'void' function returning a value: ");
//                return false;
//            }
//        }
//        else
//        {
//            if(!ast.expr)
//            {
//                //qDebug() << ast.id;
//                error(ast.id, current_function_name + " function must return a value: ");
//                return false;
//            }
//        }

        if(ast.expr)
        {
            if(!(*this)(*ast.expr))
                return false;
        }
        current_scope->op(op_return);
        return true;
    }

    bool global::operator()(ast::function_definition& ast)
    {
        qDebug() << "Processing: ast::function_definition";
        //void_return = ast.return_type == "void";
//        if(ast.return_type_code) {
//            return_type = 1;
//        }
        error(ast.id, "Functions not implemented");
        return false;
        if(functions.find(ast.dec.name.name) != functions.end())
        {
            //qDebug() << ast.function_name.id;
            error(ast.dec.name.id, "Duplicate function: " + ast.dec.name.name);
            return false;
        }
        //offset = this->variables.size();
//        foreach(function_table::value_type const& fun, functions) {
//            offset += fun.second.get()->nvars();
//        }

        boost::shared_ptr<function>& p = functions[ast.dec.name.name];
        p.reset(new function(ast.args.size(), stack_offset, current_scope));

        // op_stk_adj 0 for now. we'll know how many variables
        // we'll have later and add them
        current_scope->op(op_stk_adj, 0);
        for(ast::argument& arg : ast.args) {
            type_resolver tr(error_, current_scope);
            ast::Type t = arg.type_spec.apply_visitor(tr);
            if(t == ast::Error) {
                error(arg.id, "Invalid type");
                return false;
            }
            current_scope->add_var(arg.dec.dec.name.name, t);
        }

//        foreach(ast::statement const& state, ast.body) {
//            if(!boost::apply_visitor(*this,state))
//                return false;
//        }
        //(*current_function)[1] = current_function->nvars();   // now store the actual number of variables
                                            // this includes the arguments
        //offset += current_function->nvars();
        return true;
    }

//    bool global::operator()(ast::struct_member_declaration const& /*ast*/)
//    {
//        return true;
//    }

    bool global::operator()(ast::struct_specifier& ast)
    {
        qDebug() << "Processing: ast::struct_specifier";
        std::map<std::string, int> members;
        foreach(boost::recursive_wrapper<ast::struct_member_declaration> const& mem, ast.members) {
            //members[mem.get().dec.name.name] = members.size();
        }
//        structs.push_back(cstruct(ast.type_name.name,members));

        return true;
    }

    bool global::operator()(ast::translation_unit& ast) {
        for(ast::statement_or_function& fs : ast) {
            if(!fs.apply_visitor(*this)) {
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
        parser::function parser(error);
        bool success = phrase_parse(iter, end, +parser, skip, ast);

        if(success) {
            success = compiler(ast);
            if(success) {
                qDebug() << "SUCCESS";
                ast.clear();
            }
            else {
                qDebug() << "FAIL";
                msg_box.setText("Semantic analysis failed");
                msg_box.setInformativeText("");
                msg_box.setStandardButtons(QMessageBox::Close);
                msg_box.setDefaultButton(QMessageBox::Close);
                if(error_buf) {
                    msg_box.setDetailedText(*error_buf);
                    msg_box.setStyleSheet("QAbstractScrollArea { font-family: \"monospace\"; }");
                    qDebug() << *error_buf;
                    error_buf->clear();
                }
                msg_box.exec();
            }
        }
        else {
            qDebug() << "PARSE FAIL";
            msg_box.setText("Parsing failed");
            msg_box.setInformativeText("Syntax error");
            msg_box.setStandardButtons(QMessageBox::Close);
            msg_box.setDefaultButton(QMessageBox::Close);
            if(error_buf) {
                msg_box.setDetailedText(*error_buf);
                msg_box.setStyleSheet("QAbstractScrollArea { font-family: \"monospace\"; }");
                qDebug() << *error_buf;
                error_buf->clear();
            }
            msg_box.exec();
        }
        ast.clear();

        return success;
    }

    bool Interpreter::parse(QString input) {
        return parse(input.toStdString());
    }

    int Interpreter::execute(std::vector<int> const& code,
                             std::vector<int>::const_iterator pc,
                             std::vector<variable>::iterator frame_ptr,
                             std::vector<variable>::iterator stack_ptr)
    {
        qDebug() << "code: ";
        std::stringstream ss;
        for(auto i : code)
            ss << " " << i;
        qDebug() << ss.str().c_str();
        //std::vector<int>::iterator stack_ptr = frame_ptr;
        std::vector<int>::const_iterator start = pc;
        while(pc<code.end())
        {
            if(stack_ptr - frame_ptr > DDVS_STACK_SIZE-3) {
                msg_box.setText("Stack overflow!");
                msg_box.setInformativeText("You should be able to safely resume after this box disappears.");
                msg_box.setStandardButtons(QMessageBox::Close);
                msg_box.setDefaultButton(QMessageBox::Close);
                msg_box.exec();
                return -1;
            }
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

                case op_dereference:
                    stack_ptr[-1] = frame_ptr[stack_ptr[-1].var];
                    break;

                case op_address:
                    stack_ptr[-1] = *pc++;
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
                    //FIXME needs modifying for structs
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
//                        int r = execute(code, pos, stack_ptr - nargs);
                        //FIXME don't think this works at all
                        int r = execute(code, pos, frame_ptr, stack_ptr);

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
                    //return stack_ptr[-1];
                case op_void:
                    return 0;
            }
        }
        return 0;
    }
}

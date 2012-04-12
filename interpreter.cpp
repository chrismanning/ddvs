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
                if(tmp == -1) {
                    return false;
                }
                if(!(*this)(ast.rhs->operand_)) {
                    return false;
                }
                if(!(*this)(ast.rhs->operator_)) {
                    return false;
                }
                env->op(tmp);
                env->held_value = -1;
                return true;
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
            BOOST_FOREACH(ast::logical_OR_op& expr, ast.rest) {
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
            BOOST_FOREACH(ast::logical_AND_op& expr, ast.rest) {
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
            BOOST_FOREACH(ast::equality_op& expr, ast.rest) {
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
            BOOST_FOREACH(ast::relational_op& expr, ast.rest) {
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

    template<typename T>
    bool Expression::operator()(std::list<T>& ops)
    {
        std::vector<int> tmp_code;
        return false;
    }

    bool Expression::operator()(ast::additive_expression& ast)
    {
        qDebug() << "Processing: ast::additive_expression";

        if(!(*this)(ast.lhs)) {
            return false;
        }
        if(ast.rest.size() > 0) {
            BOOST_FOREACH(ast::additive_op& expr, ast.rest) {
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
            BOOST_FOREACH(ast::multiplicative_op& expr, ast.rest) {
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
                    if(ast.operand_.type.pointer) {
                        env->op(op_dereference);
                        ast.type = ast.operand_.type;
                        ast.type.pointer = false;
                    }
                    else {
                        error(ast.operand_.id, "type error");
                        return false;
                    }
                    break;
                case ast::op_address:
                    if(ast.operand_.type.lvalue) {
                        env->op(op_address);
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

    bool Expression::operator()(ast::struct_expr& ast)
    {
        qDebug() << "Processing: ast::struct_expr";
        return false;
    }

    bool Expression::operator()(ast::postfix_expression& ast)
    {
        qDebug() << "Processing: ast::postfix_expression";

        ast.type = (*this)(ast.first);
        if(ast.type == ast::Error) {
            error(ast.id, "primary expression type error");
            return false;
        }

//        case ast::op_select_point:
//            if(ast.operand_.type == "struct" && ast.operand_.type.pointer) {
//                env->op(op_select_point);
//            }
//            else {
//                error(ast.operand_.id, "type error");
//                return false;
//            }
//            break;
//        case ast::op_select_ref:
//            if(ast.operand_.type == "struct") {
//                env->op(op_select_ref);
//            }
//            else {
//                error(ast.operand_.id, "type error");
//                return false;
//            }
//            break;
        //reverse this? + all the above foreach loops
        int num = 0;
        std::string previous_name, previous_member_name;
        const int* addr = 0;
        int member_offset = 0;
        std::string err;
        int id = -1;
        BOOST_FOREACH(ast::postfix_op& op, ast.rest) {
            if(op.which() == 0) {
                ast::struct_expr& so = boost::get<ast::struct_expr>(op);
                if(ast.type == "struct") {
                    std::string name;
                    if(num == 0) {
                        name = boost::get<ast::identifier>(ast.first).name;
                        //previous_name = name;
                        addr = env->lookup_var(name);
                        ++num;
                    }
                    else {
                        auto i = env->structs.find(previous_name);
                        qDebug() << "struct type:" << previous_name.c_str();
                        if(i != env->structs.end()) {
                            if(addr != 0) {
                                member_offset = i->second.member_offset(previous_member_name, *addr);
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
                    if(addr != 0) {
                        ast::Type struct_type = env->stack[*addr + member_offset].type;
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
                        auto i = env->structs.find(type_name);
                        if(i != env->structs.end()) {
                            cstruct& cs = i->second;
                            previous_member_name = so.member.name;
                            //find member spec
                            auto j = cs.member_specs.find(so.member.name);
                            typedef std::pair<std::string, ast::Type> member_spec__;
                            BOOST_FOREACH(member_spec__ v, cs.member_specs) {
                                qDebug() << v.first.c_str() << ":" << v.second.type_str.c_str();
                            }
                            if(j != cs.member_specs.end()) {
                                auto member_type = j->second;
                                if(member_type == "struct") {
                                    ast.type = member_type;
                                    qDebug() << "member type:" << member_type.type_str.c_str();
                                    continue;
                                }

                                //find member offset
                                qDebug() << "Loading member"
                                         << so.member.name.c_str()
                                         << "from struct"
                                         << cs.name.c_str();
                                auto member_addr = i->second.member_offset(previous_member_name, member_offset);
//                                auto k = cs.members.find(so.member.name);
//                                if(k != cs.members.end()) {
//                                    auto member_addr = k->second;
//                                    member_offset = member_addr;
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
            if(ast.type == "struct") {
                return true;
            }
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
            env->op(op_int, ast);
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
        auto i = structs.find(name);
        if(i != structs.end()) {
            return i->second.type;
        }
        if(parent) {
            return parent->lookup_struct_type(name);
        }
        return ast::Error;
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

    void scope::add_var(std::string const& name, ast::Type type)
    {
        std::size_t n = offset;
        table[name] = n;
        stack[table[name]] = 0;
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
            Expression expr(error_, current_scope, ast.type);
            if(!expr(*ast.assign)) {
                return false;
            }
            current_scope->add_var(ast.dec.name.name, ast.type);
            current_scope->stack[current_scope->table[ast.dec.name.name]].type = ast.type;
            current_scope->op(op_store, *current_scope->lookup_var(ast.dec.name.name));
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
        BOOST_FOREACH(ast::statement& s, ast)
        {
            if(!(*this)(s))
                return false;
        }
        return true;
    }

    bool global::operator()(ast::if_statement& ast)
    {
        qDebug() << "Processing: ast::if_statement";
        BOOST_ASSERT(current_scope);
        if(!(*this)(ast.condition))
            return false;
        current_scope->op(op_jump_if, 0);                 // we shall fill this (0) in later
//        std::size_t skip = current_function->size()-1;       // mark its position
        if(!(*this)(ast.then))
            return false;
//        (*current_function)[skip] = current_function->size()-skip;    // now we know where to jump to (after the if branch)

        if(ast.else_)                                // We got an else
        {
//            (*current_function)[skip] += 2;                  // adjust for the "else" jump
            current_scope->op(op_jump, 0);                // we shall fill this (0) in later
//            std::size_t exit = current_function->size()-1;   // mark its position
            if(!(*this)(*ast.else_))
                return false;
//            (*current_function)[exit] = current_function->size()-exit;// now we know where to jump to (after the else branch)
        }

        return true;
    }

    bool global::operator()(ast::while_statement& ast)
    {
        qDebug() << "Processing: ast::while_statement";
        BOOST_ASSERT(current_scope);
//        std::size_t loop = current_function->size();         // mark our position
        if(!(*this)(ast.condition))
            return false;
//        current_function->op(op_jump_if, 0);                 // we shall fill this (0) in later
//        std::size_t exit = current_function->size()-1;       // mark its position
        if(!(*this)(ast.body))
            return false;
//        current_function->op(op_jump,
//            int(loop-1) - int(current_function->size()));    // loop back
//        (*current_function)[exit] = current_function->size()-exit;    // now we know where to jump to (to exit the loop)
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

//    bool global::operator()(ast::function_definition const& ast)
//    {
//        //void_return = ast.return_type == "void";
////        if(ast.return_type_code) {
////            return_type = 1;
////        }
//        if(functions.find(ast.function_name.name) != functions.end())
//        {
//            //qDebug() << ast.function_name.id;
//            error(ast.function_name.id, "Duplicate function: " + ast.function_name.name);
//            return false;
//        }
//        //offset = this->variables.size();
////        foreach(function_table::value_type const& fun, functions) {
////            offset += fun.second.get()->nvars();
////        }

//        boost::shared_ptr<function>& p = functions[ast.function_name.name];
//        p.reset(new function(variables, pointers, code, ast.args.size(), offset));
//        current_function = p.get();
//        current_function_name = ast.function_name.name;

//        // op_stk_adj 0 for now. we'll know how many variables
//        // we'll have later and add them
//        current_function->op(op_stk_adj, 0);
//        foreach(ast::arg const& arg, ast.args)
//        {
//            current_function->add_var(arg.dec.name.name);
//        }

//        foreach(ast::statement const& state, ast.body) {
//            if(!boost::apply_visitor(*this,state))
//                return false;
//        }
//        (*current_function)[1] = current_function->nvars();   // now store the actual number of variables
//                                            // this includes the arguments
//        offset += current_function->nvars();
//        return true;
//    }

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

    bool global::operator()(ast::function_definition& ast)
    {
        qDebug() << "Processing: ast::function_definition";
        return false;
    }

    bool global::operator()(ast::translation_unit& ast) {
        BOOST_FOREACH(ast::statement_or_function& fs, ast) {
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
                    std::cout << stack_ptr[-1] << std::endl;
                    std::cout << stack_ptr[0] << std::endl;
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
    }
}

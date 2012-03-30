/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef AST_H
#define AST_H

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>
#include <list>
#include <QDebug>

namespace ast
{
    ///////////////////////////////////////////////////////////////////////////
    //  The AST
    ///////////////////////////////////////////////////////////////////////////
    struct tagged
    {
        int id; // Used to annotate the AST with the iterator position.
                // This id is used as a key to a map<int, Iterator>
                // (not really part of the AST.)
    };

    struct nil {};
    struct unary;
    struct function_call;
    struct expression;

    struct identifier : tagged
    {
        identifier(std::string const& name = "") : name(name) {}
        std::string name;
    };

    struct type_id : tagged
    {
        type_id(std::string const& name = "") : name(name) {}
        std::string name;
    };

    struct var_type
    {
        int type_code;
        boost::optional<char> pointer;
    };

    typedef std::list<type_id> type_list;

    typedef boost::variant<
            nil
          , bool
          , unsigned int
          , identifier
          , boost::recursive_wrapper<unary>
          , boost::recursive_wrapper<function_call>
          , boost::recursive_wrapper<expression>
        >
    operand;

    enum optoken
    {
        // precedence 1
        op_comma,

        // precedence 2
        op_assign,

        // precedence 3
        op_logical_or,

        // precedence 4
        op_logical_and,

        // precedence 5
        op_equal,
        op_not_equal,

        // precedence 6
        op_less,
        op_less_equal,
        op_greater,
        op_greater_equal,

        // precedence 7
        op_plus,
        op_minus,

        // precedence 8
        op_times,
        op_divide,

        // precedence 9
        op_positive,
        op_negative,
        op_not,
        op_address,
        op_indirection,

        // precedence 10
        op_select_point,
        op_select_ref
    };

    struct unary
    {
        optoken operator_;
        operand operand_;
    };

    struct operation
    {
        optoken operator_;
        operand operand_;
    };

    struct function_call
    {
        identifier function_name;
        std::list<expression> args;
    };

    struct expression
    {
        operand first;
        std::list<operation> rest;
    };

    struct assignment
    {
        identifier lhs;
        expression rhs;
    };

    struct variable_declaration
    {
        var_type type;
        identifier name;
        boost::optional<expression> rhs;
    };

    struct pointer_declaration
    {
        int type_code;
        identifier name;
        boost::optional<expression> rhs;
    };

    struct struct_instantiation
    {
        type_id type_name;
        identifier name;
    };

    typedef boost::variant<var_type, struct_instantiation> struct_member_type;
    struct struct_member_declaration
    {
        struct_member_type type;
        identifier name;
    };

    struct struct_declaration
    {
        type_id type_name;
        std::list<struct_member_declaration> members;
    };

    typedef std::list<struct_declaration> struct_list;

    struct if_statement;
    struct while_statement;
    struct statement_list;
    struct return_statement;

    typedef boost::variant<
            variable_declaration
          , struct_member_declaration
          , struct_declaration
          , struct_instantiation
          , assignment
          , function_call
          , boost::recursive_wrapper<if_statement>
          , boost::recursive_wrapper<while_statement>
          , boost::recursive_wrapper<return_statement>
          , boost::recursive_wrapper<statement_list>
        >
    statement;

    struct statement_list : std::list<statement> {};

    struct if_statement
    {
        expression condition;
        statement then;
        boost::optional<statement> else_;
    };

    struct while_statement
    {
        expression condition;
        statement body;
    };

    struct return_statement : tagged
    {
        boost::optional<expression> expr;
    };

    struct function;
    //typedef boost::variant<statement,boost::recursive_wrapper<function> > fs;
    typedef statement_list function_body;
    struct arg
    {
        var_type type_code;
        identifier name;
    };

    struct function
    {
        var_type return_type_code;
        identifier function_name;
        std::list<arg> args;
        function_body body;
    };

    typedef std::list<function> function_list;

    typedef boost::variant<statement_list,function> func_state;
    typedef std::list<func_state> main_function;

    // print functions for debugging
    inline std::ostream& operator<<(std::ostream& out, nil)
    {
        out << "nil"; return out;
    }

    inline std::ostream& operator<<(std::ostream& out, identifier const& id)
    {
        out << id.name; return out;
    }
}

BOOST_FUSION_ADAPT_STRUCT(
    ast::unary,
    (ast::optoken, operator_)
    (ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::operation,
    (ast::optoken, operator_)
    (ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::function_call,
    (ast::identifier, function_name)
    (std::list<ast::expression>, args)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::expression,
    (ast::operand, first)
    (std::list<ast::operation>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::var_type,
    (int, type_code)
    (boost::optional<char>, pointer)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::variable_declaration,
    (ast::var_type, type)
    (ast::identifier, name)
    (boost::optional<ast::expression>, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::struct_member_declaration,
    (ast::struct_member_type, type)
    (ast::identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::struct_declaration,
    (ast::type_id, type_name)
    (std::list<ast::struct_member_declaration>, members)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::struct_instantiation,
    (ast::type_id, type_name)
    (ast::identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::assignment,
    (ast::identifier, lhs)
    (ast::expression, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::if_statement,
    (ast::expression, condition)
    (ast::statement, then)
    (boost::optional<ast::statement>, else_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::while_statement,
    (ast::expression, condition)
    (ast::statement, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::return_statement,
    (boost::optional<ast::expression>, expr)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::arg,
    (ast::var_type, type_code)
    (ast::identifier,name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::function,
    (ast::var_type, return_type_code)
    (ast::identifier, function_name)
    (std::list<ast::arg>, args)
    (ast::function_body, body)
)

#endif // AST_H

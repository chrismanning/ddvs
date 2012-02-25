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

    struct type
    {
        type(std::string const& name = "") : name(name) {}
        std::string name;
    };

    typedef std::list<type> type_list;

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
        op_plus_assign,
        op_minus_assign,
        op_times_assign,
        op_divide_assign,
        op_mod_assign,
        op_bit_and_assign,
        op_bit_xor_assign,
        op_bitor_assign,
        op_shift_left_assign,
        op_shift_right_assign,

        // precedence 3
        op_logical_or,

        // precedence 4
        op_logical_and,

        // precedence 5
        op_bit_or,

        // precedence 6
        op_bit_xor,

        // precedence 7
        op_bit_and,

        // precedence 8
        op_equal,
        op_not_equal,

        // precedence 9
        op_less,
        op_less_equal,
        op_greater,
        op_greater_equal,

        // precedence 10
        op_shift_left,
        op_shift_right,

        // precedence 11
        op_plus,
        op_minus,

        // precedence 12
        op_times,
        op_divide,
        op_mod,

        // precedence 13
        op_positive,
        op_negative,
        op_pre_incr,
        op_pre_decr,
        op_compl,
        op_not,
        op_address,
        op_indirection,

        // precedence 14
        op_post_incr,
        op_post_decr,
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
        int type_code;
        identifier name;
        boost::optional<expression> rhs;
    };

    struct struct_member_declaration
    {
        int type_code;
        identifier name;
    };

    struct struct_declaration
    {
        type type_name;
        std::list<struct_member_declaration> members;
    };

    typedef std::list<struct_declaration> struct_list;

    struct struct_instantiation
    {
        type type_name;
        identifier name;
    };

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
        int type_code;
        identifier name;
    };

    struct function
    {
        int return_type_code;
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
    ast::variable_declaration,
    (int, type_code)
    (ast::identifier, name)
    (boost::optional<ast::expression>, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::struct_member_declaration,
    (int, type_code)
    (ast::identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::struct_declaration,
    (ast::type, type_name)
    (std::list<ast::struct_member_declaration>, members)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::struct_instantiation,
    (ast::type, type_name)
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
    (int, type_code)
    (ast::identifier,name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::function,
    (int, return_type_code)
    (ast::identifier, function_name)
    (std::list<ast::arg>, args)
    (ast::function_body, body)
)

#endif // AST_H

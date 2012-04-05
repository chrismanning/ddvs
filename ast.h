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
#include <boost/tuple/tuple.hpp>
#include <boost/preprocessor/cat.hpp>
#include <list>
#include <QDebug>

#define DEFINE_EXPRESSION_STRUCTS(a,b) \
    struct BOOST_PP_CAT(a,_op) {\
        optoken operator_;\
        b operand_;\
        Type type;\
    };\
    struct BOOST_PP_CAT(a,_expression) {\
        b operand_;\
        std::list<BOOST_PP_CAT(a,_op)> rest;\
        Type type;\
    };
#define DEFINE_EXPRESSION_TUPLE(a,b) \
    BOOST_FUSION_ADAPT_STRUCT(\
        ast::BOOST_PP_CAT(a,_op),\
        (ast::optoken, operator_)\
        (ast::b, operand_)\
    )\
    BOOST_FUSION_ADAPT_STRUCT(\
        ast::BOOST_PP_CAT(a,_expression),\
        (ast::b, operand_)\
        (std::list<ast::BOOST_PP_CAT(a,_op)>, rest)\
    )

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

    struct Type {
        Type() : is_set(false) {}
        Type(std::string const& type_str, bool const& pointer)
            : type_str(type_str), pointer(pointer), is_set(true)
        {}
        std::string type_str;
        bool pointer;
        bool lvalue;
        bool is_set;

        bool operator==(Type const& b) {
            return (type_str == b.type_str) && (pointer == b.pointer);
        }
        bool operator!=(Type const& b) {
            return !((*this) == b);
        }
    };

    static Type Void("void", false);
    static Type Int("int", false);
    static Type Bool("bool", false);
    static Type Struct("struct", false);
    static Type Error("error", false);

    struct nil {};
    struct assignment_expression;

    struct identifier : tagged
    {
        identifier(std::string const& name = "") : name(name){}
        std::string name;
        Type type;
    };

    struct type_id : tagged
    {
        type_id(std::string const& name = "") : name(name) {}
        std::string name;
    };

    typedef boost::variant<
            nil
          , bool
          , unsigned int
          , identifier
          , boost::recursive_wrapper<assignment_expression>
        >
    operand;
    typedef operand primary_expression;

    enum optoken
    {
        // precedence 1
        op_comma,

        // precedence 2
        op_assign,
        op_assign_add,
        op_assign_sub,
        op_assign_mul,
        op_assign_div,

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
        op_new,
        op_delete,

        // precedence 10
        op_select_point,
        op_select_ref,
        op_post_inc,
        op_post_dec
    };

    struct struct_expr {
        optoken operator_;
        identifier member;
    };

    typedef boost::variant<struct_expr, optoken> postfix_op;
    struct postfix_expression
    {
        primary_expression first;
        std::list<postfix_op> rest;
    };

    struct unary_op
    {
        optoken operator_;
        primary_expression operand_;
    };

    typedef boost::variant<unary_op, postfix_expression> unary_expression;

    DEFINE_EXPRESSION_STRUCTS(multiplicative, unary_expression)
    DEFINE_EXPRESSION_STRUCTS(additive, multiplicative_expression)
    DEFINE_EXPRESSION_STRUCTS(relational, additive_expression)
    DEFINE_EXPRESSION_STRUCTS(equality, relational_expression)
    DEFINE_EXPRESSION_STRUCTS(logical_AND, equality_expression)
    DEFINE_EXPRESSION_STRUCTS(logical_OR, logical_AND_expression)

    struct operation
    {
        optoken operator_;
        operand operand_;
    };

    struct function_call
    {
        identifier function_name;
        std::list<assignment_expression> args;
    };

    struct unary_assign
    {
        optoken operator_;
        logical_OR_expression operand_;
    };

    struct assignment_expression
    {
        logical_OR_expression lhs;
        boost::optional<unary_assign> rhs;
    };

    struct declarator
    {
        bool pointer;
        identifier name;
    };

    struct init_declarator
    {
        declarator dec;
        boost::optional<assignment_expression> assign;
    };

    struct declaration;

    struct struct_member_declaration;

    struct struct_specifier
    {
        type_id type_name;
        std::list<boost::recursive_wrapper<struct_member_declaration> > members;
    };

    typedef boost::variant<Type, struct_specifier> type_specifier;

    struct struct_member_declaration
    {
        type_specifier type;
        declarator dec;
    };

    struct declaration : tagged
    {
        type_specifier type;
        boost::optional<init_declarator> declarator;
    };

    struct if_statement;
    struct while_statement;
    struct statement_list;
    struct return_statement;

    typedef boost::variant<
            declaration
          , assignment_expression
          , boost::recursive_wrapper<if_statement>
          , boost::recursive_wrapper<while_statement>
          , boost::recursive_wrapper<return_statement>
          , boost::recursive_wrapper<statement_list>
        >
    statement;

    struct statement_list : std::list<statement> {};

    typedef statement_list compound_statement;

    struct if_statement
    {
        assignment_expression condition;
        statement then;
        boost::optional<statement> else_;
    };

    struct while_statement
    {
        assignment_expression condition;
        statement body;
    };

    struct return_statement : tagged
    {
        boost::optional<assignment_expression> expr;
    };

    struct function_definition
    {
        type_specifier return_type;
        declarator declarator_;
        std::list<declaration> args;
        compound_statement body;
    };

    typedef std::list<boost::variant<statement, function_definition> > translation_unit;

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

DEFINE_EXPRESSION_TUPLE(multiplicative, unary_expression)
DEFINE_EXPRESSION_TUPLE(additive, multiplicative_expression)
DEFINE_EXPRESSION_TUPLE(relational, additive_expression)
DEFINE_EXPRESSION_TUPLE(equality, relational_expression)
DEFINE_EXPRESSION_TUPLE(logical_AND, equality_expression)
DEFINE_EXPRESSION_TUPLE(logical_OR, logical_AND_expression)

BOOST_FUSION_ADAPT_STRUCT(
    ast::function_definition,
    (ast::type_specifier, return_type)
    (ast::declarator, declarator_)
    (std::list<ast::declaration>, args)
    (ast::compound_statement, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::unary_op,
    (ast::optoken, operator_)
    (ast::primary_expression, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::operation,
    (ast::optoken, operator_)
    (ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::function_call,
    (ast::identifier, function_name)
    (std::list<ast::assignment_expression>, args)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::struct_expr,
    (ast::optoken, operator_)
    (ast::identifier, member)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::postfix_expression,
    (ast::primary_expression, first)
    (std::list<ast::postfix_op>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::declaration,
    (ast::type_specifier, type)
    (boost::optional<ast::init_declarator>, declarator)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::declarator,
    (bool, pointer)
    (ast::identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::init_declarator,
    (ast::declarator, dec)
    (boost::optional<ast::assignment_expression>, assign)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::struct_member_declaration,
    (ast::type_specifier, type)
    (ast::declarator, dec)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::struct_specifier,
    (ast::type_id, type_name)
    (std::list<boost::recursive_wrapper<ast::struct_member_declaration> >, members)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::assignment_expression,
    (ast::logical_OR_expression, lhs)
    (boost::optional<ast::unary_assign>, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::unary_assign,
    (ast::optoken, operator_)
    (ast::logical_OR_expression, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::if_statement,
    (ast::assignment_expression, condition)
    (ast::statement, then)
    (boost::optional<ast::statement>, else_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::while_statement,
    (ast::assignment_expression, condition)
    (ast::statement, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::return_statement,
    (boost::optional<ast::assignment_expression>, expr)
)

#endif // AST_H

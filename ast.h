/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef AST_H
#define AST_H

#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>
#include <boost/preprocessor/cat.hpp>
#include <list>
#include <QDebug>

#define DEFINE_EXPRESSION_STRUCTS(a,b) \
    struct BOOST_PP_CAT(a,_op) : Typed {\
        optoken operator_;\
        b rhs;\
    };\
    struct BOOST_PP_CAT(a,_expression) : Typed {\
        b lhs;\
        std::list<BOOST_PP_CAT(a,_op)> rest;\
    };
#define DEFINE_EXPRESSION_TUPLE(a,b) \
    BOOST_FUSION_ADAPT_STRUCT(\
        ast::BOOST_PP_CAT(a,_op),\
        (ast::optoken, operator_)\
        (ast::b, rhs)\
    )\
    BOOST_FUSION_ADAPT_STRUCT(\
        ast::BOOST_PP_CAT(a,_expression),\
        (ast::b, lhs)\
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
        Type(std::string const& type_str, unsigned int width, bool pointer = false, bool lvalue = false)
            : type_str(type_str), pointer(pointer), lvalue(lvalue), is_set(true), width(width)
        {}
        std::string type_str;
        bool pointer;
        bool lvalue;
        bool is_set;
        unsigned int width;

        bool operator==(Type const& b) {
            return (type_str == b.type_str) && (pointer == b.pointer);
        }

        bool operator==(std::string const& b) {
            return starts_with(type_str, b);
        }

        bool operator!=(Type const& b) {
            return !((*this) == b);
        }

        bool operator!=(std::string const& b) {
            return !((*this) == b);
        }

//        Type(Type const& b) {
//            _cp(b);
//        }
//        Type& operator=(Type const& b) {
//            _cp(b);
//        }

        explicit operator bool() {
            return is_set;
        }

    private:
        void _cp(Type const& b)
        {
            this->pointer = b.pointer;
            this->type_str = b.type_str;
            this->is_set = b.is_set;
            this->lvalue = b.lvalue;
            this->width = b.width;
        }

        bool starts_with(std::string const& a, std::string const& b)
        {
            if(a.size() >= b.size()) {
                return a.substr(0, b.size()) == b;
            }
            return false;
        }
    };

    static Type Void("void", 0, false);
    static Type Int("int", 1, false);
    static Type Bool("bool", 1, false);
    static Type Struct("struct", 1, false);
    static Type Error("error", 0, false);

    struct Typed : tagged
    {
        Type type;
    };

    struct Bool_Value;

    struct Int_Value : Typed
    {
        Int_Value() : value(0)
        {
            type = Int;
        }
        Int_Value(int a) : value(a)
        {
            type = Int;
        }
        //needed binary ops
        Int_Value operator+(Int_Value a)
        {
            return Int_Value(value + a.value);
        }
        Int_Value operator*(Int_Value a)
        {
            return Int_Value(value * a.value);
        }

        int value;
    };

    struct Bool_Value : Typed
    {
        Bool_Value() : value(false)
        {
            type = Bool;
        }
        Bool_Value(bool a) : value(a)
        {
            type = Bool;
        }

        bool value;
    };

    struct nil : Typed {
        nil()
        {
            type = Error;
        }
    };
    struct logical_OR_expression;

    struct identifier : Typed
    {
        identifier(std::string const& name = "") : name(name){}
        std::string name;
    };

    struct type_id : tagged
    {
        type_id(std::string const& name = "") : name(name) {}
        std::string name;
    };

    typedef boost::variant<
            nil
          , Bool_Value
          , Int_Value
          , identifier
          , boost::recursive_wrapper<logical_OR_expression>
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

    struct struct_expr : tagged {
        optoken operator_;
        identifier member;
    };

    typedef boost::variant<struct_expr, optoken> postfix_op;
    struct postfix_expression : Typed
    {
        primary_expression first;
        std::list<postfix_op> rest;
    };

    struct unary_op
    {
        optoken operator_;
        primary_expression operand_;
    };

    //typedef boost::variant<unary_op, postfix_expression> unary_expression;
    struct unary_expression : Typed
    {
        boost::optional<optoken> operator_;
        postfix_expression operand_;
    };

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
        std::list<logical_OR_expression> args;
    };

    struct allocation_expression;

    typedef boost::variant<logical_OR_expression, boost::recursive_wrapper<allocation_expression> > ass_exp;
    struct unary_assign : Typed
    {
        optoken operator_;
        ass_exp operand_;
    };

    struct assignment_expression : Typed
    {
        logical_OR_expression lhs;
        boost::optional<unary_assign> rhs;
    };

    struct declarator : Typed
    {
        bool pointer;
        identifier name;
    };

    struct init_declarator : Typed
    {
        declarator dec;
        boost::optional<ass_exp> assign;
    };

    struct declaration;

    struct struct_member_declaration;

    struct struct_specifier : Typed
    {
        type_id type_name;
        std::list<boost::recursive_wrapper<struct_member_declaration> > members;
    };

    typedef boost::variant<Type, struct_specifier> type_specifier;

    struct allocation_expression : Typed
    {
        optoken operator_;
        type_specifier type_spec;
    };

    struct struct_member_declaration : Typed
    {
        type_specifier type_spec;
        declarator dec;
    };

    struct declaration : Typed
    {
        type_specifier type_spec;
        boost::optional<init_declarator> init_dec;
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
        logical_OR_expression condition;
        statement then;
    };

    struct while_statement
    {
        logical_OR_expression condition;
        statement body;
    };

    struct return_statement : tagged
    {
        boost::optional<logical_OR_expression> expr;
    };

    struct argument : Typed
    {
        type_specifier type_spec;
        init_declarator dec;
    };

    struct function_definition : Typed
    {
        type_specifier return_type;
        declarator dec;
        std::list<argument> args;
        compound_statement body;
    };

    typedef boost::variant<statement, function_definition> statement_or_function;
    typedef std::list<statement_or_function> translation_unit;

    // print functions for debugging
    inline std::ostream& operator<<(std::ostream& out, nil)
    {
        out << "nil"; return out;
    }

    inline std::ostream& operator<<(std::ostream& out, identifier const& id)
    {
        out << id.name; return out;
    }

    inline std::ostream& operator<<(std::ostream& out, type_id const& id)
    {
        out << id.name; return out;
    }

    inline std::ostream& operator<<(std::ostream& out, Type const& id)
    {
        if(id.type_str.length() > 0) {
            out << id.type_str;
        }
        else
            out << "empty";
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, struct_member_declaration const& id)
    {
        out << id.dec.name; return out;
    }

    inline std::ostream& operator<<(std::ostream& out, boost::recursive_wrapper<struct_member_declaration> const& id)
    {
        auto s = id.get();
        out << s;
        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, Int_Value const& i)
    {
        out << i.value; return out;
    }

    inline std::ostream& operator<<(std::ostream& out, Bool_Value const& i)
    {
        out << i.value; return out;
    }
}

DEFINE_EXPRESSION_TUPLE(multiplicative, unary_expression)
DEFINE_EXPRESSION_TUPLE(additive, multiplicative_expression)
DEFINE_EXPRESSION_TUPLE(relational, additive_expression)
DEFINE_EXPRESSION_TUPLE(equality, relational_expression)
DEFINE_EXPRESSION_TUPLE(logical_AND, equality_expression)
DEFINE_EXPRESSION_TUPLE(logical_OR, logical_AND_expression)

BOOST_FUSION_ADAPT_STRUCT(
    ast::argument,
    (ast::type_specifier, type_spec)
    (ast::init_declarator, dec)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::identifier,
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::type_id,
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::allocation_expression,
    (ast::optoken, operator_)
    (ast::type_specifier, type_spec)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::function_definition,
    (ast::type_specifier, return_type)
    (ast::declarator, dec)
    (std::list<ast::argument>, args)
    (ast::compound_statement, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::unary_op,
    (ast::optoken, operator_)
    (ast::primary_expression, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::unary_expression,
    (boost::optional<ast::optoken>, operator_)
    (ast::postfix_expression, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::operation,
    (ast::optoken, operator_)
    (ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::function_call,
    (ast::identifier, function_name)
    (std::list<ast::logical_OR_expression>, args)
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
    (ast::type_specifier, type_spec)
    (boost::optional<ast::init_declarator>, init_dec)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::declarator,
    (bool, pointer)
    (ast::identifier, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::init_declarator,
    (ast::declarator, dec)
    (boost::optional<ast::ass_exp>, assign)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::struct_member_declaration,
    (ast::type_specifier, type_spec)
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
    (ast::ass_exp, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::if_statement,
    (ast::logical_OR_expression, condition)
    (ast::statement, then)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::while_statement,
    (ast::logical_OR_expression, condition)
    (ast::statement, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::return_statement,
    (boost::optional<ast::logical_OR_expression>, expr)
)

#endif // AST_H

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <boost/spirit/include/qi.hpp>
#include <annotation.h>
#include <ast.h>
#include <error.h>
#include <skipper.h>
#include <types.h>
#include <string>
#include <vector>

namespace parser
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

//    template <typename Iterator>
    struct expression : qi::grammar<Iterator, ast::expression(), skipper>
    {
        expression(error_handler& error) : expression::base_type(expr)
        {
            qi::_1_type _1;
            qi::_2_type _2;
            qi::_3_type _3;
            qi::_4_type _4;

            qi::char_type char_;
            qi::uint_type uint_;
            qi::_val_type _val;
            qi::raw_type raw;
            qi::lexeme_type lexeme;
            qi::alpha_type alpha;
            qi::alnum_type alnum;
            qi::bool_type bool_;

            using qi::on_error;
            using qi::on_success;
            using qi::fail;
            using boost::phoenix::function;

            typedef function<error_handler> error_handler_function;
            typedef function<annotation> annotation_function;

            // tokens
            binary_op.add
                ("||", ast::op_logical_or)
                ("&&", ast::op_logical_and)
                ("==", ast::op_equal)
                ("!=", ast::op_not_equal)
                ("<", ast::op_less)
                ("<=", ast::op_less_equal)
                (">", ast::op_greater)
                (">=", ast::op_greater_equal)
                ("+", ast::op_plus)
                ("-", ast::op_minus)
                ("*", ast::op_times)
                ("/", ast::op_divide)
                ;

            unary_op.add
                ("+", ast::op_positive)
                ("-", ast::op_negative)
                ("!", ast::op_not)
                ("*", ast::op_indirection)
                ("->", ast::op_select_point)
                (".", ast::op_select_ref)
                ("&", ast::op_address)
                ;

            keywords.add
                ("true")
                ("false")
                ("if")
                ("else")
                ("while")
                ("void")
                ("int")
                ("struct")
                ("return")
                ;

            // grammar
            expr =
                    unary_expr
                >> *(binary_op > unary_expr)
                ;

            unary_expr =
                    primary_expr
                |   (unary_op > primary_expr)
                ;

            primary_expr =
                    uint_
                |   function_call
                |   identifier
                |   bool_
                |   '(' > expr > ')'
                ;

            function_call =
                    (identifier >> '(')
                >   argument_list
                >   ')'
                ;

            argument_list = -(expr % ',');

            identifier =
                    !(keywords | types)
                >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
                ;

            ///////////////////////////////////////////////////////////////////////
            // Debugging and error handling and reporting support.
            BOOST_SPIRIT_DEBUG_NODES(
                (expr)
                (unary_expr)
                (primary_expr)
                (function_call)
                (argument_list)
                (identifier)
            );

            ///////////////////////////////////////////////////////////////////////
            // Error handling: on error in expr, call error_handler.
            on_error<qi::fail>(expr,
                error_handler_function(error)(
                    "Error! Expecting ", _4, _3));

            ///////////////////////////////////////////////////////////////////////
            // Annotation: on success in primary_expr, call annotation.
            on_success(primary_expr,
                annotation_function(error.iters)(_val, _1));
        }

        qi::rule<Iterator, ast::expression(), skipper> expr;
        qi::rule<Iterator, ast::operand(), skipper> unary_expr, primary_expr;
        qi::rule<Iterator, ast::function_call(), skipper> function_call;
        qi::rule<Iterator, std::list<ast::expression>(), skipper> argument_list;
        qi::rule<Iterator, std::string(), skipper> identifier;
        qi::symbols<char, ast::optoken> unary_op, binary_op;
        qi::symbols<char> keywords;
        primitive_types types;
    };
}

#endif // EXPRESSION_H

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <boost/spirit/include/qi.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <annotation.h>
#include <ast.h>
#include <error.h>
#include <skipper.h>
#include <types.h>
#include <string>
#include <vector>

#define DECLARE_RULE(r,data,name) qi::rule<Iterator, ast::name(), skipper> name;
#define DECLARE_SPIRIT_RULES(names) BOOST_PP_SEQ_FOR_EACH(DECLARE_RULE, , names)

namespace parser
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

//    template <typename Iterator>
    struct expression : qi::grammar<Iterator, ast::assignment_expression(), skipper>
    {
        expression(error_handler& error);

//        qi::rule<Iterator, ast::expression(), skipper> assignment_expression;
        DECLARE_SPIRIT_RULES(
            (primary_expression)
            (postfix_expression)
            (multiplicative_expression)
            (unary_expression)
            (additive_expression)
            (relational_expression)
            (equality_expression)
            (logical_AND_expression)
            (logical_OR_expression)
            (assignment_expression)
            (init_declarator)
            (declarator)
            (struct_expr)
        )
        //qi::rule<Iterator, ast::unary(), skipper> unary_expression;
        qi::rule<Iterator, ast::function_call(), skipper> function_call;
        qi::rule<Iterator, std::list<ast::assignment_expression>(), skipper> argument_list;
        qi::rule<Iterator, std::string(), skipper> identifier;
        qi::symbols<char, ast::optoken> unary_op, binary_op, struct_op, postfix_op,
                assign_op, relational_op, multiplicative_op, additive_op, equality_op,
                logical_and_op, logical_or_op;
        qi::symbols<char> keywords;
        primitive_types types;
    };
}

#endif // EXPRESSION_H

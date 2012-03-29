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
        expression(error_handler& error);

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

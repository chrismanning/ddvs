/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef STATEMENT_H
#define STATEMENT_H

#include <boost/spirit/include/qi.hpp>
#include <ast.h>
#include <expression.h>
#include <boost/lambda/lambda.hpp>
#include <QDebug>

namespace parser {
    struct statement : qi::grammar<Iterator, ast::statement_list(), skipper > {
        statement(error_handler& error);

        expression expr;

        qi::rule<Iterator, ast::statement_list(), skipper> statement_list, compound_statement;
        DECLARE_SPIRIT_RULES(
            (type_id)
            (if_statement)
            (while_statement)
            (return_statement)
            (type_specifier)
            (declaration)
            (init_declarator)
            (declarator)
            (struct_specifier)
            (struct_member_declaration)
        )//
        qi::rule<Iterator, ast::statement(), skipper> statement_;
        qi::rule<Iterator, std::string(), skipper> identifier;

        primitive_types& types;
    };
}

#endif // STATEMENT_H

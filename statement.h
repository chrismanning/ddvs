/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef STATEMENT_H
#define STATEMENT_H

#include <expression.h>

namespace parser {
    struct statement : qi::grammar<Iterator, ast::statement_list(), skipper > {
        statement(error_handler& error);

        expression expr;

        qi::rule<Iterator, ast::statement_list(), skipper> statement_list, compound_statement;
        DECLARE_SPIRIT_RULES(
            (if_statement)
            (while_statement)
            (return_statement)
            (declaration)
            (init_declarator)
            (identifier)
        )//
        qi::rule<Iterator, ast::statement(), skipper> statement_;

        primitive_types& types;
    };
}

#endif // STATEMENT_H

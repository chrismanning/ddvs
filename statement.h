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
//    template <typename Iterator>
    struct statement : qi::grammar<Iterator, ast::statement_list(), skipper > {
        statement(error_handler& error);

        expression expr;//<Iterator>

        qi::rule<Iterator, ast::statement_list(), skipper> statement_list, compound_statement;
        qi::rule<Iterator, ast::type_id(), skipper> type_id;
        qi::rule<Iterator, ast::statement(), skipper> statement_;
        qi::rule<Iterator, ast::if_statement(), skipper> if_statement;
        qi::rule<Iterator, ast::while_statement(), skipper> while_statement;
        qi::rule<Iterator, ast::return_statement(), skipper> return_statement;
        qi::rule<Iterator, ast::type_specifier(), skipper> type_specifier;
        qi::rule<Iterator, ast::declaration(), skipper> declaration;
        qi::rule<Iterator, ast::struct_specifier(), skipper> struct_specifier;
        qi::rule<Iterator, ast::expression(), skipper> expression_statement;
        qi::rule<Iterator, ast::struct_member_declaration(), skipper> struct_member_declaration;

        qi::rule<Iterator, std::string(), skipper> identifier;
        //        qi::rule<Iterator, std::string(), skipper > type_spec;
        //        qi::rule<Iterator, std::string(), skipper > pointer;

        //qi::symbols<char,ast::type> primitive_types;

        primitive_types& types;
    };
}

#endif // STATEMENT_H

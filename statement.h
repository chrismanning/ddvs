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
        qi::rule<Iterator, ast::function_call(), skipper> function_call_statement;
        qi::rule<Iterator, ast::variable_declaration(), skipper> variable_declaration;
        qi::rule<Iterator, ast::var_type(), skipper> var_type;
        qi::rule<Iterator, ast::struct_member_declaration(), skipper> struct_member_declaration;
        qi::rule<Iterator, ast::assignment(), skipper> assignment;
        qi::rule<Iterator, ast::if_statement(), skipper> if_statement;
        qi::rule<Iterator, ast::while_statement(), skipper> while_statement;
        qi::rule<Iterator, ast::return_statement(), skipper> return_statement;
        qi::rule<Iterator, ast::struct_declaration(), skipper> struct_declaration;
        qi::rule<Iterator, ast::struct_instantiation(), skipper> struct_instantiation;
        qi::rule<Iterator, std::string(), skipper> identifier;
        //        qi::rule<Iterator, std::string(), skipper > type_spec;
        //        qi::rule<Iterator, std::string(), skipper > pointer;

        //qi::symbols<char,ast::type> primitive_types;

        primitive_types& types;
    };
}

#endif // STATEMENT_H

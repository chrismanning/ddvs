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
        statement(error_handler& error) : statement::base_type(statement_list), expr(error),
            types(expr.types)
        {
            qi::_1_type _1;
            qi::_2_type _2;
            qi::_3_type _3;
            qi::_4_type _4;

            qi::char_type char_;
            qi::_val_type _val;
            qi::raw_type raw;
            qi::lexeme_type lexeme;
            qi::alpha_type alpha;
            qi::alnum_type alnum;
            qi::lit_type lit;

            using qi::on_error;
            using qi::on_success;
            using qi::fail;
            using boost::phoenix::function;

            typedef function<error_handler> error_handler_function;
            typedef function<annotation> annotation_function;

            statement_list =
                +statement_
                ;

            statement_ =
                    variable_declaration
                |   struct_member_declaration
                |   struct_declaration
                |   struct_instantiation
                |   assignment
                |   compound_statement
                |   function_call_statement
                |   if_statement
                |   while_statement
                |   return_statement
                ;

            identifier =
                    !(expr.keywords | types)
                >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
                ;

            function_call_statement =
                    expr.function_call
                >   ';'
                ;

            type_id = identifier;

            var_type = types > -lexeme[char_('*')];

            variable_declaration =
                    var_type
                >   identifier
                >   -('=' > expr)
                >   ';'
                ;

            struct_member_declaration =
                    lexeme[types]
                >   identifier
                >   ';'
                ;

            struct_declaration =
                    lexeme["struct" >> !(alnum | '_')]
                >   type_id //FIXME
                >   '{'
                >   *struct_member_declaration
                >   '}'
                >   ';'
                ;

            struct_instantiation =
                    lexeme["struct" >> !(alnum | '_')]
                >   type_id
                >   identifier
                >   ';'
                ;

            assignment =
                    identifier
                >   '='
                >   expr
                >   ';'
                ;

            if_statement =
                    lit("if")
                >   '('
                >   expr
                >   ')'
                >   statement_
                >
                   -(
                        lexeme["else" >> !(alnum | '_')] // make sure we have whole words
                    >   statement_
                    )
                ;

            while_statement =
                    lit("while")
                >   '('
                >   expr
                >   ')'
                >   statement_
                ;

            compound_statement =
                '{' >> -statement_list >> '}'
                ;

            return_statement =
                    lexeme["return" >> !(alnum | '_')] // make sure we have whole words
                >  -expr
                >   ';'
                ;

            // Debugging and error handling and reporting support.
            BOOST_SPIRIT_DEBUG_NODES(
                (statement_list)
                (identifier)
                (variable_declaration)
                (assignment)
            );

            // Error handling: on error in statement_list, call error_handler.
            on_error<qi::fail>(statement_list,
                error_handler_function(error)(
                    "Error! Expecting ", _4, _3));

            // Annotation: on success in variable_declaration,
            // assignment and return_statement, call annotation.
            on_success(variable_declaration,
                annotation_function(error.iters)(_val, _1));
            on_success(assignment,
                annotation_function(error.iters)(_val, _1));
            on_success(return_statement,
                annotation_function(error.iters)(_val, _1));
        }

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

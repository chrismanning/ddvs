/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef FUNCTION_H
#define FUNCTION_H

#include <statement.h>

namespace parser {
    //template <typename Iterator>
    struct function : qi::grammar<Iterator, ast::function(), skipper >
    {
        function(error_handler& error) : function::base_type(start), body(error)
        {
            qi::_1_type _1;
            qi::_2_type _2;
            qi::_3_type _3;
            qi::_4_type _4;

            qi::_val_type _val;
            qi::raw_type raw;
            qi::lexeme_type lexeme;
            qi::alpha_type alpha;
            qi::alnum_type alnum;
            qi::string_type string;

            using qi::on_error;
            using qi::on_success;
            using qi::fail;
            using boost::phoenix::function;

            typedef function<error_handler> error_handler_function;
            typedef function<annotation> annotation_function;

            name =
                    !body.expr.keywords
                >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
                ;

            identifier = name;
            argument_list = -((types > identifier) % ',');

            start =
                    lexeme[types] //| lexeme[(string("void") | string("int"))
                        //>> !(alnum | '_')]  // make sure we have whole words
                >   identifier
                >   '(' > argument_list > ')'
                >   '{'
                >   +(body.statement_ | start)
//                >   body
                >   '}'
                ;

            // Debugging and error handling and reporting support.
            BOOST_SPIRIT_DEBUG_NODES(
                (identifier)
                (argument_list)
                (start)
            );

            // Error handling: on error in start, call error_handler.
            on_error<qi::fail>(start,
                error_handler_function(error)(
                    "Error! Expecting ", _4, _3));

            // Annotation: on success in start, call annotation.
            on_success(identifier,
                annotation_function(error.iters)(_val, _1));
        }

        statement body;//<Iterator>
        qi::rule<Iterator, std::string(), skipper> name;
        qi::rule<Iterator, ast::identifier(), skipper> identifier;
        qi::rule<Iterator, std::list<ast::arg>(), skipper> argument_list;
        qi::rule<Iterator, ast::function(), skipper> start;

        return_types types;
    };
}

#endif // FUNCTION_H

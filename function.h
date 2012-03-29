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
        function(error_handler& error);

        statement body;//<Iterator>
        //qi::rule<Iterator, std::string(), skipper> name;
        qi::rule<Iterator, ast::identifier(), skipper> identifier;
        qi::rule<Iterator, std::list<ast::arg>(), skipper> argument_list;
        qi::rule<Iterator, ast::function(), skipper> start;

        primitive_types types;
    };
}

#endif // FUNCTION_H

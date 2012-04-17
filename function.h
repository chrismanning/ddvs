/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef FUNCTION_H
#define FUNCTION_H

#include <statement.h>

namespace parser {
    struct function : qi::grammar<Iterator, ast::translation_unit(), skipper >
    {
        function(error_handler& error);

        statement body;
        DECLARE_SPIRIT_RULES(
            (translation_unit)
            (function_definition)
            (argument)
        )//

        primitive_types types;
    };
}

#endif // FUNCTION_H

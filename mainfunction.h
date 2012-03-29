#ifndef MAINFUNCTION_H
#define MAINFUNCTION_H

#include <function.h>

namespace parser {
    //template <typename Iterator>
    struct main_function : qi::grammar<Iterator, ast::main_function(), skipper >
    {
        main_function(error_handler& error);

        primitive_types& types;

        function function_i;//<Iterator>
        statement statement_i;//<Iterator>
        qi::rule<Iterator, ast::main_function(), skipper > all;
    };
}

#endif // MAINFUNCTION_H

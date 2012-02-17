#ifndef MAINFUNCTION_H
#define MAINFUNCTION_H

#include <function.h>

namespace parser {
    //template <typename Iterator>
    struct main_function : qi::grammar<Iterator, ast::main_function(), skipper >
    {
        main_function(error_handler& error) : main_function::base_type(all), function_i(error), statement_i(error), types(statement_i.types)
        {
            all =
                    +(statement_i | function_i)
                ;
        }

        primitive_types& types;

        function function_i;//<Iterator>
        statement statement_i;//<Iterator>
        qi::rule<Iterator, ast::main_function(), skipper > all;
    };
}

#endif // MAINFUNCTION_H

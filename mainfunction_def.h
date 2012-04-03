#ifndef MAINFUNCTION_DEF_H
#define MAINFUNCTION_DEF_H

#include<mainfunction.h>

namespace parser {
    main_function::main_function(error_handler& error)
        : main_function::base_type(all), function_i(error), statement_i(error), types(statement_i.types)
    {
        all =
                +(statement_i | function_i)
            ;
    }
}
#endif // MAINFUNCTION_DEF_H

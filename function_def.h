#ifndef FUNCTION_DEF_H
#define FUNCTION_DEF_H

#include <function.h>

namespace parser {
    function::function(error_handler& error)
        : function::base_type(translation_unit), body(error)
    {
        qi::_1_type _1;
        qi::_2_type _2;
        qi::_3_type _3;
        qi::_4_type _4;

        qi::_val_type _val;

        using qi::on_error;
        using qi::on_success;
        using qi::fail;
        using boost::phoenix::function;

        typedef function<error_handler> error_handler_function;
        typedef function<annotation> annotation_function;

        translation_unit =
            +(  body
            |   function_definition
            )
            ;

        argument =
                body.expr.type_specifier
            >   body.init_declarator
            ;

        function_definition =
                body.expr.type_specifier
            >   body.expr.declarator
            >   '('
            >   -(argument % ',')
            >   ')'
            >   body.compound_statement
            ;

        // Debugging and error handling and reporting support.
        BOOST_SPIRIT_DEBUG_NODES(
            (translation_unit)
            (function_definition)
            (argument)
        );

        // Error handling
        on_error<qi::fail>(function_definition,
            error_handler_function(error)(
                "Error! Expecting ", _4, _3));

        // Annotation: on success in start, call annotation.
        SUCCESS_ANNOTATE(function_definition);
        SUCCESS_ANNOTATE(argument);
//        on_success(identifier,
//            annotation_function(error.iters)(_val, _1));
    }
}

#endif // FUNCTION_DEF_H

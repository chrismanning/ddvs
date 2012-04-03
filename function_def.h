#ifndef FUNCTION_DEF_H
#define FUNCTION_DEF_H

#include <function.h>

namespace parser {
    function::function(error_handler& error)
        : function::base_type(start), body(error)
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

        identifier = body.expr.identifier;
        argument_list = (body.var_type > identifier) % ',';

        start =
                body.type_specifier
            >   identifier
            >   '(' > -argument_list > ')'
            >   '{'
            >   body
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
}

#endif // FUNCTION_DEF_H

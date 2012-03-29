#ifndef STATEMENT_DEF_H
#define STATEMENT_DEF_H

#include <statement.h>

namespace parser {
    statement::statement(error_handler& error) : statement::base_type(statement_list), expr(error),
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

        identifier = expr.identifier
            ;

        function_call_statement =
                expr.function_call
            >   ';'
            ;

        type_id = identifier.alias();

        var_type = types > -lexeme[char_('*')];

        variable_declaration =
                var_type
            >   identifier
            >   -('=' > expr)
            >   ';'
            ;

        struct_member_declaration =
                (var_type | struct_instantiation)
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
}

#endif // STATEMENT_DEF_H

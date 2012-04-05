#ifndef STATEMENT_DEF_H
#define STATEMENT_DEF_H

#include <statement.h>

namespace parser {
    statement::statement(error_handler& error)
        : statement::base_type(statement_list), expr(error), types(expr.types)
    {
        qi::_1_type _1;
        qi::_2_type _2;
        qi::_3_type _3;
        qi::_4_type _4;

        qi::_val_type _val;
        qi::lexeme_type lexeme;
        qi::alnum_type alnum;
        qi::lit_type lit;

        using qi::on_error;
        using qi::on_success;
        using qi::fail;
        using boost::phoenix::function;

        typedef function<error_handler> error_handler_function;
        typedef function<annotation> annotation_function;

        statement_list =
            +statement_;

        statement_ =
                declaration
            |   expr > ';'
            |   if_statement
            |   while_statement
            |   return_statement
            |   compound_statement
            ;

        identifier = expr.identifier
            ;

        type_id = identifier.alias();

        declaration = type_specifier > -expr.init_declarator > ';';

        type_specifier =
                types
            |   struct_specifier
            ;

        struct_member_declaration = type_specifier > expr.declarator > ';';

        struct_specifier =
                lexeme["struct"] > type_id > -('{' > +struct_member_declaration > '}');

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
            (type_specifier)
            (type_id)
            (declaration)
        );

        // Error handling
        on_error<qi::fail>(statement_list,
            error_handler_function(error)(
                "Error! Expecting ", _4, _3));

        // Annotation: on success in declaration,
        // assignment and return_statement, call annotation.
        on_success(declaration,
            annotation_function(error.iters)(_val, _1));
        on_success(return_statement,
            annotation_function(error.iters)(_val, _1));
    }
}

#endif // STATEMENT_DEF_H

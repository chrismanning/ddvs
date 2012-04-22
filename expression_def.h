#ifndef EXPRESSION_DEF_H
#define EXPRESSION_DEF_H

#include <expression.h>

namespace parser {
    expression::expression(error_handler& error)
        : expression::base_type(assignment_expression)
    {
        qi::_1_type _1;
        qi::_2_type _2;
        qi::_3_type _3;
        qi::_4_type _4;

        qi::char_type char_;
        qi::uint_parser<ast::Int_Value> uint_;
        qi::_val_type _val;
        qi::matches_type matches;
        qi::raw_type raw;
        qi::lexeme_type lexeme;
        qi::alpha_type alpha;
        qi::alnum_type alnum;
        qi::bool_parser<ast::Bool_Value> bool_;

        using qi::on_error;
        using qi::on_success;
        using qi::fail;
        using qi::debug;
        using boost::phoenix::function;

        typedef function<error_handler> error_handler_function;
        typedef function<annotation> annotation_function;

        // operators from lowest to highest precedence
        assign_op.add
            ("=" , ast::op_assign)
            ;

        logical_or_op.add
            ("||", ast::op_logical_or)
            ;
        logical_and_op.add
            ("&&", ast::op_logical_and)
            ;

        equality_op.add
            ("==", ast::op_equal)
            ("!=", ast::op_not_equal)
            ;

        relational_op.add
            ("<" , ast::op_less)
            ("<=", ast::op_less_equal)
            (">" , ast::op_greater)
            (">=", ast::op_greater_equal)
            ;

        additive_op.add
            ("+", ast::op_plus)
            ("-", ast::op_minus)
            ;

        multiplicative_op.add
            ("*", ast::op_times)
            ("/", ast::op_divide)
            ;

        unary_op.add
            ("+", ast::op_positive)
            ("-", ast::op_negative)
            ("!", ast::op_not)
            ("*", ast::op_indirection)
            ("&", ast::op_address)
            ;

        memory_op.add
            ("new", ast::op_new)
            ;

        struct_op.add
            ("->", ast::op_select_point)
            ("." , ast::op_select_ref)
            ;

        postfix_op.add
            ("++", ast::op_post_inc)
            ("--", ast::op_post_dec)
            ;

        keywords.add
            ("true")
            ("false")
            ("if")
            ("while")
            ("struct")
            ("return")
            ("new")
            ("error")
            ;

        //expressions in reverse precedence
        assignment_expression = logical_OR_expression > -unary_assign;

        allocation_expression = memory_op > type_specifier;

        unary_assign = assign_op > (allocation_expression | logical_OR_expression);

        logical_OR_expression = logical_AND_expression > *(logical_or_op > logical_AND_expression);

        logical_AND_expression = equality_expression > *(logical_and_op > equality_expression);

        equality_expression = relational_expression > *(equality_op > relational_expression);

        relational_expression = additive_expression > *(relational_op > additive_expression);

        additive_expression = multiplicative_expression > *(additive_op > multiplicative_expression);

        multiplicative_expression = unary_expression > *(multiplicative_op > unary_expression);

        unary_expression =
                -unary_op
            >   postfix_expression
            ;

        struct_expr = struct_op > identifier;

        postfix_expression =
                primary_expression
            >   *(struct_expr | postfix_op)
            ;

        primary_expression =
                uint_
            |   identifier
            |   bool_
            |   ('(' > logical_OR_expression > ')')
            ;
        //end expressions

        type_specifier =
                types
            |   struct_specifier
            ;

        declarator = matches['*'] > identifier;

        struct_member_declaration = type_specifier > declarator > ';';

        struct_specifier =
                lexeme["struct"] > type_id > -('{' > +struct_member_declaration > '}');

        identifier =
                !(keywords | types)
            >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
            ;

        type_id =
                !(keywords | types)
            >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
            ;

        ///////////////////////////////////////////////////////////////////////
        // Debugging and error handling and reporting support.
        BOOST_SPIRIT_DEBUG_NODES(
            (assignment_expression)
            (unary_assign)
            (logical_OR_expression)
            (logical_AND_expression)
            (equality_expression)
            (relational_expression)
            (additive_expression)
            (multiplicative_expression)
            (unary_expression)
            (struct_expr)
            (postfix_expression)
            (primary_expression)
            (identifier)
            (struct_specifier)
            (type_id)
            (declarator)
            (struct_member_declaration)
            (allocation_expression)
            (type_specifier)
        );

        ///////////////////////////////////////////////////////////////////////
        // Error handling
        on_error<fail>(assignment_expression,
            error_handler_function(error)(
                "Error! Expecting ", _4, _3));
        on_error<fail>(identifier,
            error_handler_function(error)(
                "Error! Expecting ", _4, _3));
        on_error<fail>(declarator,
            error_handler_function(error)(
                "Error! Expecting ", _4, _3));
        on_error<fail>(type_specifier,
            error_handler_function(error)(
                "Error! Expecting ", _4, _3));
        on_error<fail>(allocation_expression,
            error_handler_function(error)(
                "Error! Expecting ", _4, _3));

        ///////////////////////////////////////////////////////////////////////
        // Annotation: on success in primary_expression, call annotation.
        SUCCESS_ANNOTATE(primary_expression);
        SUCCESS_ANNOTATE(postfix_expression);
        SUCCESS_ANNOTATE(multiplicative_expression);
        SUCCESS_ANNOTATE(unary_expression);
        SUCCESS_ANNOTATE(additive_expression);
        SUCCESS_ANNOTATE(relational_expression);
        SUCCESS_ANNOTATE(equality_expression);
        SUCCESS_ANNOTATE(logical_AND_expression);
        SUCCESS_ANNOTATE(logical_OR_expression);
        SUCCESS_ANNOTATE(assignment_expression);
        SUCCESS_ANNOTATE(struct_expr);
        SUCCESS_ANNOTATE(identifier);
        SUCCESS_ANNOTATE(unary_assign);
        SUCCESS_ANNOTATE(struct_specifier);
        SUCCESS_ANNOTATE(declarator);
        SUCCESS_ANNOTATE(allocation_expression);
    }
}
#endif // EXPRESSION_DEF_H

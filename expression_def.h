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
        qi::uint_type uint_;
        qi::_val_type _val;
        qi::raw_type raw;
        qi::lexeme_type lexeme;
        qi::matches_type matches;
        qi::alpha_type alpha;
        qi::alnum_type alnum;
        qi::bool_type bool_;

        using qi::on_error;
        using qi::on_success;
        using qi::fail;
        using boost::phoenix::function;

        typedef function<error_handler> error_handler_function;
        typedef function<annotation> annotation_function;

        // operators from lowest to highest precedence
        assign_op.add
            ("=", ast::op_assign)
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
            ("<", ast::op_less)
            ("<=", ast::op_less_equal)
            (">", ast::op_greater)
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
            ("new", ast::op_new)
            ("delete", ast::op_delete)
            ;

        struct_op.add
            ("->", ast::op_select_point)
            (".", ast::op_select_ref)
            ;

        postfix_op.add
            ("++", ast::op_post_inc)
            ("--", ast::op_post_dec)
            ;

        keywords.add
            ("true")
            ("false")
            ("if")
            ("else")
            ("while")
            ("struct")
            ("return")
            ("new")
            ("delete")
            ;

        // grammar
//        expr =
//                unary_expr
//            >> *(binary_op > unary_expr)
//            ;

        //expressions in reverse precedence
        assignment_expression = -(unary_expression > assign_op) > logical_OR_expression;

        logical_OR_expression = logical_AND_expression > *(logical_or_op > logical_AND_expression);

        logical_AND_expression = equality_expression > *(logical_and_op > equality_expression);

        equality_expression = relational_expression > *(equality_op > relational_expression);

        relational_expression = additive_expression > *(relational_op > additive_expression);

        additive_expression = multiplicative_expression > *(additive_op > multiplicative_expression);

        multiplicative_expression = unary_expression > *(multiplicative_op > unary_expression);

        unary_expression =
                (unary_op > primary_expression)
            |   postfix_expression
            ;

        postfix_expression =
                primary_expression
            >   *(struct_expr | postfix_op)
            ;

        struct_expr = struct_op > identifier;

        primary_expression =
                uint_
            |   identifier
            |   bool_
            |   '(' > assignment_expression > ')'
            ;
        //end expressions

        declarator = matches['*'] > identifier;

        init_declarator = declarator > -("="  > assignment_expression);

        function_call =
                (identifier >> '(')
            >   argument_list
            >   ')'
            ;

        argument_list = -(assignment_expression % ',');

        identifier =
                !(keywords | types)
            >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
            ;

        ///////////////////////////////////////////////////////////////////////
        // Debugging and error handling and reporting support.
        BOOST_SPIRIT_DEBUG_NODES(
            (assignment_expression)
            (unary_expression)
            (primary_expression)
            (function_call)
            (argument_list)
            (identifier)
        );

        ///////////////////////////////////////////////////////////////////////
        // Error handling: on error in expr, call error_handler.
        on_error<qi::fail>(assignment_expression,
            error_handler_function(error)(
                "Error! Expecting ", _4, _3));

        ///////////////////////////////////////////////////////////////////////
        // Annotation: on success in primary_expr, call annotation.
        on_success(primary_expression,
            annotation_function(error.iters)(_val, _1));
    }
}
#endif // EXPRESSION_DEF_H

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <map>
#include <boost/variant/apply_visitor.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/mpl/bool.hpp>
#include <ast.h>
#include <QDebug>

typedef std::string::const_iterator Iterator;

struct annotation
{
    template <typename, typename>
    struct result { typedef void type; };

    std::vector<Iterator>& iters;
    annotation(std::vector<Iterator>& iters)
      : iters(iters) {}

    struct set_id
    {
        typedef void result_type;

        int id;
        set_id(int id) : id(id) {}

        template <typename T>
        void operator()(T& x) const
        {
            this->dispatch(x, boost::is_base_of<ast::tagged, T>());
        }

        // This will catch all nodes except those inheriting from ast::tagged
        template <typename T>
        void dispatch(T& /*x*/, boost::mpl::false_) const
        {
            // (no-op) no need for tags
        }

        // This will catch all nodes inheriting from ast::tagged
        template <typename T>
        void dispatch(T& x, boost::mpl::true_) const
        {
            x.id = id;
        }
    };

    void operator()(ast::operand& ast, Iterator pos) const
    {
        int id = iters.size();
        iters.push_back(pos);
        boost::apply_visitor(set_id(id), ast);
    }

    void operator()(ast::declaration& ast, Iterator pos) const
    {
        int id = iters.size();
        iters.push_back(pos);
        ast.id = id;
    }

    void operator()(ast::return_statement& ast, Iterator pos) const
    {
        int id = iters.size();
        iters.push_back(pos);
        ast.id = id;
    }

    void operator()(ast::identifier& ast, Iterator pos) const
    {
        int id = iters.size();
        iters.push_back(pos);
        ast.id = id;
    }

//    void operator()(ast::struct_declaration& ast, Iterator pos) const
//    {
//        int id = iters.size();
//        iters.push_back(pos);
//        ast.type_name.id = id;
//    }

//    void operator()(ast::struct_instantiation& ast, Iterator pos) const
//    {
//        int id = iters.size();
//        iters.push_back(pos);
//        ast.name.id = id;
//    }
};

#endif // ANNOTATION_H

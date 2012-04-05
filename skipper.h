/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef SKIPPER_H
#define SKIPPER_H

#include <boost/spirit/include/qi.hpp>

namespace parser
{
    namespace qi = boost::spirit::qi;
    namespace encoding = boost::spirit::standard;
    typedef std::string::const_iterator Iterator;

    ///////////////////////////////////////////////////////////////////////////////
    //  The skipper grammar
    ///////////////////////////////////////////////////////////////////////////////
    struct skipper : qi::grammar<Iterator>
    {
        skipper() : skipper::base_type(start)
        {
            qi::char_type char_;
            encoding::space_type space;

            start =
                    space                               // tab/space/cr/lf
                |   "/*" >> *(char_ - "*/") >> "*/"     // C-style comments
                ;
        }

        qi::rule<Iterator> start;
    };
}

#endif // SKIPPER_H

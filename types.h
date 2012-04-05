#ifndef TYPES_H
#define TYPES_H

#include <boost/spirit/include/qi.hpp>
#include <ast.h>

namespace parser {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    struct primitive_types : qi::symbols<char,ast::Type> {
        primitive_types() {
            this->add
                    ("void",ast::Void)
                    ("int",ast::Int)
                    ("bool",ast::Bool)
                    ;
        }
    };
    typedef qi::symbols<char, int> struct_types;
}

#endif // TYPES_H

#ifndef TYPES_H
#define TYPES_H

#include <boost/spirit/include/qi.hpp>
#include <ast.h>

namespace parser {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    struct primitive_types : qi::symbols<char,int> {
        primitive_types() {
            this->add
                    ("int",1)
                    ;
        }
    };
    struct return_types : primitive_types {
        return_types() : primitive_types() {
            this->add
                    ("void",0)
                    ;
        }
    };
}

#endif // TYPES_H

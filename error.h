/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef ERROR_H
#define ERROR_H

#include <QDebug>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

typedef std::string::const_iterator Iterator;

//template <typename Iterator>
struct error_handler
{
    error_handler(Iterator& first, Iterator& last, boost::shared_ptr<QString>& error_buf)
      : first(first), last(last), error_buf(error_buf) {}

    template <typename, typename, typename>
    struct result { typedef void type; };

    template <typename Message, typename What>
    void operator()(
        Message const& message,
        What const& what,
        Iterator err_pos) const
    {
        int line;
        Iterator line_start = get_pos(err_pos, line);
        std::stringstream ss;
        if(err_pos != last)
        {
            ss << message << what << " line " << line << ':' << '\n';
            ss << get_line(line_start) << '\n';
            for(; line_start != err_pos; ++line_start)
                ss << ' ';
            ss << '^';
        }
        else
        {
            ss << "Unexpected end of input. ";
            ss << message << what << " line " << line;
        }
        error_buf.reset(new QString(ss.str().c_str()));
    }

    Iterator get_pos(Iterator err_pos, int& line) const
    {
        line = 1;
        Iterator i = first;
        Iterator line_start = first;
        int count = 0;
        while (i != err_pos)
        {
            bool eol = false;
            if(i != err_pos && *i == '\r') // CR
            {
                eol = true;
                line_start = ++i;
            }
            if(i != err_pos && *i == '\n') // LF
            {
                eol = true;
                line_start = ++i;
            }
            if(eol)
                ++line;
            else
                ++i;
            ++count;
        }
        return line_start;
    }

    std::string get_line(Iterator err_pos) const
    {
        Iterator i = err_pos;
        // position i to the next EOL
        while(i != last && (*i != '\r' && *i != '\n'))
            ++i;
        return std::string(err_pos, i);
    }

    Iterator& first;
    Iterator& last;
    std::vector<Iterator> iters;
    boost::shared_ptr<QString>& error_buf;
};
#endif // ERROR_H

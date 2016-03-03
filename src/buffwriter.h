/*
This file is part of geomandel. Mandelbrot Set infused by GeoTIFF
Copyright © 2016, 2016 Christian Rapp

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BUFFWRITER_H
#define BUFFWRITER_H

#include "global.h"

#include <string>
#include <regex>
#include <memory>

/**
 * @brief Simple interface to be able to use objects with different
 * specializations of type Regexpattern in a std::vector
 */
struct RegexpatternIface {
    RegexpatternIface(){};
    virtual ~RegexpatternIface(){};

    virtual void parse_filename(std::string &filename) = 0;
};

template <typename T>
struct Regexpattern : public RegexpatternIface {
    Regexpattern(T value, std::string pattern)
        : RegexpatternIface(), value(value), regpattern(std::move(pattern)){};
    virtual ~Regexpattern(){};

    /**
     * @brief Parse the filename string and replace every occurence of regpattern 
     * with value
     *
     * @param filename
     */
    void parse_filename(std::string &filename)
    {
        std::regex re(this->regpattern);
        filename = std::regex_replace(filename, re, std::to_string(this->value));
    }

private:
    T value;
    std::string regpattern;
};

/**
 * @brief Base class of all classes that write the mandelbrot buffer to a 
 * file/stream
 */
class Buffwriter
{
public:
    Buffwriter(const constants::mandelbuff &buff);
    virtual ~Buffwriter();

    virtual void write_buffer() = 0;

protected:
    const constants::mandelbuff &buff;

    std::string out_file_name(const std::string &string_pattern,
                              unsigned int bailout, unsigned int xrange,
                              unsigned int yrange, unsigned int zoom,
                              unsigned int cores, unsigned int xcoord,
                              unsigned int ycoord, double z_real_min,
                              double z_real_max, double z_ima_min,
                              double z_ima_max);
};

#endif /* ifndef BUFFWRITER_H */

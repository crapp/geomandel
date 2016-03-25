/*
This file is part of geomandel. Mandelbrot Set infused by GeoTIFF
Copyright © 2015, 2016 Christian Rapp

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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>

#include "cxxopts.hpp"

/**
 * @brief namespace for constants, typedefs and structs
 */
namespace constants
{
enum OUT_FORMAT {
    IMAGE_PNM_BW,
    IMAGE_PNM_GREY,
    IMAGE_PNM_COL,
    IMAGE_PNG,
    IMAGE_JPG,
    GEOTIFF
};

enum FRACTAL { MANDELBROT, TRICORN };
// TODO: Underscore 2 might not be the best name for this :/
enum COL_ALGO { ESCAPE_TIME, ESCAPE_TIME_2, CONTINUOUS };

const std::map<OUT_FORMAT, std::vector<std::string>> BITMAP_DEFS{
    {OUT_FORMAT::IMAGE_PNM_BW, {"pbm", "P1"}},
    {OUT_FORMAT::IMAGE_PNM_GREY, {"pgm", "P2"}},
    {OUT_FORMAT::IMAGE_PNM_COL, {"ppm", "P3"}},
    {OUT_FORMAT::IMAGE_PNG, {"png"}},
    {OUT_FORMAT::IMAGE_JPG, {"jpg"}}};

struct Iterations {
    unsigned int default_index;
    double continous_index;

    Iterations()
    {
        this->default_index = 0;
        this->continous_index = 0;
    }

    friend std::ostream &operator<<(std::ostream &out, const Iterations &it)
    {
        out << "(" << it.default_index << ", " << it.continous_index << ")";
        return out;
    }
};

typedef std::vector<std::vector<Iterations>> fracbuff;
}

namespace utility
{
/**
 * @brief Simple split function
 *
 * @param s Input string
 * @param delim Delimeter to split on
 * @param elems Vector to put the tokens
 *
 * @note Does not skip empty tokens
 */
inline void split(const std::string &s, char delim,
                  std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}
}
#endif /* ifndef GLOBAL_H */

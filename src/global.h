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

/**
 * @brief namespace for constants, typedefs and structs
 */
namespace constants
{
enum OUT_FORMAT { IMAGE_BW, IMAGE_GREY, IMAGE_COL, GEOTIFF };
enum COL_ALGO { ESCAPE_TIME, CONTINUOUS };

const std::map<OUT_FORMAT, std::vector<std::string>> BITMAP_DEFS{
    {OUT_FORMAT::IMAGE_BW, {"pbm", "P1"}},
    {OUT_FORMAT::IMAGE_GREY, {"pgm", "P2"}},
    {OUT_FORMAT::IMAGE_COL, {"ppm", "P3"}}};

struct Iterations {
    int default_index;
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

typedef std::vector<std::vector<Iterations>> mandelbuff;
}

#endif /* ifndef GLOBAL_H */

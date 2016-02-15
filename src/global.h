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

inline void configure_command_line_parser(cxxopts::Options &p)
{
    // clang-format off
    p.add_options()
        ("help", "Show this help")
        ("m,multi", "Use multiple cores",
         cxxopts::value<int>()->implicit_value("1"));

    p.add_options("Mandelbrot")
        ("b,bailout", "Bailout value for the mandelbrot set algorithm",
         cxxopts::value<int>()->default_value("1000"))
        ("creal-min", "Real part minimum",
         cxxopts::value<double>()->default_value("-2.5"))
        ("creal-max", "Real part maximum",
         cxxopts::value<double>()->default_value("1.0"))
        ("cima-min", "Imaginary part minimum",
         cxxopts::value<double>()->default_value("-1.5"))
        ("cima-max", "Imaginary part maximum",
         cxxopts::value<double>()->default_value("1.5"));

    p.add_options("Image")
        ("image-file", "Image file base name",
         cxxopts::value<std::string>()->default_value("geomandel"))
        ("w,width", "Image width", cxxopts::value<int>()->default_value("1000"))
        ("h,height", "Image height",
         cxxopts::value<int>()->default_value("1000"))
        ("img-bandw", "Write Buffer to B&W Bitmap")
        ("img-greyscale", "Write Buffer to Greyscale Bitmap")
        ("img-color", "Write Buffer to RGB Bitmap")
        ("colalgo", "Coloring algorithm 0->Escape Time, 1->Continuous Coloring",
         cxxopts::value<int>()->default_value("0"))
        ("grey-base", "Base grey color between 0 - 255",
         cxxopts::value<int>()->default_value("55"))
        ("grey-freq", "Frequency for grey shade computation",
         cxxopts::value<int>()->default_value("5"))
        ("rgb-base", "Base RGB color as comma separated string",
         cxxopts::value<std::string>()->default_value("255,0,0"))
        ("rgb-freq", "Frequency for RGB computation as comma separated string. You may use doubles but no negative values",
         cxxopts::value<std::string>()->default_value("0,16,16"))
        ("rgb-phase", "Phase for RGB computation as comma separated string",
         cxxopts::value<std::string>()->default_value("0,2,4"))
        ("zoom", "Zoom level. Use together with xcoord, ycoord",
         cxxopts::value<int>())
        ("xcoord", "Image X coordinate where you want to zoom into the fractal",
         cxxopts::value<int>())
        ("ycoord", "Image Y coordinate where you want to zoom into the fractal",
         cxxopts::value<int>());

    p.add_options("Export")
        ("p,print", "Print Buffer to terminal")
        ("csv", "Export data to csv files");
    // clang-format on
}
}
#endif /* ifndef GLOBAL_H */

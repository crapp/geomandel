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

#include "buffwriter.h"

Buffwriter::Buffwriter(const constants::mandelbuff &buff) : buff(buff) {}
Buffwriter::~Buffwriter() {}
std::string Buffwriter::out_file_name(const std::string &base_string,
                                      unsigned int bailout, unsigned int xrange,
                                      unsigned int yrange, unsigned int zoom,
                                      unsigned int cores,
                                      constants::COL_ALGO col_algo)
{
    std::stringstream ss;
    ss << base_string << "_" << bailout << "_" << xrange << "x" << yrange << "_"
       << zoom << "x"
       << "_" << cores << "c"
       << "_" << col_algo;
    return ss.str();
}

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

#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include <fstream>
#include <string>
#include <cmath>
#include <tuple>

#include "buffwriter.h"
#include "global.h"

class Imagewriter : public Buffwriter
{
public:
    Imagewriter(const constants::mandelbuff &buff,
                const constants::OUT_FORMAT format,
                const constants::COL_ALGO col_algo, const int maxiter,
                std::tuple<int, int, int> rgb_base,
                std::tuple<int, int, int> rgb_freq);
    virtual ~Imagewriter();

    void write_buffer();

private:
    /* data */
    const constants::OUT_FORMAT format;
    const constants::COL_ALGO col_algo;
    const int maxiter;
    std::tuple<int, int, int> rgb_base;
    std::tuple<int, int, int> rgb_freq;

    /**
     * @brief Map iteration count on RGB colors in a inear fashion
     *
     * @param its Number of iterations
     * @param rgb_base The RGB base color
     * @param rgb_freq The RGB frequency
     *
     * @return RGB values
     *
     * @details
     * This function uses the Escape Time Algorithm to compute RGB colors. This
     * leads to visible color bands but is fast and easy to implement. The
     * method is also used by the greyscale Bitmap code. Only the first element
     * (red) of the RGB tuple is used in this case.
     */
    std::tuple<int, int, int> rgb_linear(
        int its, const std::tuple<int, int, int> &rgb_base,
        const std::tuple<int, int, int> &rgb_freq);
};

#endif /* ifndef IMAGEWRITER_H */

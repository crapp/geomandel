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

#include <string>
#include <cmath>
#include <tuple>

#include "buffwriter.h"
#include "global.h"
#include "mandelparams.h"

class Imagewriter : public Buffwriter
{
public:
    Imagewriter(const constants::mandelbuff &buff,
                const std::shared_ptr<MandelParameters> &params);
    virtual ~Imagewriter();

    virtual void write_buffer() = 0;

protected:
    const std::shared_ptr<MandelParameters> &params;

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
        unsigned int its, const std::tuple<int, int, int> &rgb_base,
        const std::tuple<double, double, double> &rgb_freq);

    /**
     * @brief Map contiuous iteration count on rgb colors using sine waves
     *
     * @param its Continous iteration count
     * @param rgb_base RGB base color
     * @param rgb_freq Frequency determining how fast color changes
     * @param rgb_phase For out of phase waves
     *
     * @return RGB tuple
     *
     * @details
     *
     * Based on the concepts of [Jim Bumgardner](http://krazydad.com/tutorials/makecolors.php)
     */
    std::tuple<int, int, int> rgb_continuous(
        double its, const std::tuple<int, int, int> &rgb_base,
        const std::tuple<double, double, double> &rgb_freq,
        const std::tuple<int, int, int> &rgb_phase);

private:
    /* data */
};

#endif /* ifndef IMAGEWRITER_H */

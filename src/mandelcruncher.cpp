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

#include "mandelcruncher.h"

Mandelcruncher::Mandelcruncher(constants::fracbuff &buff,
                               const std::shared_ptr<FractalParameters> &params)
    : buff(buff), params(params)
{
}
Mandelcruncher::~Mandelcruncher() {}
std::tuple<unsigned int, double, double> Mandelcruncher::crunch_mandel_complex(
    double x, double y, unsigned int bailout) const
{
    // The Mandelbrot Set algorithm derived from pseudo code
    unsigned int iterations = 0;
    double x0 = x;
    double y0 = y;
    // std::cout << "zO(" << x << ", " << y << ")" << std::endl;
    while (x * x + y * y <= 4.0 && iterations < bailout) {
        double x_old = x;
        x = x * x - y * y + x0;
        y = 2 * x_old * y + y0;
        iterations++;
    }
    // std::cout << "Iter: " << iterations << " zE(" << x << ", " << y << ")"
    //           << std::endl;
    return std::make_tuple(iterations, x, y);
}

constants::Iterations Mandelcruncher::iterations_factory(unsigned int its,
                                                         double Zx,
                                                         double Zy) const
{
    constants::Iterations it;
    if (this->params->col_algo == constants::COL_ALGO::ESCAPE_TIME ||
        this->params->col_algo == constants::COL_ALGO::ESCAPE_TIME_2)
        it.default_index = its;
    if (this->params->col_algo == constants::COL_ALGO::CONTINUOUS) {
        it.default_index = its;
        double cont_index =
            its + 1 -
            (std::log(2) / std::sqrt(Zx * Zx + Zy * Zy)) / std::log(2.0);

        // its - (std::log(std::log(std::sqrt(Zx * Zx + Zy * Zy)))) / std::log(2.0);
        it.continous_index = cont_index;
    }
    return it;
}

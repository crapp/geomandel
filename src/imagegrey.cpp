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

#include "imagegrey.h"

Imagegrey::Imagegrey(const constants::mandelbuff &buff,
                     const std::shared_ptr<MandelParameters> &params,
                     std::tuple<int, int, int> rgb_base,
                     std::tuple<double, double, double> rgb_freq)
    : Imagewriter(buff, params, constants::OUT_FORMAT::IMAGE_GREY),
      rgb_base(std::move(rgb_base)),
      rgb_freq(std::move(rgb_freq))
{
}

Imagegrey::~Imagegrey() {}
void Imagegrey::out_format_write(std::ofstream &img,
                                 const constants::Iterations &data)
{
    unsigned int its = data.default_index;
    if (this->params->col_algo == constants::COL_ALGO::ESCAPE_TIME) {
        // Escape time algorithm coloring
        // not very efficient to do some of the math over
        // and over again. Hopefully the compiler will
        // optimize this ;)
        if (its == this->params->bailout) {
            img << 0 << " ";
        } else {
            std::tuple<int, int, int> rgb = this->rgb_linear(
                its, std::make_tuple(55, 0, 0), std::make_tuple(5, 0, 0));
            img << std::get<0>(rgb) << " ";
        }
    }
    if (this->params->col_algo == constants::COL_ALGO::CONTINUOUS) {
        if (its < this->params->bailout) {
            double continuous_index = data.continous_index;
            // TODO: Clang format is producing some weird code formatting here.
            img << static_cast<int>(std::floor(std::abs(
                       std::sin(0.016 * continuous_index + 4) * 230 + 25)))
                << " ";
        } else {
            img << 0 << " ";
        }
    }
}

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

#include "imagecol.h"

Imagecol::Imagecol(const constants::mandelbuff &buff,
                   const constants::COL_ALGO col_algo, const int maxiter,
                   const constants::OUT_FORMAT format,
                   std::tuple<int, int, int> rgb_base,
                   std::tuple<int, int, int> rgb_freq,
                   std::tuple<int, int, int> rgb_phase)
    : Imagewriter(buff, col_algo, maxiter, format),
      rgb_base(std::move(rgb_base)),
      rgb_freq(std::move(rgb_freq)),
      rgb_phase(std::move(rgb_phase))
{
}

Imagecol::~Imagecol() {}
void Imagecol::out_format_write(std::ofstream &img,
                                const constants::Iterations &data)
{
    int its = data.default_index;
    if (col_algo == constants::COL_ALGO::ESCAPE_TIME) {
        if (its == maxiter) {
            img << "0 0 0"
                << "\t";
        } else {
            std::tuple<int, int, int> rgb =
                this->rgb_linear(its, this->rgb_base, this->rgb_freq);
            //(green + ((its % 16) * 16))

            img << std::get<0>(rgb) << " " << std::get<1>(rgb) << " "
                << std::get<2>(rgb) << "\t";
        }
    }
}

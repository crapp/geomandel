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

#include "image_pnm_col.h"

Imagecol::Imagecol(const constants::mandelbuff &buff,
                   const std::shared_ptr<MandelParameters> &params,
                   std::tuple<int, int, int> rgb_base,
                   std::tuple<double, double, double> rgb_freq,
                   std::tuple<int, int, int> rgb_phase)
    : ImagePNM(buff, params, constants::OUT_FORMAT::IMAGE_PNM_COL),
      rgb_base(std::move(rgb_base)),
      rgb_freq(std::move(rgb_freq)),
      rgb_phase(std::move(rgb_phase))
{
}

Imagecol::~Imagecol() {}
void Imagecol::out_format_write(std::stringstream &img_buf,
                                const constants::Iterations &data)
{
    unsigned int its = data.default_index;
    double continous_index = data.continous_index;
    if (its == this->params->bailout) {
        img_buf << "0 0 0"
                << "\t";
        return;
    }
    if (this->params->col_algo == constants::COL_ALGO::ESCAPE_TIME) {
        auto rgb = this->rgb_linear(its, this->rgb_base, this->rgb_freq);
        //(green + ((its % 16) * 16))

        img_buf << std::get<0>(rgb) << " " << std::get<1>(rgb) << " "
                << std::get<2>(rgb) << "\t";
    }
    if (this->params->col_algo == constants::COL_ALGO::CONTINUOUS) {
        auto rgb = this->rgb_continuous(continous_index, this->rgb_base,
                                        this->rgb_freq, this->rgb_phase);

        img_buf << std::get<0>(rgb) << " " << std::get<1>(rgb) << " "
                << std::get<2>(rgb) << "\t";
    }
}

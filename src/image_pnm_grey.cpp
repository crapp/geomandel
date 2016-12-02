/*
This file is part of geomandel. An artful fractal generator
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

#include "image_pnm_grey.h"

Imagegrey::Imagegrey(const constants::fracbuff &buff,
                     const std::shared_ptr<FractalParameters> &params,
                     const std::shared_ptr<Printer> &prnt,
                     std::tuple<int, int, int> &&rgb_base,
                     std::tuple<double, double, double> &&rgb_freq)

    : ImagePNM(buff, params, prnt, constants::OUT_FORMAT::IMAGE_PNM_GREY),
      rgb_base(std::move(rgb_base)),
      rgb_freq(std::move(rgb_freq))
{
    this->rgb_phase = std::make_tuple(0, 0, 0);
}

Imagegrey::~Imagegrey() {}
void Imagegrey::out_format_write(std::stringstream &img_buf,
                                 const constants::Iterations &data)
{
    unsigned int its = data.default_index;
    if (its == this->params->bailout) {
        img_buf << 0 << " ";
        return;
    }

    std::tuple<int, int, int> rgb{0, 0, 0};
    if (this->params->col_algo == constants::COL_ALGO::ESCAPE_TIME) {
        rgb = this->rgb_linear(its, this->rgb_base, this->rgb_freq);
    }
    if (this->params->col_algo == constants::COL_ALGO::CONTINUOUS_SINE) {
        double continuous_index = data.continous_index;
        rgb = this->rgb_continuous_sine(continuous_index, this->rgb_base,
                                        this->rgb_freq, this->rgb_phase);
    }

    img_buf << std::get<0>(rgb) << " ";
}

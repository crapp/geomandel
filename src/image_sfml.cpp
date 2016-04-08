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

#include "image_sfml.h"

ImageSFML::ImageSFML(const constants::fracbuff &buff,
                     const std::shared_ptr<FractalParameters> &params,
                     const std::shared_ptr<Printer> &prnt,
                     std::tuple<int, int, int> rgb_base,
                     std::tuple<int, int, int> rgb_set_base,
                     std::tuple<double, double, double> rgb_freq,
                     std::tuple<int, int, int> rgb_phase,
                     std::tuple<double, double, double> rgb_amp, uint8_t outfmt)
    : Imagewriter(buff, params, prnt),
      rgb_base(std::move(rgb_base)),
      rgb_set_base(std::move(rgb_set_base)),
      rgb_freq(std::move(rgb_freq)),
      rgb_phase(std::move(rgb_phase)),
      rgb_amp(std::move(rgb_amp)),
      outfmt(std::move(outfmt))
{
}

ImageSFML::~ImageSFML() {}
void ImageSFML::write_buffer()
{
    std::vector<uint8_t> sfml_img_buf;
    // reserve memory this will make push_back less costly
    sfml_img_buf.reserve(this->params->xrange * this->params->yrange);
    // SFML needs a data structure with 4 uint8_t per RGBA pixel
    for (const auto &v : this->buff) {
        for (const auto &it : v) {
            // TODO: This code is quite similar to the one used in the ppm class
            unsigned int its = it.default_index;
            double continous_index = it.continous_index;
            if (its == this->params->bailout) {
                sfml_img_buf.push_back(std::get<0>(this->rgb_set_base));
                sfml_img_buf.push_back(std::get<1>(this->rgb_set_base));
                sfml_img_buf.push_back(std::get<2>(this->rgb_set_base));
                sfml_img_buf.push_back(255);
                continue;
            }

            auto rgb = std::make_tuple(0, 0, 0);

            if (this->params->col_algo == constants::COL_ALGO::ESCAPE_TIME) {
                rgb = this->rgb_linear(its, this->rgb_base, this->rgb_freq);
            }
            if (this->params->col_algo == constants::COL_ALGO::CONTINUOUS_SINE) {
                rgb = this->rgb_continuous_sine(continous_index, this->rgb_base,
                                                this->rgb_freq, this->rgb_phase);
            }
            if (this->params->col_algo == constants::COL_ALGO::CONTINUOUS_BERN) {
                rgb = this->rgb_continuous_bernstein(
                    its, this->params->bailout, this->rgb_base, this->rgb_amp);
            }

            sfml_img_buf.push_back(static_cast<uint8_t>(std::get<0>(rgb)));
            sfml_img_buf.push_back(static_cast<uint8_t>(std::get<1>(rgb)));
            sfml_img_buf.push_back(static_cast<uint8_t>(std::get<2>(rgb)));
            sfml_img_buf.push_back(255);
        }
    }

    std::string filename = this->out_file_name(
        this->params->image_base, this->params->fractal_type,
        this->params->bailout, this->params->xrange, this->params->yrange,
        this->params->zoom, this->params->cores, this->params->xcoord,
        this->params->ycoord, this->params->xl, this->params->xh,
        this->params->yl, this->params->yh);

    sf::Image img;
    img.create(this->params->xrange, this->params->yrange, sfml_img_buf.data());
    if ((outfmt & static_cast<uint8_t>(constants::OUT_FORMAT::IMAGE_JPG)) ==
        static_cast<uint8_t>(constants::OUT_FORMAT::IMAGE_JPG)) {
        this->prnt << "+ \u2937 " + filename + ".jpg" << std::endl;
        img.saveToFile(filename + ".jpg");
    }
    if ((outfmt & static_cast<uint8_t>(constants::OUT_FORMAT::IMAGE_PNG)) ==
        static_cast<uint8_t>(constants::OUT_FORMAT::IMAGE_PNG)) {
        this->prnt << "+ \u2937 " + filename + ".png" << std::endl;
        img.saveToFile(filename + ".png");
    }
}

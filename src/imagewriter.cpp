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

#include "imagewriter.h"

Imagewriter::Imagewriter(const constants::mandelbuff &buff,
                         const constants::COL_ALGO col_algo,
                         const MandelParameters &params,
                         const constants::OUT_FORMAT format)
    : Buffwriter(buff), col_algo(col_algo), params(params), format(format)
{
}

Imagewriter::~Imagewriter() {}
void Imagewriter::write_buffer()
{
    // This will overwrite any existing image. Image is written into the
    // directory from where the application was called.
    // File name: BAILOUT_WIDTHxHEIGHT_ZOOMx.FILE_TYPE
    std::string filename = "geomandel_" + std::to_string(this->params.bailout) +
                           "_" + std::to_string(this->params.xrange) + "x" +
                           std::to_string(this->params.yrange) + "_" +
                           std::to_string(this->params.zoom) + "x" + "." +
                           constants::BITMAP_DEFS.at(this->format).at(0);
    std::ofstream img(filename, std::ofstream::out);
    img.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        if (img.is_open()) {
            // magic number for bitmap
            img << constants::BITMAP_DEFS.at(this->format).at(1) << std::endl;
            // comments
            img << "# geomandel output" << std::endl;
            // specify width and height of the bitmap
            img << this->buff.at(0).size() << " " << this->buff.size()
                << std::endl;
            if (this->format == constants::OUT_FORMAT::IMAGE_GREY ||
                this->format == constants::OUT_FORMAT::IMAGE_COL)
                img << 255 << std::endl;
            for (const auto &v : this->buff) {
                int linepos = 1;
                for (const auto &data : v) {
                    // this kind of images don't allow for more than 70
                    // characters in one row
                    // FIXME: This kind of linepos handling is borked
                    if (linepos % 70 == 0) {
                        img << std::endl;
                        linepos = 0;
                    }
                    // write data to image file stream
                    this->out_format_write(img, data);
                    linepos++;
                }
            }
        }
    } catch (const std::ifstream::failure &e) {
        std::cerr << "Error writing image file" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

std::tuple<int, int, int> Imagewriter::rgb_linear(
    int its, const std::tuple<int, int, int> &rgb_base,
    const std::tuple<double, double, double> &rgb_freq)
{
    int red_base = std::get<0>(rgb_base);
    int green_base = std::get<1>(rgb_base);
    int blue_base = std::get<2>(rgb_base);

    std::tuple<int, int, int> rgb =
        std::make_tuple(red_base, green_base, blue_base);

    double red_freq = std::get<0>(rgb_freq);
    double green_freq = std::get<1>(rgb_freq);
    double blue_freq = std::get<2>(rgb_freq);

    // map iterations on rgb colors
    // we only touch the base color if frequency is higher than 0
    // Have a look at the readme for some charts how color values change using
    // this method
    if (red_freq > 0) {
        std::get<0>(rgb) =
            red_base + (static_cast<int>((red_freq * its)) % (255 - red_base));
    }
    if (green_freq > 0) {
        std::get<1>(rgb) = green_base + (static_cast<int>((green_freq * its)) %
                                         (255 - green_base));
    }
    if (blue_freq > 0) {
        std::get<2>(rgb) = blue_base + (static_cast<int>((blue_freq * its)) %
                                        (255 - blue_base));
    }

    return rgb;
}

std::tuple<int, int, int> Imagewriter::rgb_continuous(
    double its, const std::tuple<int, int, int> &rgb_base,
    const std::tuple<double, double, double> &rgb_freq,
    const std::tuple<int, int, int> &rgb_phase)
{
    int red_base = std::get<0>(rgb_base);
    int green_base = std::get<1>(rgb_base);
    int blue_base = std::get<2>(rgb_base);

    std::tuple<int, int, int> rgb =
        std::make_tuple(red_base, green_base, blue_base);

    double red_freq = std::get<0>(rgb_freq);
    double green_freq = std::get<1>(rgb_freq);
    double blue_freq = std::get<2>(rgb_freq);

    double red_phase = std::get<0>(rgb_phase);
    double green_phase = std::get<1>(rgb_phase);
    double blue_phase = std::get<2>(rgb_phase);

    std::get<0>(rgb) = static_cast<int>(std::fabs(
        std::sin(red_freq * its + red_phase) * red_base + (255 - red_base)));
    std::get<1>(rgb) = static_cast<int>(
        std::fabs(std::sin(green_freq * its + green_phase) * green_base +
                  (255 - green_base)));
    std::get<2>(rgb) = static_cast<int>(std::fabs(
        std::sin(blue_freq * its + blue_phase) * blue_base + (255 - blue_base)));

    return rgb;
}

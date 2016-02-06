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
                         const constants::OUT_FORMAT format,
                         const constants::COL_ALGO col_algo, const int maxiter,
                         std::tuple<int, int, int> rgb_base,
                         std::tuple<int, int, int> rgb_freq)
    : Buffwriter(buff),
      format(format),
      col_algo(col_algo),
      maxiter(maxiter),
      rgb_base(std::move(rgb_base)),
      rgb_freq(std::move(rgb_freq))
{
}
Imagewriter::~Imagewriter() {}
void Imagewriter::write_buffer()
{
    // This will overwrite any existing image. Image is written into the
    // directory from where the application was called.
    std::string filename = "geomandel_" + std::to_string(maxiter) + "." +
                           constants::BITMAP_DEFS.at(format).at(0);
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
                for (auto data : v) {
                    // this kind of images don't allow for more than 70
                    // characters in one row
                    // FIXME: This kind of linepos handling is borked
                    if (linepos % 70 == 0) {
                        img << std::endl;
                        linepos = 0;
                    }
                    if (this->format == constants::OUT_FORMAT::IMAGE_BW) {
                        if (data.default_index == maxiter) {
                            img << 1 << " ";
                        } else {
                            img << 0 << " ";
                        }
                    }
                    if (this->format == constants::OUT_FORMAT::IMAGE_GREY) {
                        int its = data.default_index;
                        if (col_algo == constants::COL_ALGO::ESCAPE_TIME) {
                            // Escape time algorithm coloring
                            // not very efficient to do some of the math over
                            // and over again. Hopefully the compiler will
                            // optimize this ;)
                            if (its == maxiter) {
                                img << 0 << " ";
                            } else {
                                std::tuple<int, int, int> rgb = this->rgb_linear(
                                    its, std::make_tuple(55, 0, 0),
                                    std::make_tuple(5, 0, 0));
                                img << std::get<0>(rgb) << " ";
                            }
                        }
                        if (col_algo == constants::COL_ALGO::CONTINUOUS) {
                            if (its < maxiter) {
                                double continuous_index = data.continous_index;
                                // TODO: Clang format is producing some weird
                                // code formatting here.
                                img << static_cast<int>(std::floor(std::abs(
                                           std::sin(0.016 * continuous_index +
                                                    4) *
                                               230 +
                                           25)))
                                    << " ";
                            } else {
                                img << 0 << " ";
                            }
                        }
                    }
                    if (this->format == constants::OUT_FORMAT::IMAGE_COL) {
                        int its = data.default_index;
                        if (col_algo == constants::COL_ALGO::ESCAPE_TIME) {
                            if (its == maxiter) {
                                img << "0 0 0"
                                    << "\t";
                            } else {
                                std::tuple<int, int, int> rgb = this->rgb_linear(
                                    its, this->rgb_base, this->rgb_freq);
                                //(green + ((its % 16) * 16))

                                img << std::get<0>(rgb) << " "
                                    << std::get<1>(rgb) << " "
                                    << std::get<2>(rgb) << "\t";
                            }
                        }
                    }
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
    const std::tuple<int, int, int> &rgb_freq)
{
    int red_base = std::get<0>(rgb_base);
    int green_base = std::get<1>(rgb_base);
    int blue_base = std::get<2>(rgb_base);

    std::tuple<int, int, int> rgb =
        std::make_tuple(red_base, green_base, blue_base);

    int red_freq = std::get<0>(rgb_freq);
    int green_freq = std::get<1>(rgb_freq);
    int blue_freq = std::get<2>(rgb_freq);

    // map iterations on rgb colors
    if (red_freq > 0) {
        std::get<0>(rgb) = red_base + ((red_freq * its) % (255 - red_base));
    }
    if (green_freq > 0) {
        std::get<1>(rgb) =
            green_base + ((green_freq * its) % (255 - green_base));
    }
    if (blue_freq > 0) {
        std::get<2>(rgb) = blue_base + ((blue_freq * its) % (255 - blue_base));
    }

    return rgb;
}

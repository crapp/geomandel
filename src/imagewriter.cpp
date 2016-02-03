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
                         const constants::COL_ALGO col_algo, const int maxiter)
    : Buffwriter(buff), format(format), col_algo(col_algo), maxiter(maxiter)
{
}
Imagewriter::~Imagewriter() {}
void Imagewriter::write_buffer()
{
    // This will overwrite any existing image. Image is written into the
    // directory from where the application was called.
    std::string filename =
        "geomandel." + constants::BITMAP_DEFS.at(format).at(0);
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
            if (this->format == constants::OUT_FORMAT::IMAGE_GREY)
                img << 255 << std::endl;
            for (const auto &v : this->buff) {
                int linepos = 1;
                for (auto data : v) {
                    // this kind of images don't allow for more than 70
                    // characters in one row
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
                            if (its < maxiter / 2) {
                                img << 127 + (floor(its / (maxiter / 127.0)))
                                    << " ";
                            } else if (its >= maxiter / 2 && its != maxiter) {
                                img << floor(its / (maxiter / 127.0)) << " ";
                            } else if (its == maxiter) {
                                img << 0 << " ";
                            }
                        }
                        if (col_algo == constants::COL_ALGO::CONTINUOUS) {
                            if (its < maxiter) {
                                double continuous_index = data.continous_index;
                                img << std::sin(0.016 * continuous_index + 4) *
                                               230 +
                                           25
                                    << " ";
                            } else {
                                img << 0 << " ";
                            }
                        }
                    }
                    if (this->format == constants::OUT_FORMAT::IMAGE_COL) {
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

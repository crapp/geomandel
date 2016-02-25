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

#include "image_pnm.h"

ImagePNM::ImagePNM(const constants::mandelbuff &buff,
                   const std::shared_ptr<MandelParameters> &params,
                   const constants::OUT_FORMAT format)
    : Imagewriter(buff, params), format(format){};
ImagePNM::~ImagePNM(){};

void ImagePNM::write_buffer()
{
    // This will overwrite any existing image. Image is written into the
    // directory from where the application was called.
    // TODO: Maybe add coordinates for zoom
    // FIXME: Add zoom value to filename only when greater 0
    // File name: FILENAME_BAILOUT_WIDTHxHEIGHT_ZOOMx.FILE_TYPE
    std::string filename =
        this->out_file_name(this->params->image_base, this->params->bailout,
                            this->params->xrange, this->params->yrange,
                            this->params->zoom, this->params->cores,
                            this->params->col_algo) +
        "." + constants::BITMAP_DEFS.at(this->format).at(0);
    std::cout << "+ \u2937 " + filename << std::endl;

    std::ofstream img(filename, std::ofstream::out);
    img.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        if (img.is_open()) {
            // magic number for bitmap
            img << constants::BITMAP_DEFS.at(this->format).at(1) << std::endl;
            // comments
            img << "# Created with geomandel https://git.io/vgXRW" << std::endl;
            // specify width and height of the bitmap
            img << this->buff.at(0).size() << " " << this->buff.size()
                << std::endl;
            if (this->format == constants::OUT_FORMAT::IMAGE_PNM_GREY ||
                this->format == constants::OUT_FORMAT::IMAGE_PNM_COL)
                img << 255 << std::endl;
            for (const auto &v : this->buff) {
                int linepos = 1;
                for (const auto &data : v) {
                    std::stringstream img_buf;
                    // this kind of images don't allow for more than 70
                    // characters in one row
                    // FIXME: This kind of linepos handling is borked
                    if (linepos % 70 == 0) {
                        img_buf << std::endl;
                        linepos = 0;
                    }
                    // write data to image file stream
                    this->out_format_write(img_buf, data);
                    img << img_buf.rdbuf();
                    linepos++;
                }
            }
        }
    } catch (const std::ifstream::failure &e) {
        std::cerr << "Error writing image file" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

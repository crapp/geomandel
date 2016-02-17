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

#include "imagebw.h"

ImageBW::ImageBW(const constants::mandelbuff &buff,
                 const std::shared_ptr<MandelParameters> &params)
    : Imagewriter(buff, params, constants::OUT_FORMAT::IMAGE_BW)
{
}

ImageBW::~ImageBW() {}
void ImageBW::out_format_write(std::ofstream &img,
                               const constants::Iterations &data)
{
    if (data.default_index == this->params->bailout) {
        img << 1 << " ";
    } else {
        img << 0 << " ";
    }
}

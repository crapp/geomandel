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

#include "image_pnm_bw.h"

ImageBW::ImageBW(const constants::fracbuff &buff,
                 const std::shared_ptr<FractalParameters> &params,
                 const std::shared_ptr<Printer> &prnt)
    : ImagePNM(buff, params, prnt, constants::OUT_FORMAT::IMAGE_PNM_BW)
{
}

ImageBW::~ImageBW() {}
void ImageBW::out_format_write(std::stringstream &img_buf,
                               const constants::Iterations &data)
{
    if (data.default_index == this->params->bailout) {
        img_buf << 1 << " ";
    } else {
        img_buf << 0 << " ";
    }
}

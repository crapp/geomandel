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

#ifndef IMAGE_SFML_H
#define IMAGE_SFML_H

#include <SFML/Graphics/Image.hpp>

#include "imagewriter.h"

class ImageSFML : public Imagewriter
{
public:
    ImageSFML(const constants::mandelbuff &buff,
              const std::shared_ptr<MandelParameters> &params,
              std::tuple<int, int, int> rgb_base,
              std::tuple<double, double, double> rgb_freq,
              std::tuple<int, int, int> rgb_phase, uint8_t outfmt);
    virtual ~ImageSFML();

    void write_buffer();

private:
    /* data */
    std::tuple<int, int, int> rgb_base;
    std::tuple<double, double, double> rgb_freq;
    std::tuple<int, int, int> rgb_phase;
    uint8_t outfmt;
};

#endif /* ifndef IMAGE_SFML_H */

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

#include "imagewriter.h"

Imagewriter::Imagewriter(const constants::fracbuff &buff,
                         const std::shared_ptr<FractalParameters> &params,
                         const std::shared_ptr<Printer> &prnt)
    : Buffwriter(buff), params(params), prnt(prnt)
{
}

Imagewriter::~Imagewriter() {}
std::tuple<int, int, int> Imagewriter::rgb_linear(
    unsigned int its, const std::tuple<int, int, int> &rgb_base,
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
    if (red_freq > 0 && 255 - red_base > 0) {
        std::get<0>(rgb) =
            red_base + (static_cast<int>((red_freq * its)) % (255 - red_base));
    }
    if (green_freq > 0 && 255 - green_base > 0) {
        std::get<1>(rgb) = green_base + (static_cast<int>((green_freq * its)) %
                                         (255 - green_base));
    }
    if (blue_freq > 0 && 255 - blue_base > 0) {
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

    int red_phase = std::get<0>(rgb_phase);
    int green_phase = std::get<1>(rgb_phase);
    int blue_phase = std::get<2>(rgb_phase);

    if (red_freq > 0) {
        std::get<0>(rgb) = static_cast<int>(std::fabs(
            std::sin(red_freq * its + red_phase) * (255 - red_base) + red_base));
    }
    if (green_freq > 0) {
        std::get<1>(rgb) = static_cast<int>(std::fabs(
            std::sin(green_freq * its + green_phase) * (255 - green_base) +
            green_base));
    }
    if (blue_freq > 0) {
        std::get<2>(rgb) = static_cast<int>(std::fabs(
            std::sin(blue_freq * its + blue_phase) * (255 - blue_base) +
            blue_base));
    }
    // std::cout << std::get<0>(rgb) << "," << std::get<1>(rgb) << ","
    //<< std::get<2>(rgb) << std::endl;
    return rgb;
}

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

#include <memory>

#include "cxxopts.hpp"

#include "mandelparams.h"
#include "mandelzoom.h"

inline void init_mandel_parameters(std::shared_ptr<MandelParameters> params,
                                   const cxxopts::Options &parser)
{
    try {
        int bailout = parser["b"].as<int>();

        // define complex plane variables
        double xl = parser["creal-min"].as<double>();
        double xh = parser["creal-max"].as<double>();
        int xrange = parser["w"].as<int>();

        double yl = parser["cima-min"].as<double>();
        double yh = parser["cima-max"].as<double>();
        int yrange = parser["h"].as<int>();

        int zoomlvl = 0;

        // check if user wants to zoom
        if (parser.count("zoom")) {
            if (!parser.count("xcoord") || !parser.count("ycoord")) {
                std::cerr << "Please provide x/y coordinates to zoom"
                          << std::endl;
                exit(1);
            }
            // get zoom parameters and coordinate
            zoomlvl = parser["zoom"].as<int>();
            int xcoord = parser["xcoord"].as<int>();
            int ycoord = parser["ycoord"].as<int>();
            Mandelzoom zoomer;
            // calculate new complex plane
            zoomer.calcalute_zoom_cpane(xh, xl, yh, yl, zoomlvl, xcoord, ycoord,
                                        xrange, yrange);
        }

        // Stores informations used by the mandel cruncher and some data writer
        // classes
        params = std::make_shared<MandelParameters>(
            xrange, xl, xh, yrange, yl, yh, bailout, zoomlvl,
            parser["image-file"].as<std::string>());
    } catch (const cxxopts::missing_argument_exception &ex) {
        std::cerr << "Missing argument " << ex.what() << std::endl;
    } catch (const cxxopts::OptionParseException &ex) {
        std::cerr << "Can not parse option value " << ex.what() << std::endl;
    }
}

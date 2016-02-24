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

#include "config.h"

#include "mandelparams.h"
#include "mandelzoom.h"

// TODO: params Argument could be a reference to avoid reference counting
inline void init_mandel_parameters(std::shared_ptr<MandelParameters> &params,
                                   const cxxopts::Options &parser)
{
    // TODO: Thiy try catch block could be unnecessary as cxxopts does most of
    // the checking itself when parse is called
    try {
        unsigned int bailout = parser["b"].as<unsigned int>();

        // define complex plane variables
        double xl = parser["creal-min"].as<double>();
        double xh = parser["creal-max"].as<double>();
        if (xh < xl) {
            std::cerr << "Real part maximum can not be lower than minimum"
                      << std::endl;
            return;
        }
        unsigned int xrange = parser["w"].as<unsigned int>();

        double yl = parser["cima-min"].as<double>();
        double yh = parser["cima-max"].as<double>();
        if (yh < yl) {
            std::cerr << "Imaginary part maximum can not be lower than minimum"
                      << std::endl;
            return;
        }
        unsigned int yrange = parser["h"].as<unsigned int>();

        unsigned int zoomlvl = 0;

        // check if user wants to zoom
        if (parser.count("zoom")) {
            if (!parser.count("xcoord") || !parser.count("ycoord")) {
                std::cerr << "Please provide x/y coordinates to zoom"
                          << std::endl;
                return;
            }
            // get zoom parameters and coordinate
            zoomlvl = parser["zoom"].as<unsigned int>();
            unsigned int xcoord = parser["xcoord"].as<unsigned int>();
            unsigned int ycoord = parser["ycoord"].as<unsigned int>();

            if (xcoord > xrange) {
                std::cerr << "X Coordinate outside of the image space"
                          << std::endl;
                return;
            }
            if (ycoord > yrange) {
                std::cerr << "Y Coordinate outside of the image space"
                          << std::endl;
                return;
            }

            Mandelzoom zoomer;
            // calculate new complex plane
            zoomer.calcalute_zoom_cpane(xh, xl, yh, yl, zoomlvl, xcoord, ycoord,
                                        xrange, yrange);
        }

        unsigned int cores = 0;
        if (parser.count("m"))
            cores = parser["m"].as<unsigned int>();

        constants::COL_ALGO col_algo = static_cast<constants::COL_ALGO>(
            parser["colalgo"].as<unsigned int>());

        // Stores informations used by the mandel cruncher and some data
        // writer classes
        params = std::make_shared<MandelParameters>(
            xrange, xl, xh, yrange, yl, yh, bailout, zoomlvl,
            parser["image-file"].as<std::string>(), cores, col_algo);
    } catch (const cxxopts::missing_argument_exception &ex) {
        std::cerr << "Missing argument \n  " << ex.what() << std::endl;
    } catch (const cxxopts::OptionParseException &ex) {
        std::cerr << "Can not parse option value \n  " << ex.what() << std::endl;
    } catch (const std::exception &ex) {
        // last ressort catch
        std::cerr << "Exception occured \n  " << ex.what() << std::endl;
    }
}

inline void configure_command_line_parser(cxxopts::Options &p)
{
    // clang-format off
    p.add_options()
        ("help", "Show this help")
        ("m,multi", "Use multiple cores",
         cxxopts::value<unsigned int>()->implicit_value("2"));

    p.add_options("Mandelbrot")
        ("b,bailout", "Bailout value for the mandelbrot set algorithm",
         cxxopts::value<unsigned int>()->default_value("1000"))
        ("creal-min", "Real part minimum",
         cxxopts::value<double>()->default_value("-2.5"))
        ("creal-max", "Real part maximum",
         cxxopts::value<double>()->default_value("1.0"))
        ("cima-min", "Imaginary part minimum",
         cxxopts::value<double>()->default_value("-1.5"))
        ("cima-max", "Imaginary part maximum",
         cxxopts::value<double>()->default_value("1.5"));

    p.add_options("Image")
        ("image-file", "Image file base name",
         cxxopts::value<std::string>()->default_value("geomandel"))
        ("w,width", "Image width",
         cxxopts::value<unsigned int>()->default_value("1000"))
        ("h,height", "Image height",
         cxxopts::value<unsigned int>()->default_value("1000"))
        ("img-pnm-bw", "Write Buffer to PBM Bitmap")
        ("img-pnm-grey", "Write Buffer to grey scale PGM")
        ("img-pnm-col", "Write Buffer to PPM Bitmap")
#ifdef HAVE_SFML
        ("img-jpg", "Write Buffer to JPG image")
        ("img-png", "Write Buffer to PNG image")
#endif
        ("colalgo", "Coloring algorithm 0->Escape Time, 1->Continuous Coloring",
         cxxopts::value<unsigned int>()->default_value("0"))
        ("grey-base", "Base grey color between 0 - 255",
         cxxopts::value<int>()->default_value("55"))
        ("grey-freq", "Frequency for grey shade computation",
         cxxopts::value<int>()->default_value("5"))
        ("rgb-base", "Base RGB color as comma separated string",
         cxxopts::value<std::string>()->default_value("255,0,0"))
        ("rgb-freq", "Frequency for RGB computation as comma separated string. You may use doubles but no negative values",
         cxxopts::value<std::string>()->default_value("0,16,16"))
        ("rgb-phase", "Phase for RGB computation as comma separated string",
         cxxopts::value<std::string>()->default_value("0,2,4"))
        ("zoom", "Zoom level. Use together with xcoord, ycoord",
         cxxopts::value<unsigned int>())
        ("xcoord", "Image X coordinate where you want to zoom into the fractal",
         cxxopts::value<unsigned int>())
        ("ycoord", "Image Y coordinate where you want to zoom into the fractal",
         cxxopts::value<unsigned int>());

    p.add_options("Export")
        ("p,print", "Print Buffer to terminal")
        ("csv", "Export data to csv files");
    // clang-format on
}

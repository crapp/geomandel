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

#include <cmath>
#include <iostream>
#include <memory>
/**
 * measure time
 */
#include <chrono>
#include <fstream>
#include <tuple>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include "config.h"
#include "global.h"
#include "main_helper.h"
#include "printer.h"

#include "image_pnm_bw.h"
#include "image_pnm_col.h"
#include "image_pnm_grey.h"
#ifdef HAVE_SFML
#include "image_sfml.h"
#endif

#include "csvwriter.h"

#include "fractalzoom.h"

//#include "fractalcrunchmulti.h"
#include "fractalcrunchsingle.h"

#include "ctpl_stl.h"
#include "cxxopts.hpp"

namespace bmp = boost::multiprecision;

/**
 * Based on
 * * http://beej.us/blog/data/mandelbrot-set
 * * http://www.math.tu-dresden.de/~jzumbr/fractals/fractals.html
 * * http://krazydad.com/tutorials/makecolors.php
 */

// TODO: Put this code in a separate class derived from Buffwriter
void prnt_buff(const constants::fracbuff &buff, unsigned int bailout)
{
    for (auto &v : buff) {
        for (auto &val : v) {
            if (val.default_index == bailout) {
                std::cout << "*";

            } else {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
    cxxopts::Options parser("geomandel", " - command line options");
    configure_command_line_parser(parser);
    try {
        parser.parse(argc, argv);
    } catch (const cxxopts::OptionParseException &ex) {
        std::cerr << parser.help({"", "Fractal", "Image", "Export"})
                  << std::endl;
        std::cerr << "Could not parse command line arguments" << std::endl;
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    if (parser.count("help")) {
        std::cout << parser.help({"", "Fractal", "Image", "Export"})
                  << std::endl;
        return 0;
    }

    std::shared_ptr<Printer> prnt =
        std::make_shared<Printer>(static_cast<bool>(parser.count("quiet")));
#ifdef HAVE_BOOST
    std::shared_ptr<FractalParameters<bmp::cpp_dec_float_100>> params = nullptr;
#else
    std::shared_ptr<FractalParameters<double>> params = nullptr;
#endif
    init_fractal_paramters(params, parser);

    if (params == nullptr) {
        std::cerr << parser.help({"", "Fractal", "Image", "Export"})
                  << std::endl;
        std::cerr << "Could not parse command line arguments" << std::endl;
        return 1;
    }

    std::string version =
        std::string(GEOMANDEL_MAJOR) + "." + std::string(GEOMANDEL_MINOR);
    if (std::string(GEOMANDEL_PATCH) != "0") {
        version += "." + std::string(GEOMANDEL_PATCH);
    }

    std::string frac_type = "Mandelbrot";

    // Set a string according to fractal type for better output
    if (params->set_type == constants::FRACTAL::TRICORN) {
        frac_type = "Tricorn";
    }
    if (params->set_type == constants::FRACTAL::JULIA) {
        frac_type = "Julia Set";
    }
    if (params->set_type == constants::FRACTAL::BURNING_SHIP) {
        frac_type = "Burning Ship";
    }

    // FIXME: real and Imaginary part only seem to have a precision of 5 digits
    // whereas the Zoom level is printed in scientific notation correctly.

    prnt << "+++++++++++++++++++++++++++++++++++++" << std::endl;
    prnt << "+       Welcome to geomandel " << version << std::endl;
    prnt << "+                                    " << std::endl;
    prnt << "+ Fractal type '" << frac_type << "'" << std::endl;
    prnt << "+ Bailout: " << std::to_string(params->bailout) << std::endl;
    prnt << "+ Complex plane:" << std::endl;
    prnt << "+   Im " << params->yl << " " << params->yh << std::endl;
    prnt << "+   Re " << params->xl << " " << params->xh << std::endl;
    prnt << "+ Image: " << std::to_string(params->xrange) << "x"
         << std::to_string(params->yrange) << std::endl;
    prnt << "+ Zoom: " << std::endl;
    prnt << "+   Coordinate " << params->xcoord << ", " << params->ycoord
         << std::endl;
    prnt << "+   Level " << params->zoom << "x" << std::endl;

    // TODO: Using a two dimensional vector is unnecessary. Have a look at
    // test_computation.cpp for a better solution.
    // create the buffer that holds our data
    constants::fracbuff fractalbuffer;
    fractalbuffer.assign(params->yrange, std::vector<constants::Iterations>());
    for (auto &v : fractalbuffer) {
        v.assign(params->xrange, constants::Iterations());
    }

    std::unique_ptr<Fractalcruncher> crunchi = nullptr;

    if (parser.count("m")) {
        prnt << "+ Multicore: " << params->cores << std::endl;
        // crunchi = std::unique_ptr<Fractalcrunchmulti>(
        // new Fractalcrunchmulti(fractalbuffer, params));
    } else {
        prnt << "+ Singlecore " << std::endl;
        crunchi = std::unique_ptr<Fractalcrunchsingle>(
            new Fractalcrunchsingle(fractalbuffer, params));
    }

    // Do the work
    std::chrono::time_point<std::chrono::system_clock> tbegin;
    tbegin = std::chrono::system_clock::now();
    crunchi->fill_buffer();
    std::chrono::time_point<std::chrono::system_clock> tend =
        std::chrono::system_clock::now();

    // calculate time delta
    auto deltat =
        std::chrono::duration_cast<std::chrono::milliseconds>(tend - tbegin);
    prnt << "+" << std::endl;
    prnt << "+ Fractalcruncher time " << deltat.count() << "ms \n+" << std::endl;

    // TODO: More refactoring needed here. Would be nice to move this somewhere
    // else. Maybe we could put this into the Mandelparameters structure.
    // The way we make it right now is not testable by Catch.
    // TODO: Shouldn't we use unsigned int in rgb tuples?

    // visualize/export the crunched numbers
    // std::unique_ptr<Buffwriter> img;
    // std::unique_ptr<Buffwriter> csv =
    // std::unique_ptr<CSVWriter>(new CSVWriter(fractalbuffer, params));
    //if (parser.count("image-pnm-bw")) {
    // prnt << "+ Generating B/W image" << std::endl;
    // img = std::unique_ptr<ImageBW>(new ImageBW(fractalbuffer, params, prnt));
    // img->write_buffer();
    //}
    //if (parser.count("image-pnm-grey")) {
    // prnt << "+ Generating grey scale bitmap" << std::endl;
    //unsigned int grey_base = parser["grey-base"].as<unsigned int>();
    //// do we need to use std::fabs for the parsed double here?
    //double grey_freq = parser["grey-freq"].as<double>();
    // img = std::unique_ptr<Imagegrey>(new Imagegrey(
    // fractalbuffer, params, prnt, std::make_tuple(grey_base, 0, 0),
    // std::make_tuple(grey_freq, 0, 0)));
    // img->write_buffer();
    //}
    //if (parser.count("image-pnm-col")) {
    // prnt << "+ Generating RGB bitmap" << std::endl;
    // std::tuple<int, int, int> rgb_base;
    // std::tuple<int, int, int> rgb_set_base;
    // std::tuple<double, double, double> rgb_freq;
    // std::tuple<int, int, int> rgb_phase;
    // std::tuple<double, double, double> rgb_amp;
    // parse_rgb_command_options(parser, rgb_base, rgb_set_base, rgb_freq,
    // rgb_phase, rgb_amp);
    // img = std::unique_ptr<Imagecol>(
    // new Imagecol(fractalbuffer, params, prnt, std::move(rgb_base),
    // std::move(rgb_set_base), std::move(rgb_freq),
    // std::move(rgb_phase), std::move(rgb_amp)));
    // img->write_buffer();
    //}
    // uint8_t png_jpg = 0;
    //if (parser.count("image-png"))
    // png_jpg |= static_cast<uint8_t>(constants::OUT_FORMAT::IMAGE_PNG);

    //if (parser.count("image-jpg"))
    // png_jpg |= static_cast<uint8_t>(constants::OUT_FORMAT::IMAGE_JPG);
    // if (png_jpg != 0) {
    // prnt << "+ Generating jpg/png image" << std::endl;
    // std::tuple<int, int, int> rgb_base;
    // std::tuple<int, int, int> rgb_set_base;
    // std::tuple<double, double, double> rgb_freq;
    // std::tuple<int, int, int> rgb_phase;
    // std::tuple<double, double, double> rgb_amp;
    // parse_rgb_command_options(parser, rgb_base, rgb_set_base, rgb_freq,
    // rgb_phase, rgb_amp);
    //// TODO: Don't like ifdefs in code. Maybe better off with an "empty"
    //// ImageSFML stub class
    //#ifdef HAVE_SFML
    // img = std::unique_ptr<ImageSFML>(
    // new ImageSFML(fractalbuffer, params, prnt, std::move(rgb_base),
    // std::move(rgb_set_base), std::move(rgb_freq),
    // std::move(rgb_phase), std::move(rgb_amp), png_jpg));
    // img->write_buffer();
    //#endif
    //}

    //if (parser.count("csv")) {
    // prnt << "+ Exporting data to csv files" << std::endl;
    // csv->write_buffer();
    //}
    //if (parser.count("p"))
    // prnt_buff(fractalbuffer, params->bailout);  // print the buffer

    // prnt << "+\n+" << std::endl;
    // prnt << "+++++++++++++++++++++++++++++++++++++" << std::endl << std::endl;
    return 0;
}

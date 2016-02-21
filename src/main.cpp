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

#include <iostream>
#include <memory>
#include <cmath>
/**
 * measure time
 */
#include <chrono>
#include <fstream>
#include <tuple>

#include "global.h"
#include "main_helper.h"
#include "config.h"

#include "imagebw.h"
#include "imagegrey.h"
#include "imagecol.h"

#include "csvwriter.h"

#include "mandelzoom.h"

#include "mandelcrunchsingle.h"
#include "mandelcrunchmulti.h"

#include "ctpl_stl.h"
#include "cxxopts.hpp"

/**
 * Based on
 * * http://beej.us/blog/data/mandelbrot-set
 * * http://www.math.tu-dresden.de/~jzumbr/fractals/fractals.html
 * * http://krazydad.com/tutorials/makecolors.php
 */

// TODO: Put this code in a separate class derived from Buffwriter
void prnt_buff(const constants::mandelbuff &buff, unsigned int bailout)
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
        std::cerr << parser.help({"", "Mandelbrot", "Image", "Export"})
                  << std::endl;
        std::cerr << "Could not parse command line arguments" << std::endl;
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    if (parser.count("help")) {
        std::cout << parser.help({"", "Mandelbrot", "Image", "Export"})
                  << std::endl;
        return 0;
    }

    std::shared_ptr<MandelParameters> params = nullptr;
    init_mandel_parameters(params, parser);

    if (params == nullptr) {
        std::cerr << parser.help({"", "Mandelbrot", "Image", "Export"})
                  << std::endl;
        std::cerr << "Could not parse command line arguments" << std::endl;
        return 1;
    }

    std::string version =
        std::string(GEOMANDEL_MAJOR) + "." + std::string(GEOMANDEL_MINOR);
    if (std::string(GEOMANDEL_PATCH) != "0") {
        version += "." + std::string(GEOMANDEL_PATCH);
    }

    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "+       Welcome to geomandel " << version << std::endl;
    std::cout << "+                                    " << std::endl;
    std::cout << "+ Bailout: " << std::to_string(params->bailout) << std::endl;
    std::cout << "+ Complex plane:" << std::endl;
    std::cout << "+   Im " << params->yl << " " << params->yh << std::endl;
    std::cout << "+   Re " << params->xl << " " << params->xh << std::endl;
    std::cout << "+ Image " << std::to_string(params->xrange) << "x"
              << std::to_string(params->yrange) << std::endl;

    // TODO: Using a two dimensional vector is unnecessary. Have a look at
    // test_computation.cpp for a better solution.
    // create the buffer that holds our data
    constants::mandelbuff mandelbuffer;
    mandelbuffer.assign(params->yrange, std::vector<constants::Iterations>());
    for (auto &v : mandelbuffer) {
        v.assign(params->xrange, constants::Iterations());
    }

    std::unique_ptr<Mandelcruncher> crunchi;

    if (parser.count("m")) {
        std::cout << "+ Multicore: " << params->cores << std::endl;
        crunchi = std::unique_ptr<Mandelcrunchmulti>(
            new Mandelcrunchmulti(mandelbuffer, params));
    } else {
        std::cout << "+ Singlecore " << std::endl;
        crunchi = std::unique_ptr<Mandelcrunchsingle>(
            new Mandelcrunchsingle(mandelbuffer, params));
    }

    std::chrono::time_point<std::chrono::system_clock> tbegin;
    tbegin = std::chrono::system_clock::now();
    crunchi->fill_buffer();
    std::chrono::time_point<std::chrono::system_clock> tend =
        std::chrono::system_clock::now();

    // calculate time delta
    auto deltat =
        std::chrono::duration_cast<std::chrono::milliseconds>(tend - tbegin);
    std::cout << "+" << std::endl;
    std::cout << "+ Mandelcruncher time " << deltat.count() << "ms \n+"
              << std::endl;

    // TODO: More refactoring needed here. Would be nice to move this somewhere
    // else. Maybe we could put this into the Mandelparameters structure.
    // The way we make it right now is not testable by Catch.
    // TODO: Shouldn't we use unsigned int in rgb tuples
    // visualize/export the crunched numbers
    std::unique_ptr<Buffwriter> img;
    std::unique_ptr<Buffwriter> csv =
        std::unique_ptr<CSVWriter>(new CSVWriter(mandelbuffer, params));
    if (parser.count("img-bandw")) {
        std::cout << "+ Generating B/W image" << std::endl;
        img = std::unique_ptr<ImageBW>(new ImageBW(mandelbuffer, params));
        img->write_buffer();
    }
    if (parser.count("img-greyscale")) {
        std::cout << "+ Generating greyscale bitmap" << std::endl;
        int grey_base = parser["grey-base"].as<int>();
        int grey_freq = parser["grey-freq"].as<int>();
        img = std::unique_ptr<Imagegrey>(
            new Imagegrey(mandelbuffer, params, std::make_tuple(grey_base, 0, 0),
                          std::make_tuple(grey_freq, 0, 0)));
        img->write_buffer();
    }
    if (parser.count("img-color")) {
        std::cout << "+ Generating RGB bitmap" << std::endl;
        // read command line parameters and create rgb tuples
        std::vector<std::string> rgb_base_vec;
        utility::split(parser["rgb-base"].as<std::string>(), ',', rgb_base_vec);
        auto rgb_base = std::make_tuple(std::stoi(rgb_base_vec.at(0)),
                                        std::stoi(rgb_base_vec.at(1)),
                                        std::stoi(rgb_base_vec.at(2)));
        std::vector<std::string> rgb_freq_vec;
        utility::split(parser["rgb-freq"].as<std::string>(), ',', rgb_freq_vec);
        auto rgb_freq =
            std::make_tuple(std::fabs(std::stod(rgb_freq_vec.at(0))),
                            std::fabs(std::stod(rgb_freq_vec.at(1))),
                            std::fabs(std::stod(rgb_freq_vec.at(2))));
        std::vector<std::string> rgb_phase_vec;
        utility::split(parser["rgb-phase"].as<std::string>(), ',',
                       rgb_phase_vec);
        auto rgb_phase = std::make_tuple(std::stoi(rgb_phase_vec.at(0)),
                                         std::stoi(rgb_phase_vec.at(1)),
                                         std::stoi(rgb_phase_vec.at(2)));
        img = std::unique_ptr<Imagecol>(
            new Imagecol(mandelbuffer, params, std::move(rgb_base),
                         std::move(rgb_freq), std::move(rgb_phase)));
        img->write_buffer();
    }
    if (parser.count("csv")) {
        std::cout << "+ Exporting data to csv files" << std::endl;
        csv->write_buffer();
    }
    if (parser.count("p"))
        prnt_buff(mandelbuffer, params->bailout);  // print the buffer

    std::cout << "+\n+ All data written, Good Bye" << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl
              << std::endl;
    return 0;
}

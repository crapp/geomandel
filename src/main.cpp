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
#include "config.h"

#include "imagebw.h"
#include "imagegrey.h"
#include "imagecol.h"

#include "csvwriter.h"

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

void prnt_buff(const constants::mandelbuff &buff)
{
    for (auto &v : buff) {
        for (auto &val : v) {
            std::cout << val.default_index;
        }
        std::cout << std::endl;
    }
}

void setup_command_line_parser(cxxopts::Options &p)
{
    // clang-format off
    p.add_options()
        ("help", "Show this help")
        ("m,multi", "Use multiple cores",
         cxxopts::value<int>()->implicit_value("1"));

    p.add_options("Mandelbrot")
        ("b,bailout", "Bailout value for the mandelbrot set algorithm",
         cxxopts::value<int>()->default_value("1000"))
        ("creal-min", "Real part minimum",
         cxxopts::value<double>()->default_value("-2.5"))
        ("creal-max", "Real part maximum",
         cxxopts::value<double>()->default_value("1.0"))
        ("cima-min", "Imaginary part minimum",
         cxxopts::value<double>()->default_value("-1.5"))
        ("cima-max", "Imaginary part maximum",
         cxxopts::value<double>()->default_value("1.5"));

    p.add_options("Image")
        ("w,width", "Image width", cxxopts::value<int>()->default_value("1000"))
        ("h,height", "Image height",
         cxxopts::value<int>()->default_value("1000"))
        ("bandw", "Write Buffer to B&W Bitmap")
        ("greyscale", "Write Buffer to Greyscale Bitmap")
        ("color", "Write Buffer to RGB Bitmap")
        ("colalgo", "Coloring algorithm 0->Escape Time, 1->Continuous Coloring",
         cxxopts::value<int>()->default_value("0"))
        ("grey-base", "Base grey color between 0 - 255",
         cxxopts::value<int>()->default_value("55"))
        ("grey-freq", "Frequency for grey shade computation",
         cxxopts::value<int>()->default_value("5"))
        ("rgb-base", "Base RGB color as comma separated string",
         cxxopts::value<std::string>()->default_value("255,0,0"))
        ("rgb-freq", "Frequency for RGB computation as comma separated string",
         cxxopts::value<std::string>()->default_value("0,16,16"))
        ("rgb-phase", "Phase for RGB computation as comma separated string",
         cxxopts::value<std::string>()->default_value("0,4,2"));

    p.add_options("Export")
        ("p,print", "Print Buffer to terminal")
        ("csv", "Export data to csv files");
    // clang-format on
}

int main(int argc, char *argv[])
{
    cxxopts::Options parser("geomandel", " - command line options");
    setup_command_line_parser(parser);
    try {
        parser.parse(argc, argv);
    } catch (const cxxopts::OptionParseException &ex) {
        std::cout << parser.help({"", "Mandelbrot", "Image", "Export"})
                  << std::endl;
        std::cerr << "Could not parse command line options" << std::endl;
        std::cerr << ex.what() << std::endl;
        exit(1);
    }

    if (parser.count("help")) {
        std::cout << parser.help({"", "Mandelbrot", "Image", "Export"})
                  << std::endl;
        exit(0);
    }
    int bailout = parser["b"].as<int>();

    // define some variables
    double xl = parser["creal-min"].as<double>();
    double xh = parser["creal-max"].as<double>();
    int xrange = parser["w"].as<int>();

    double yl = parser["cima-min"].as<double>();
    double yh = parser["cima-max"].as<double>();
    int yrange = parser["h"].as<int>();

    MandelParameters params(xrange, xl, xh, yrange, yl, yh, bailout);

    std::string version =
        std::string(GEOMANDEL_MAJOR) + "." + std::string(GEOMANDEL_MINOR);
    if (std::string(GEOMANDEL_PATCH) != "0") {
        version += "." + std::string(GEOMANDEL_PATCH);
    }

    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "+       Welcome to geomandel " << version << std::endl;
    std::cout << "+                                    " << std::endl;
    std::cout << "+ Bailout: " << std::to_string(bailout) << std::endl;
    std::cout << "+ Complex plane:" << std::endl;
    std::cout << "+   Im " << yl << " " << yh << std::endl;
    std::cout << "+   Re " << xl << " " << xh << std::endl;
    std::cout << "+ Image " << std::to_string(xrange) << "x"
              << std::to_string(yrange) << std::endl;

    // create the buffer that holds our data
    constants::mandelbuff mandelbuffer;
    mandelbuffer.assign(yrange, std::vector<constants::Iterations>());
    for (auto &v : mandelbuffer) {
        v.assign(xrange, constants::Iterations());
    }

    std::unique_ptr<Mandelcruncher> crunchi;

    if (parser.count("m")) {
        std::cout << "+ Multicore: " << std::to_string(parser["m"].as<int>())
                  << std::endl;
        crunchi = std::unique_ptr<Mandelcrunchmulti>(new Mandelcrunchmulti(
            mandelbuffer, params,
            static_cast<constants::COL_ALGO>(parser["colalgo"].as<int>()),
            parser["m"].as<int>()));
    } else {
        std::cout << "+ Singlecore " << std::endl;
        crunchi = std::unique_ptr<Mandelcrunchsingle>(new Mandelcrunchsingle(
            mandelbuffer, params,
            static_cast<constants::COL_ALGO>(parser["colalgo"].as<int>())));
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
    std::cout << "+ Mandelcruncher time " << deltat.count() << "ms" << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl
              << std::endl;

    // visualize/export the crunched numbers
    std::unique_ptr<Buffwriter> img;
    std::unique_ptr<Buffwriter> csv =
        std::unique_ptr<CSVWriter>(new CSVWriter(mandelbuffer));
    if (parser.count("bandw")) {
        std::cout << " Generating B/W image" << std::endl;
        img = std::unique_ptr<ImageBW>(new ImageBW(
            mandelbuffer,
            static_cast<constants::COL_ALGO>(parser["colalgo"].as<int>()),
            params.bailout, constants::OUT_FORMAT::IMAGE_BW));
        img->write_buffer();
    }
    if (parser.count("greyscale")) {
        std::cout << " Generating greyscale bitmap" << std::endl;
        int grey_base = parser["grey-base"].as<int>();
        int grey_freq = parser["grey-freq"].as<int>();
        img = std::unique_ptr<Imagegrey>(new Imagegrey(
            mandelbuffer,
            static_cast<constants::COL_ALGO>(parser["colalgo"].as<int>()),
            params.bailout, constants::OUT_FORMAT::IMAGE_GREY,
            std::make_tuple(grey_base, 0, 0), std::make_tuple(grey_freq, 0, 0)));
        img->write_buffer();
    }
    if (parser.count("color")) {
        std::cout << " Generating RGB bitmap" << std::endl;
        // read command line parameters and create rgb tuples
        std::vector<std::string> rgb_base_vec;
        utility::split(parser["rgb-base"].as<std::string>(), ',', rgb_base_vec);
        auto rgb_base = std::make_tuple(std::stoi(rgb_base_vec.at(0)),
                                        std::stoi(rgb_base_vec.at(1)),
                                        std::stoi(rgb_base_vec.at(2)));
        std::vector<std::string> rgb_freq_vec;
        utility::split(parser["rgb-freq"].as<std::string>(), ',', rgb_freq_vec);
        auto rgb_freq = std::make_tuple(std::stoi(rgb_freq_vec.at(0)),
                                        std::stoi(rgb_freq_vec.at(1)),
                                        std::stoi(rgb_freq_vec.at(2)));
        std::vector<std::string> rgb_phase_vec;
        utility::split(parser["rgb-base"].as<std::string>(), ',', rgb_phase_vec);
        auto rgb_phase = std::make_tuple(std::stoi(rgb_phase_vec.at(0)),
                                         std::stoi(rgb_phase_vec.at(1)),
                                         std::stoi(rgb_phase_vec.at(2)));
        img = std::unique_ptr<Imagecol>(new Imagecol(
            mandelbuffer,
            static_cast<constants::COL_ALGO>(parser["colalgo"].as<int>()),
            params.bailout, constants::OUT_FORMAT::IMAGE_COL,
            std::move(rgb_base), std::move(rgb_freq), std::move(rgb_phase)));
        img->write_buffer();
    }
    if (parser.count("csv")) {
        std::cout << " Exporting data to csv files" << std::endl;
        csv->write_buffer();
    }
    if (parser.count("p"))
        prnt_buff(mandelbuffer);  // print the buffer

    std::cout << "\n All data written, Good Bye" << std::endl;
    return 0;
}

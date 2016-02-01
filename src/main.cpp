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

#include "global.h"
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

void write_buff_to_image(const constants::mandelbuff &buff, int maxiter,
                         constants::OUT_FORMAT f, constants::COL_ALGO colalgo)
{
    // Escape time algorithm
    // This will overwrite any existing image. Image is written into the
    // directory from where the application was called.
    std::string filename = "geomandel." + constants::BITMAP_DEFS.at(f).at(0);
    std::ofstream img(filename, std::ofstream::out);
    img.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        if (img.is_open()) {
            // magic number for bitmap
            img << constants::BITMAP_DEFS.at(f).at(1) << std::endl;
            // comments
            img << "# geomandel output" << std::endl;
            // specify width and height of the bitmap
            img << buff.at(0).size() << " " << buff.size() << std::endl;
            if (f == constants::OUT_FORMAT::IMAGE_GREY)
                img << 255 << std::endl;
            for (const auto &v : buff) {
                int linepos = 1;
                for (auto data : v) {
                    // this kind of images don't allow for more than 70
                    // characters in one row
                    if (linepos % 70 == 0) {
                        img << std::endl;
                        linepos = 0;
                    }
                    if (f == constants::OUT_FORMAT::IMAGE_BW) {
                        if (data.default_index == maxiter) {
                            img << 1 << " ";
                        } else {
                            img << 0 << " ";
                        }
                    }
                    if (f == constants::OUT_FORMAT::IMAGE_GREY) {
                        int its = data.default_index;
                        if (colalgo == constants::COL_ALGO::ESCAPE_TIME) {
                            // not very efficient to do some of the math over
                            // and over again. Hopefully the compiler will
                            // optimize this ;)
                            if (its < maxiter / 2) {
                                img << 127 + (floor(its / (maxiter / 127.0)))
                                    << " ";
                            } else if (its >= maxiter / 2 && its != maxiter) {
                                img << floor(its / (maxiter / 127.0)) << " ";
                            } else if (its == maxiter) {
                                img << 0 << " ";
                            }
                        }
                        if (colalgo == constants::COL_ALGO::CONTINUOUS) {
                            if (its < maxiter) {
                                double continuous_index = data.continous_index;
                                img << std::sin(0.016 * continuous_index + 4) *
                                               230 +
                                           25
                                    << " ";
                            } else {
                                img << 0 << " ";
                            }
                        }
                    }
                    if (f == constants::OUT_FORMAT::IMAGE_COL) {
                    }
                    linepos++;
                }
            }
        }
    } catch (const std::ifstream::failure &e) {
        std::cout << "Error writing image file" << std::endl;
        std::cout << e.what() << std::endl;
    }
}

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
        ("p,print", "Print Buffer to Screen")
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
        ("bwimage", "Write Buffer to B&W Bitmap")
        ("greyscale", "Write Buffer to Greyscale Bitmap")
        ("colalgo", "Coloring algorithm 0->Escape Time, 1->Continuous Coloring",
         cxxopts::value<int>()->default_value("0"));
    // clang-format on
}

int main(int argc, char *argv[])
{
    cxxopts::Options parser("geomandel", " - command line options");
    setup_command_line_parser(parser);
    try {
        parser.parse(argc, argv);
    } catch (const cxxopts::OptionParseException &ex) {
        std::cout << parser.help({"", "Mandelbrot", "Image"}) << std::endl;
        std::cerr << "Could not parse command line options" << std::endl;
        std::cerr << ex.what() << std::endl;
        exit(1);
    }

    if (parser.count("help")) {
        std::cout << parser.help({"", "Mandelbrot", "Image"}) << std::endl;
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

    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "+       Welcome to geomandel        " << std::endl;
    std::cout << "+                                   " << std::endl;
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
    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;

    // visualize what the crunched numbers
    if (parser.count("p"))
        prnt_buff(mandelbuffer);  // print the buffer
    if (parser.count("bwimage"))
        write_buff_to_image(
            mandelbuffer, bailout, constants::OUT_FORMAT::IMAGE_BW,
            static_cast<constants::COL_ALGO>(parser["colalgo"].as<int>()));
    if (parser.count("greyscale"))
        write_buff_to_image(
            mandelbuffer, bailout, constants::OUT_FORMAT::IMAGE_GREY,
            static_cast<constants::COL_ALGO>(parser["colalgo"].as<int>()));
    std::cout << "\n All data written, Good Bye" << std::endl;
    return 0;
}

// A Mandelbrot Set paired with GeoTIFF
// Copyright © 2015, 2016 Christian Rapp
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <vector>
#include <iostream>
#include <string>
/** 
 * measure time
 */
#include <chrono>
#include <fstream>

#include "ctpl_stl.h"
#include "cxxopts.hpp"
/**
 * Based on http://beej.us/blog/data/mandelbrot-set
 *
 */

int mandel_cruncher(double x, double y, int bailout)
{
    // std::cout << "Calculating [" << x << "/" << y << "]" << std::endl;
    int iterations = 0;
    double x0 = x;
    double y0 = y;
    while (x * x + y * y <= 4.0 && iterations < bailout) {
        double x_old = x;
        x = x * x - y * y + x0;
        y = 2 * x_old * y + y0;
        iterations++;
    }
    if (iterations == bailout)
        return 1;
    //     return iterations;
    return 0;
}

void write_buff_to_image(const std::vector<std::vector<int>> &buff)
{
    // Escape time algorithm
    // This will overwrite any existing image. Image is written into the
    // directory from where the application was called.
    std::ofstream img("geomandel.pbm", std::ofstream::out);
    img.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        if (img.is_open()) {
            // magic number for bw bitmap
            img << "P1" << std::endl;
            // comments
            img << "# geomandel bw output" << std::endl;
            // specifiy width and height of the pbm
            img << buff.at(0).size() << " " << buff.size() << std::endl;
            for (const auto &v : buff) {
                int linepos = 1;
                for (auto data : v) {
                    // this kind of images don't allow for more than 70
                    // characters in one row
                    if (linepos % 70) {
                        img << std::endl;
                        linepos = 0;
                    }
                    if (data > 0) {
                        img << 1;
                    } else {
                        img << data;
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

void prnt_buff(const std::vector<std::vector<int>> &buff)
{
    // Escape time algorithm
    for (auto &v : buff) {
        for (auto &val : v) {
            std::cout << val;
        }
        std::cout << std::endl;
    }
}

void setup_command_line_parser(cxxopts::Options &p)
{
    // clang-format off
    p.add_options()
        ("b,bailout", "Bailout value for the mandelbrot set algorithm", 
         cxxopts::value<int>()->default_value("1000"))
        ("p,print", "Print Buffer to Screen", 
         cxxopts::value<bool>()->default_value("0"))
        ("bwimage", "Write Buffer to B&W Bitmap", 
         cxxopts::value<bool>()->default_value("1"))
        ("greyscale", "Write Buffer to Greyscale Bitmap", 
         cxxopts::value<bool>()->default_value("1"));
    // clang-format on
}
int main(int argc, char *argv[])
{
    cxxopts::Options parser(argv[0], " - command line options");
    setup_command_line_parser(parser);
    try {
        parser.parse(argc, argv);
    } catch (const cxxopts::OptionParseException &ex) {
        std::cout << parser.help({""}) << std::endl;
        std::cerr << "Could not parse command line options" << std::endl;
        std::cerr << ex.what() << std::endl;
    }

    if (parser.count("help")) {
        std::cout << parser.help({""}) << std::endl;
        exit(0);
    }
    int bailout = parser["b"].as<int>();
    std::vector<std::vector<int>> mandelbuffer;

    // define some variables
    double x = -1.5;
    double xl = -1.5;
    double xh = 1.5;
    double xrange = 480;
    double xdelta = ((xl * -1) + xh) / xrange;
    double y = -2.5;
    double yl = -2.5;
    double yh = 1.0;
    double yrange = 640;
    double ydelta = ((yl * -1) + yh) / yrange;

    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "+       Welcome to geomandel        +" << std::endl;
    std::cout << "+                                   +" << std::endl;
    std::cout << "+ Bailout: " << std::to_string(bailout) << "           +"
              << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;

    // create the buffer that holds our data
    mandelbuffer.assign(xrange, std::vector<int>());
    for (auto &v : mandelbuffer) {
        v.assign(yrange, 0);
    }

    std::chrono::time_point<std::chrono::system_clock> tbegin =
        std::chrono::system_clock::now();
    // calculate complex numbers for all pixels
    for (int ix = 0; ix < xrange; ix++) {
        for (int iy = 0; iy <= yrange; iy++) {
            int its = mandel_cruncher(x, y, bailout);
            mandelbuffer[ix][iy] = its;
            y += ydelta;
        }
        x += xdelta;
        y = yl;
    }
    std::chrono::time_point<std::chrono::system_clock> tend =
        std::chrono::system_clock::now();
    auto deltat =
        std::chrono::duration_cast<std::chrono::milliseconds>(tend - tbegin);

    std::cout << "Mandelcruncher time " << deltat.count() << "ms" << std::endl;

    if (parser.count("p") && parser["p"].as<bool>() == true)
        prnt_buff(mandelbuffer);  // print the buffer
    if (parser.count("bwimage"))
        write_buff_to_image(mandelbuffer);
    return 0;
}

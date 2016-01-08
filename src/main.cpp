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
#include "cmdparser.hpp"

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
    if (iterations < bailout)
        return 0;
  //     return iterations;
    return iterations;
}

void write_buff_to_image(const std::vector<std::vector<int>> &buff)
{
    std::ofstream img("geomandel.pbm", std::ofstream::out);
    img.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        if (img.is_open()) {
            img << "P1" << std::endl;
            img << "# geomandel bw output" << std::endl;
            img << buff.at(0).size() << " " << buff.size() << std::endl;
            for (const auto &v : buff) {
                int linepos = 1;
                for (auto data : v) {
                    if (linepos % 70) {
                        img << std::endl;
                        linepos = 0;
                    }
                    img << data;
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
    for (auto &v : buff) {
        for (auto &val : v) {
            std::cout << val;
        }
        std::cout << std::endl;
    }
}

void setup_command_line_parser(cli::Parser &p)
{
    p.set_optional<int>("o", "optional", 0, "Mandelbrot Set orientation");
    p.set_optional<int>("b", "bailout", 1000,
                        "How long do we try to calculate the complex number");
    p.set_required<std::string>("g", "geotiff", "GEOTiff file to use");
    p.run_and_exit_if_error();
}

int main(int argc, char *argv[])
{
    // get the command line parser working
    cli::Parser parser(argc, argv);
    setup_command_line_parser(parser);

    int bailout = parser.get<int>("b");
    std::vector<std::vector<int>> mandelbuffer;

    // define some variables
    double x = -2.5;
    double xl = -2.0;
    double xh = 1.0;
    double xrange = 100;
    double xdelta = ((xl * -1) + xh) / xrange;
    double y = -1.5;
    double yl = -1.5;
    double yh = 1.5;
    double yrange = 200;
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
            if (its > 0)
                mandelbuffer[ix][iy] = 1;
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

    // print the buffer
    prnt_buff(mandelbuffer);
    write_buff_to_image(mandelbuffer);
    return 0;
}

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
#include <map>
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

namespace constants
{
enum OUT_FORMAT { IMAGE_BW, IMAGE_GREY, IMAGE_COL, GEOTIFF };

const std::map<OUT_FORMAT, std::vector<std::string>> BITMAP_DEFS{
    {OUT_FORMAT::IMAGE_BW, {"pbm", "P1"}},
    {OUT_FORMAT::IMAGE_GREY, {"pgm", "P2"}}};
}

/**
 * @brief Mandelbrot algorithm. 
 *
 * @param x
 * @param y
 * @param bailout
 *
 * @details
 * Returns the number of iterations it took to check whether the complex number 
 * made of x and y (representing the imaginary and the real part of a complex 
 * number) is within our 4.0 radius and therefor inside the Mandelbrot. This means
 * if the number of iterations is equal to bailout we assume this complex number 
 * to be part of the mandelbrot set. The number of iterations can later be used 
 * to make a colored image what is also called the Escape Time Algorithm. 
 *
 * @return Return number of iterations.
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
    return iterations;
}

/**
 * @brief Single thread mandelbrot set calculator
 *
 * @param buff The Buffer we use to store the number of iterations
 * @param xrange
 * @param xdelta
 * @param x
 * @param yrange
 * @param ydelta
 * @param y
 * @param yl
 * @param bailout
 */
void mandel_single(std::vector<std::vector<int>> &buff, double xrange,
                   double xdelta, double x, double xl, double yrange,
                   double ydelta, double y, double yl, int bailout)
{
    // calculate pixel by pixel
    for (int iy = 0; iy < yrange; iy++) {
        for (int ix = 0; ix <= xrange; ix++) {
            int its = mandel_cruncher(x, y, bailout);
            buff[iy][ix] = its;
            x += xdelta;
        }
        y += ydelta;
        x = xl;
    }
}

void mandel_multi(std::vector<std::vector<int>> &buff, double xrange,
                  double xdelta, double x, double xl, double yrange,
                  double ydelta, double y, double yl, int bailout, int cores)
{
    // a vector filled with futures. We will wait for all of them to be finished.
    std::vector<std::future<void>> futures;
    ctpl::thread_pool tpl(cores);

    // calculate the madelbrot set line by line. Each line will be pushed to the
    // thread pool as separate job.
    int iy = 0; /**< row to calculate*/
    for (auto &int_vec : buff) {
        futures.push_back(tpl.push([&int_vec, xrange, xdelta, x, xl, yrange,
                                    ydelta, y, yl, iy, bailout](int id) {
                double ypass = y;
                double xpass = x;
                if (iy != 0)
                    ypass += ydelta * iy;
                for (int ix = 0; ix <= xrange; ix++) {
                    int its = mandel_cruncher(xpass, ypass, bailout);
                    int_vec[ix] = its;
                    xpass += xdelta;
                }
        }));
        iy++;
    }
    // make sure all jobs are finished
    for (const std::future<void> &f : futures) {
        f.wait();
    }
}

void write_buff_to_image(const std::vector<std::vector<int>> &buff, int maxiter,
                         constants::OUT_FORMAT f)
{
    // Escape time algorithm
    // This will overwrite any existing image. Image is written into the
    // directory from where the application was called.
    std::string filename = "geomandel." + constants::BITMAP_DEFS.at(f).at(0);
    std::ofstream img(filename, std::ofstream::out);
    img.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        if (img.is_open()) {
            // magic number for bw bitmap
            img << constants::BITMAP_DEFS.at(f).at(1) << std::endl;
            // comments
            img << "# geomandel output" << std::endl;
            // specify width and height of the pbm
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
                        if (data > 0) {
                            img << 1 << " ";
                        } else {
                            img << data << " ";
                        }
                    }
                    if (f == constants::OUT_FORMAT::IMAGE_GREY) {
                        // not very efficient to do some of the math over
                        // and over again. Hopefully the compiler will
                        // optimize this ;)
                        if (data < maxiter / 2) {
                            img << 127 + (floor(data / (maxiter / 127.0)))
                                << " ";
                        } else if (data >= maxiter / 2 && data != maxiter) {
                            img << floor(data / (maxiter / 127.0)) << " ";
                        } else if (data == maxiter) {
                            img << 0 << " ";
                        }
                    }
                    if (f == constants::OUT_FORMAT::IMAGE_COL) {
                        if (data < maxiter / 2) {
                        }
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
        ("h,help", "Show this help")
        ("b,bailout", "Bailout value for the mandelbrot set algorithm", 
         cxxopts::value<int>()->default_value("1000"))
        ("p,print", "Print Buffer to Screen")
        ("bwimage", "Write Buffer to B&W Bitmap")
        ("greyscale", "Write Buffer to Greyscale Bitmap")
        ("m,multi", "Use multiple cores",
         cxxopts::value<int>()->implicit_value("1"));
    // clang-format on
}
int main(int argc, char *argv[])
{
    cxxopts::Options parser("geomandel", " - command line options");
    setup_command_line_parser(parser);
    try {
        parser.parse(argc, argv);
    } catch (const cxxopts::OptionParseException &ex) {
        std::cout << parser.help({""}) << std::endl;
        std::cerr << "Could not parse command line options" << std::endl;
        std::cerr << ex.what() << std::endl;
        exit(1);
    }

    if (parser.count("help")) {
        std::cout << parser.help({""}) << std::endl;
        exit(0);
    }
    int bailout = parser["b"].as<int>();
    std::vector<std::vector<int>> mandelbuffer;

    // define some variables
    double x = -2.5;
    double xl = -2.5;
    double xh = 1.0;
    double xrange = 1000;
    double xdelta = ((xl * -1) + xh) / xrange;
    double y = -1.5;
    double yl = -1.5;
    double yh = 1.5;
    double yrange = 1000;
    double ydelta = ((yl * -1) + yh) / yrange;

    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "+       Welcome to geomandel        " << std::endl;
    std::cout << "+                                   " << std::endl;
    std::cout << "+ Bailout: " << std::to_string(bailout) << std::endl;

    // create the buffer that holds our data
    mandelbuffer.assign(xrange, std::vector<int>());
    for (auto &v : mandelbuffer) {
        v.assign(yrange, 0);
    }

    std::chrono::time_point<std::chrono::system_clock> tbegin;
    if (parser.count("m")) {
        std::cout << "+ Multicore: " << std::to_string(parser["m"].as<int>())
                  << std::endl;
        tbegin = std::chrono::system_clock::now();
        mandel_multi(mandelbuffer, xrange, xdelta, x, xl, yrange, ydelta, y, yl,
                     bailout, parser["m"].as<int>());
    } else {
        std::cout << "+ Singlecore " << std::endl;
        tbegin = std::chrono::system_clock::now();
        mandel_single(mandelbuffer, xrange, xdelta, x, xl, yrange, ydelta, y, yl,
                      bailout);
    }
    std::chrono::time_point<std::chrono::system_clock> tend =
        std::chrono::system_clock::now();

    // calculate time delta
    auto deltat =
        std::chrono::duration_cast<std::chrono::milliseconds>(tend - tbegin);
    std::cout << "+ Mandelcruncher time " << deltat.count() << "ms" << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;

    // visualize what the crunched numbers
    if (parser.count("p"))
        prnt_buff(mandelbuffer);  // print the buffer
    if (parser.count("bwimage"))
        write_buff_to_image(mandelbuffer, bailout,
                            constants::OUT_FORMAT::IMAGE_BW);
    if (parser.count("greyscale"))
        write_buff_to_image(mandelbuffer, bailout,
                            constants::OUT_FORMAT::IMAGE_GREY);
    return 0;
}

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
#include <cmath>
#include <tuple>
/**
 * measure time
 */
#include <chrono>
#include <fstream>

#include "ctpl_stl.h"
#include "cxxopts.hpp"

/**
 * Based on
 * * http://beej.us/blog/data/mandelbrot-set
 * * http://www.math.tu-dresden.de/~jzumbr/fractals/fractals.html
 * * http://krazydad.com/tutorials/makecolors.php
 *
 */


/**
 * @brief namespace for constants, typedefs and structs
 */
namespace constants
{
enum OUT_FORMAT { IMAGE_BW, IMAGE_GREY, IMAGE_COL, GEOTIFF };
enum COL_ALGO { ESCAPE_TIME, CONTINUOUS };

const std::map<OUT_FORMAT, std::vector<std::string>> BITMAP_DEFS{
    {OUT_FORMAT::IMAGE_BW, {"pbm", "P1"}},
    {OUT_FORMAT::IMAGE_GREY, {"pgm", "P2"}}};

struct Iterations {

    int default_index;
    double continous_index;

    Iterations()
    {
        this->default_index = 0;
        this->continous_index = 0;
    }

    friend std::ostream& operator<<(std::ostream &out, const Iterations &it){
        out << "(" << it.default_index << ", " << it.continous_index << ")";
        return out;
    }
};

typedef std::vector<std::vector<Iterations>> mandelbuff;
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
 * to be part of the mandelbrot set.
 *
 * @return Return number of iterations as well as Real and Imaginary Part of the
 * Complex Number.
 */
std::tuple<int, double, double> mandel_cruncher(double x, double y, int bailout)
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
    return std::make_tuple(iterations, x, y);
}

/**
 * @brief Generates a tuple for our buffer according to the coloring algorithm
 *
 * @param its Number of iterations
 * @param Zx Real part of the complex number
 * @param Zy Imaginary part of the complex number
 * @param colalgo Coloring algorithm
 *
 * @return Manelbrot Set Buffer tuple
 */
constants::Iterations mandel_buff_iteration(int its, double Zx, double Zy,
                                            constants::COL_ALGO colalgo)
{
    constants::Iterations it;
    if (colalgo == constants::COL_ALGO::ESCAPE_TIME)
        it.default_index = its;
    if (colalgo == constants::COL_ALGO::CONTINUOUS) {
        double cont_index =
            its -
            (std::log(std::log(std::sqrt(Zx * Zx + Zy * Zy)))) / std::log(2.0);
        it.continous_index = cont_index;
    }
    return it;
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
void mandel_single(constants::mandelbuff &buff, int xrange, double xdelta,
                   double x, double xl, int yrange, double ydelta, double y,
                   double yl, int bailout, constants::COL_ALGO col)
{
    // calculate pixel by pixel
    for (int iy = 0; iy < yrange; iy++) {
        for (int ix = 0; ix < xrange; ix++) {
            auto crunched_mandel = mandel_cruncher(x, y, bailout);
            int its = std::get<0>(crunched_mandel);
            if (col == constants::COL_ALGO::ESCAPE_TIME) {
                constants::Iterations it = mandel_buff_iteration(its, 0, 0, col);
                buff[iy][ix].default_index = it.default_index;
            }
            if (col == constants::COL_ALGO::CONTINUOUS) {
                double Zx = std::get<1>(crunched_mandel);
                double Zy = std::get<2>(crunched_mandel);
                buff[iy][ix] = mandel_buff_iteration(its, Zx, Zy, col);
            }
            x += xdelta;
        }
        y += ydelta;
        x = xl;
    }
}

void mandel_multi(constants::mandelbuff &buff, int xrange, double xdelta,
                  double x, double xl, int yrange, double ydelta, double y,
                  double yl, int bailout, int cores, constants::COL_ALGO col)
{
    // a vector filled with futures. We will wait for all of them to be finished.
    std::vector<std::future<void>> futures;
    ctpl::thread_pool tpl(cores);

    // calculate the mandelbrot set line by line. Each line will be pushed to the
    // thread pool as separate job. The id parameter of the lambda function
    // represents the thread id.
    int iy = 0; /**< row to calculate*/
    for (auto &int_vec : buff) {
        futures.push_back(tpl.push([&int_vec, xrange, xdelta, x, xl, yrange,
                                    ydelta, y, yl, iy, bailout, col](int id) {
            double ypass = y;  // y value is constant for each row
            double xpass = x;
            if (iy != 0)
                ypass += ydelta * iy;
            for (int ix = 0; ix < xrange; ix++) {
                auto crunched_mandel = mandel_cruncher(xpass, ypass, bailout);
                int its = std::get<0>(crunched_mandel);
                if (col == constants::COL_ALGO::ESCAPE_TIME)
                    int_vec[ix] = mandel_buff_iteration(its, 0, 0, col);
                if (col == constants::COL_ALGO::CONTINUOUS) {
                    double Zx = std::get<1>(crunched_mandel);
                    double Zy = std::get<2>(crunched_mandel);
                    int_vec[ix] = mandel_buff_iteration(its, Zx, Zy, col);
                }
                // increment xpass by xdelta
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
                        if (data.default_index > 0) {
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
         cxxopts::value<int>()->default_value("1000"));

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
    double x = -2.0;
    double xl = -2.0;
    double xh = 1.0;
    int xrange = parser["w"].as<int>();
    double xdelta = ((xl * -1) + xh) / xrange;
    double y = -1.5;
    double yl = -1.5;
    double yh = 1.5;
    int yrange = parser["h"].as<int>();
    double ydelta = ((yl * -1) + yh) / yrange;

    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "+       Welcome to geomandel        " << std::endl;
    std::cout << "+                                   " << std::endl;
    std::cout << "+ Bailout: " << std::to_string(bailout) << std::endl;
    std::cout << "+ Width: " << std::to_string(xrange) << std::endl;
    std::cout << "+ Height: " << std::to_string(yrange) << std::endl;

    // create the buffer that holds our data
    constants::mandelbuff mandelbuffer;
    mandelbuffer.assign(yrange, std::vector<constants::Iterations>());
    for (auto &v : mandelbuffer) {
        v.assign(xrange, constants::Iterations());
    }

    std::chrono::time_point<std::chrono::system_clock> tbegin;
    if (parser.count("m")) {
        std::cout << "+ Multicore: " << std::to_string(parser["m"].as<int>())
                  << std::endl;
        tbegin = std::chrono::system_clock::now();
        mandel_multi(
            mandelbuffer, xrange, xdelta, x, xl, yrange, ydelta, y, yl, bailout,
            parser["m"].as<int>(),
            static_cast<constants::COL_ALGO>(parser["colalgo"].as<int>()));
    } else {
        std::cout << "+ Singlecore " << std::endl;
        tbegin = std::chrono::system_clock::now();
        mandel_single(
            mandelbuffer, xrange, xdelta, x, xl, yrange, ydelta, y, yl, bailout,
            static_cast<constants::COL_ALGO>(parser["colalgo"].as<int>()));
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

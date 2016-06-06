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

#include <memory>

#include "cxxopts.hpp"

#include "config.h"

#include "fractalparams.h"
#include "fractalzoom.h"

inline void init_mandel_parameters(std::shared_ptr<FractalParameters> &params,
                                   const cxxopts::Options &parser)
{
    // TODO: This try catch block could be unnecessary as cxxopts does most of
    // the checking itself when parse is called
    try {
        unsigned int stype = parser["f"].as<unsigned int>();
        constants::FRACTAL set_type;
        std::string fractal_type = "mandelbrot";
        switch (stype) {
        case 0:
            set_type = constants::FRACTAL::MANDELBROT;
            break;
        case 1:
            set_type = constants::FRACTAL::TRICORN;
            fractal_type = "tricorn";
            break;
        case 2:
            set_type = constants::FRACTAL::JULIA;
            fractal_type = "julia";
            break;
        case 3:
            set_type = constants::FRACTAL::BURNING_SHIP;
            fractal_type = "burning_ship";
            break;
        default:
            throw std::out_of_range("Fractal argument out of range");
        }

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

        double julia_real = parser["julia-real"].as<double>();
        double julia_ima = parser["julia-ima"].as<double>();

        double zoomlvl = 0;
        double xcoord = 0;
        double ycoord = 0;

        // check if user wants to zoom
        if (parser.count("zoom")) {
            if (!parser.count("xcoord") || !parser.count("ycoord")) {
                std::cerr << "Please provide x/y coordinates to zoom"
                          << std::endl;
                return;
            }
            // get zoom parameters and coordinate
            parser["zoom"].as<double>() == 0
                ? zoomlvl = 1
                : zoomlvl = parser["zoom"].as<double>();
            xcoord = parser["xcoord"].as<double>();
            ycoord = parser["ycoord"].as<double>();

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

            Fractalzoom zoomer;
            // calculate new complex plane
            zoomer.calcalute_zoom_cpane(xh, xl, yh, yl, zoomlvl, xcoord, ycoord,
                                        xrange, yrange);
        }

        unsigned int cores = 0;
        if (parser.count("m"))
            cores = parser["m"].as<unsigned int>();

        constants::COL_ALGO col_algo;
        unsigned int calgo = parser["col-algo"].as<unsigned int>();
        switch (calgo) {
        case 0:
            col_algo = constants::COL_ALGO::ESCAPE_TIME;
            break;
        case 1:
            col_algo = constants::COL_ALGO::CONTINUOUS_SINE;
            break;
        case 2:
            col_algo = constants::COL_ALGO::CONTINUOUS_BERN;
            break;
        default:
            throw std::out_of_range("Color algorithm argument out of range");
        }

        // Stores informations used by the mandel cruncher and some data
        // writer classes
        params = std::make_shared<FractalParameters>(
            set_type, xrange, xl, xh, yrange, yl, yh, julia_real, julia_ima,
            bailout, zoomlvl, xcoord, ycoord,
            parser["image-file"].as<std::string>(), fractal_type, cores,
            col_algo);
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
         cxxopts::value<unsigned int>()->implicit_value("2"))
        ("q,quiet", "Don't write to stdout (This does not influence stderr)");

    p.add_options("Fractal")
        ("f,fractal", "Choose which kind of fractal you want to compute and render",
         cxxopts::value<unsigned int>()->default_value("0"))
        ("b,bailout", "Bailout value for the fractal algorithm",
         cxxopts::value<unsigned int>()->default_value("1000"))
        ("creal-min", "Real part minimum",
         cxxopts::value<double>()->default_value("-2.0"))
        ("creal-max", "Real part maximum",
         cxxopts::value<double>()->default_value("1.0"))
        ("cima-min", "Imaginary part minimum",
         cxxopts::value<double>()->default_value("-1.5"))
        ("cima-max", "Imaginary part maximum",
         cxxopts::value<double>()->default_value("1.5"))
        ("julia-real", "Julia set constant real part",
         cxxopts::value<double>()->default_value("-0.8"))
        ("julia-ima", "Julia set constant imaginary part",
         cxxopts::value<double>()->default_value("0.156"));

    p.add_options("Image")
        ("image-file", "Image file name pattern. You can use different printf "
        "like '%' items interspersed with normal text for the output "
        "file name. Have a look in the README for more instructions.",
         cxxopts::value<std::string>()->default_value("geomandel"))
        ("w,width", "Image width",
         cxxopts::value<unsigned int>()->default_value("1000"))
        ("h,height", "Image height",
         cxxopts::value<unsigned int>()->default_value("1000"))
        ("image-pnm-bw", "Write Buffer to PBM Bitmap")
        ("image-pnm-grey", "Write Buffer to grey scale PGM")
        ("image-pnm-col", "Write Buffer to PPM Bitmap")
#ifdef HAVE_SFML
        ("image-jpg", "Write Buffer to JPG image")
        ("image-png", "Write Buffer to PNG image")
#endif
        ("col-algo", "Coloring algorithm 0->Escape Time Linear, "
         "1->Continuous Coloring Sine, "
         "2->Continuous Coloring Bernstein" ,
         cxxopts::value<unsigned int>()->default_value("1"))
        ("grey-base", "Base grey shade between 0 - 255",
         cxxopts::value<unsigned int>()->default_value("55"))
        ("grey-freq", "Frequency for grey shade computation",
         cxxopts::value<double>()->default_value("0.01"))
        ("rgb-base", "Base RGB color as comma separated string",
         cxxopts::value<std::string>()->default_value("128,128,128"))
        ("rgb-freq", "Frequency for RGB computation as comma separated string. You may use decimals but no negative values",
         cxxopts::value<std::string>()->default_value("0.01,0.01,0.01"))
        ("rgb-phase", "Phase for Sine Wave RGB computation as comma separated string",
         cxxopts::value<std::string>()->default_value("0,2,4"))
        ("rgb-amp", "Amplitude for Bernstein RGB computation as comma separated string",
         cxxopts::value<std::string>()->default_value("9,15,8.5"))
        ("set-color", "Color for pixels inside the set",
         cxxopts::value<std::string>()->default_value("0,0,0"))
        ("zoom", "Zoom level. Use together with xcoord, ycoord",
         cxxopts::value<double>())
        ("xcoord", "Image X coordinate where you want to zoom into the fractal",
         cxxopts::value<double>())
        ("ycoord", "Image Y coordinate where you want to zoom into the fractal",
         cxxopts::value<double>());

    p.add_options("Export")
        ("p,print", "Print Buffer to terminal")
        ("csv", "Export data to csv files");
    // clang-format on
}

void parse_rgb_command_options(const cxxopts::Options &parser,
                               std::tuple<int, int, int> &rgb_base,
                               std::tuple<int, int, int> &rgb_set_base,
                               std::tuple<double, double, double> &rgb_freq,
                               std::tuple<int, int, int> &rgb_phase,
                               std::tuple<double, double, double> &rgb_amp)
{
    // read command line parameters and create rgb tuples
    std::vector<std::string> rgb_base_vec;
    utility::split(parser["rgb-base"].as<std::string>(), ',', rgb_base_vec);
    rgb_base = std::make_tuple(std::stoi(rgb_base_vec.at(0)),
                               std::stoi(rgb_base_vec.at(1)),
                               std::stoi(rgb_base_vec.at(2)));
    std::vector<std::string> rgb_set_base_vec;
    utility::split(parser["set-color"].as<std::string>(), ',', rgb_set_base_vec);
    rgb_set_base = std::make_tuple(std::stoi(rgb_set_base_vec.at(0)),
                                   std::stoi(rgb_set_base_vec.at(1)),
                                   std::stoi(rgb_set_base_vec.at(2)));
    std::vector<std::string> rgb_freq_vec;
    utility::split(parser["rgb-freq"].as<std::string>(), ',', rgb_freq_vec);
    rgb_freq = std::make_tuple(std::fabs(std::stod(rgb_freq_vec.at(0))),
                               std::fabs(std::stod(rgb_freq_vec.at(1))),
                               std::fabs(std::stod(rgb_freq_vec.at(2))));
    std::vector<std::string> rgb_phase_vec;
    utility::split(parser["rgb-phase"].as<std::string>(), ',', rgb_phase_vec);
    rgb_phase = std::make_tuple(std::stoi(rgb_phase_vec.at(0)),
                                std::stoi(rgb_phase_vec.at(1)),
                                std::stoi(rgb_phase_vec.at(2)));
    std::vector<std::string> rgb_amp_vec;
    utility::split(parser["rgb-amp"].as<std::string>(), ',', rgb_amp_vec);
    rgb_amp = std::make_tuple(std::fabs(std::stod(rgb_amp_vec.at(0))),
                              std::fabs(std::stod(rgb_amp_vec.at(1))),
                              std::fabs(std::stod(rgb_amp_vec.at(2))));
}

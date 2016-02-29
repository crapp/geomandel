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

#include "catch.hpp"

#include "global.h"
#include "main_helper.h"
#include "mandelparams.h"
#include "mandelzoom.h"

#include <iostream>
#include <vector>
#include <string>
#include <memory>

cxxopts::Options generate_empty_parser()
{
    cxxopts::Options parser("Unittester", "Geomandel unittester");
    configure_command_line_parser(parser);
    return parser;
}

TEST_CASE("Command line arguments", "[commandline]")
{
    SECTION("Basic parser checking, expecting exception")
    {
        auto parser = generate_empty_parser();
        const char *test_argv[] = {"some", "idiotic", "--options"};
        char **cxxopt_pointer = const_cast<char **>(test_argv);
        int test_argc = 3;
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
    }

    SECTION("Test options that need an argument")
    {
        char **cxxopt_pointer = nullptr;
        // bailout
        auto parser = generate_empty_parser();
        const char *test_argv_bail[] = {"Unittester", "-b"};
        cxxopt_pointer = const_cast<char **>(test_argv_bail);
        int test_argc = 2;
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // creal-min
        parser = generate_empty_parser();
        const char *test_argv_crealm[] = {"Unittester", "--creal-min"};
        cxxopt_pointer = const_cast<char **>(test_argv_crealm);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // creal-max
        parser = generate_empty_parser();
        const char *test_argv_crealmax[] = {"Unittester", "--creal-max"};
        cxxopt_pointer = const_cast<char **>(test_argv_crealmax);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // cima-min
        parser = generate_empty_parser();
        const char *test_argv_cima_min[] = {"Unittester", "--cima-min"};
        cxxopt_pointer = const_cast<char **>(test_argv_cima_min);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // cima-max
        parser = generate_empty_parser();
        const char *test_argv_cima_max[] = {"Unittester", "--cima-max"};
        cxxopt_pointer = const_cast<char **>(test_argv_cima_max);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // image-file
        parser = generate_empty_parser();
        const char *test_argv_img_file[] = {"Unittester", "--image-file"};
        cxxopt_pointer = const_cast<char **>(test_argv_img_file);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // width
        parser = generate_empty_parser();
        const char *test_argv_width[] = {"Unittester", "-w"};
        cxxopt_pointer = const_cast<char **>(test_argv_width);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // height
        parser = generate_empty_parser();
        const char *test_argv_height[] = {"Unittester", "-h"};
        cxxopt_pointer = const_cast<char **>(test_argv_height);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // colalgo
        parser = generate_empty_parser();
        const char *test_argv_colalgo[] = {"Unittester", "--colalgo"};
        cxxopt_pointer = const_cast<char **>(test_argv_colalgo);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // grey-base
        parser = generate_empty_parser();
        const char *test_argv_grey_base[] = {"Unittester", "--grey-base"};
        cxxopt_pointer = const_cast<char **>(test_argv_grey_base);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // grey-freq
        parser = generate_empty_parser();
        const char *test_argv_grey_freq[] = {"Unittester", "--grey-freq"};
        cxxopt_pointer = const_cast<char **>(test_argv_grey_freq);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // rgb-base
        parser = generate_empty_parser();
        const char *test_argv_rgb_base[] = {"Unittester", "--rgb-base"};
        cxxopt_pointer = const_cast<char **>(test_argv_grey_freq);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // rgb-freq
        parser = generate_empty_parser();
        const char *test_argv_rgb_freq[] = {"Unittester", "--rgb-freq"};
        cxxopt_pointer = const_cast<char **>(test_argv_grey_freq);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // rgb-phase
        parser = generate_empty_parser();
        const char *test_argv_rgb_phase[] = {"Unittester", "--rgb-phase"};
        cxxopt_pointer = const_cast<char **>(test_argv_rgb_phase);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // zoom
        parser = generate_empty_parser();
        const char *test_argv_zoom[] = {"Unittester", "--zoom"};
        cxxopt_pointer = const_cast<char **>(test_argv_zoom);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // xcoord
        parser = generate_empty_parser();
        const char *test_argv_xcoord[] = {"Unittester", "--xcoord"};
        cxxopt_pointer = const_cast<char **>(test_argv_xcoord);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
        // ycoord
        parser = generate_empty_parser();
        const char *test_argv_ycoord[] = {"Unittester", "--ycoord"};
        cxxopt_pointer = const_cast<char **>(test_argv_ycoord);
        REQUIRE_THROWS(parser.parse(test_argc, cxxopt_pointer));
    }

    SECTION("Test command line parameters \"logic\"")
    {
        // max lower min not allowed
        std::shared_ptr<MandelParameters> params = nullptr;
        auto parser = generate_empty_parser();
        const char *test_argv_max_low_min[] = {"Unittester", "--creal-min=-1.8",
                                               "--creal-max=-2.9"};
        int test_argc = 3;
        char **cxxopt_pointer = const_cast<char **>(test_argv_max_low_min);
        parser.parse(test_argc, cxxopt_pointer);
        init_mandel_parameters(params, parser);
        REQUIRE(params == nullptr);

        params = nullptr;
        parser = generate_empty_parser();
        const char *test_argv_max_low_min_ima[] = {"Unittester", "--cima-min=3",
                                                   "--cima-max=1"};
        cxxopt_pointer = const_cast<char **>(test_argv_max_low_min_ima);
        test_argc = 3;
        parser.parse(test_argc, cxxopt_pointer);
        init_mandel_parameters(params, parser);
        REQUIRE(params == nullptr);

        // no parameters at all should give as a default Mandelparameters object
        params = nullptr;
        parser = generate_empty_parser();
        const char *test_argv_empty[] = {"Unittester"};
        test_argc = 1;
        cxxopt_pointer = const_cast<char **>(test_argv_empty);
        parser.parse(test_argc, cxxopt_pointer);
        init_mandel_parameters(params, parser);
        REQUIRE(params != nullptr);
    }
}

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

#include <iostream>
#include <vector>
#include <string>

cxxopts::Options generate_empty_parser()
{
    cxxopts::Options parser("UnitTest command line parser");
    configure_command_line_parser(parser);
    return parser;
}

TEST_CASE("Command line arguments", "[commandline]")
{
    std::vector<const char *> test_argv_vec;
    SECTION("Basic parser checking, expecting exception")
    {
        auto parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"some", "idiotic", "--options"};
        char **test_argv = const_cast<char **>(test_argv_vec.data());
        int test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
    }
    SECTION("Test options that need an argument")
    {
        // bailout
        auto parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "-b"};
        char **test_argv = const_cast<char **>(test_argv_vec.data());
        int test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // creal-min
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--creal-min"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // creal-max
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--creal-max"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // cima-min
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--cima-min"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // cima-max
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--cima-max"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // image-file
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--image-file"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // width
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "-w"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // height
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "-h"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // colalgo
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--colalgo"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // grey-base
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--grey-base"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // grey-freq
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--grey-freq"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // rgb-base
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--rgb-base"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // rgb-freq
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--rgb-freq"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // rgb-phase
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--rgb-phase"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // zoom
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--zoom"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // xcoord
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--xcoord"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
        // ycoord
        parser = generate_empty_parser();
        test_argv_vec.clear();
        test_argv_vec = {"Unittester", "--ycoord"};
        test_argv = const_cast<char **>(test_argv_vec.data());
        test_argc = static_cast<int>(test_argv_vec.size());
        REQUIRE_THROWS(parser.parse(test_argc, test_argv));
    }
    SECTION("Test ") {}
}

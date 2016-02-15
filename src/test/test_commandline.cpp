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

#include <vector>
#include <string>

TEST_CASE("Command line arguments", "[commandline]")
{
    int test_argc = 1;
    std::vector<const char *> test_argv(test_argc + 1);
    const char *first_opt = "--help";
    test_argv[0] = first_opt;
    cxxopts::Options parser("Test command line parser");
    utility::configure_command_line_parser(parser);
    char **test = const_cast<char **>(test_argv.data());
    parser.parse(test_argc, test);
}

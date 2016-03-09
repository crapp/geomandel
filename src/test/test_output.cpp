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

#include "buffwriter_mock.h"

TEST_CASE("Filename Patterns", "[output]")
{
    // some sane values for supported filename patterns
    unsigned int bailout = 2048;  // %b
    unsigned int xrange = 1024;   // %w
    unsigned int yrange = 768;    // %h
    unsigned int zoom = 0;        // %z
    unsigned int cores = 4;       // %c
    unsigned int xcoord = 323;    // %x
    unsigned int ycoord = 233;    // %y
    double z_real_min = -2.5;     // %Zr
    double z_real_max = 1.0;      // %ZR
    double z_ima_min = -1.5;      // %Zi
    double z_ima_max = 1.5;       // %ZI

    constants::mandelbuff b;
    BuffwriterMock bmock(b);

    SECTION("Simple filename with three patterns")
    {
        std::string pattern = "mandi_%wx%h_%z.png";
        std::string filename = bmock.test_filename_patterns(
            pattern, bailout, xrange, yrange, zoom, cores, xcoord, ycoord,
            z_real_min, z_real_max, z_ima_min, z_ima_max);
        REQUIRE(filename ==
                "mandi_" + std::to_string(xrange) + "x" +
                    std::to_string(yrange) + "_" + std::to_string(zoom) +
                    ".png");
    }

    SECTION("Complex file name with all patterns")
    {
        // %a is a fake pattern
        std::string pattern =
            "mandeltest_%a_%wx%h_b%b_m%c_[%x, %y --> %z]_(%Zr, %Zi)->(%ZR, %ZI)";
        std::string filename = bmock.test_filename_patterns(
            std::move(pattern), bailout, xrange, yrange, zoom, cores, xcoord,
            ycoord, z_real_min, z_real_max, z_ima_min, z_ima_max);
        REQUIRE(
            filename ==
            "mandeltest_%a_" + std::to_string(xrange) + "x" +
                std::to_string(yrange) + "_b" + std::to_string(bailout) + "_m" +
                std::to_string(cores) + "_[" + std::to_string(xcoord) + ", " +
                std::to_string(ycoord) + " --> " + std::to_string(zoom) + "]_(" +
                std::to_string(z_real_min) + ", " + std::to_string(z_ima_min) +
                ")->(" + std::to_string(z_real_max) + ", " +
                std::to_string(z_ima_max) + ")");
    }
}

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

#include "catch.hpp"

#include "buffwriter_mock.h"
#include "global.h"

TEST_CASE("Filename Patterns", "[output]")
{
    // some sane values for supported filename patterns
    std::string fractal_type = "unit_test_fractal";  // %f
    unsigned int bailout = 2048;                     // %b
    unsigned int xrange = 1024;                      // %w
    unsigned int yrange = 768;                       // %h
    unsigned int zoom = 0;                           // %z
    unsigned int cores = 4;                          // %c
    double xcoord = 323;                             // %x
    double ycoord = 233;                             // %y
    double z_real_min = -2.5;                        // %Zr
    double z_real_max = 1.0;                         // %ZR
    double z_ima_min = -1.5;                         // %Zi
    double z_ima_max = 1.5;                          // %ZI

    constants::fracbuff b;
    BuffwriterMock bmock(b);

    SECTION("Simple filename with three patterns")
    {
        std::string pattern = "mandi_%wx%h_%z.png";
        std::string filename = bmock.test_filename_patterns(
            pattern, fractal_type, bailout, xrange, yrange, zoom, cores, xcoord,
            ycoord, z_real_min, z_real_max, z_ima_min, z_ima_max);
        REQUIRE(filename ==
                "mandi_" + utility::primitive_to_string(xrange) + "x" +
                    utility::primitive_to_string(yrange) + "_" +
                    utility::primitive_to_string(zoom) + ".png");
    }

    SECTION("Special case where .0 floats should be displayed as integers")
    {
        std::string pattern = "mandeltest_[%x-%y]";
        xcoord = 280.0;
        ycoord = 178.0;
        std::string filename = bmock.test_filename_patterns(
            pattern, fractal_type, bailout, xrange, yrange, zoom,
            cores, xcoord, ycoord, z_real_min, z_real_max, z_ima_min, z_ima_max);
        REQUIRE(filename ==
                "mandeltest_[" + utility::primitive_to_string(xcoord) + "-" +
                    utility::primitive_to_string(ycoord) + "]");
    }
    SECTION("Complex file name with all patterns")
    {
        // %a is a fake pattern
        std::string pattern =
            "mandeltest_%f_%a_%wx%h_b%b_m%c_[%x, %y --> %z]_(%Zr, %Zi)->(%ZR, "
            "%ZI)";
        std::string filename = bmock.test_filename_patterns(
            std::move(pattern), fractal_type, bailout, xrange, yrange, zoom,
            cores, xcoord, ycoord, z_real_min, z_real_max, z_ima_min, z_ima_max);
        REQUIRE(filename ==
                "mandeltest_" + fractal_type + "_%a_" +
                    utility::primitive_to_string(xrange) + "x" +
                    utility::primitive_to_string(yrange) + "_b" +
                    utility::primitive_to_string(bailout) + "_m" +
                    utility::primitive_to_string(cores) + "_[" +
                    utility::primitive_to_string(xcoord) + ", " +
                    utility::primitive_to_string(ycoord) + " --> " +
                    utility::primitive_to_string(zoom) + "]_(" +
                    utility::primitive_to_string(z_real_min) + ", " +
                    utility::primitive_to_string(z_ima_min) + ")->(" +
                    utility::primitive_to_string(z_real_max) + ", " +
                    utility::primitive_to_string(z_ima_max) + ")");
    }
}

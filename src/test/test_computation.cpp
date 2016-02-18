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
#include "mandelzoom.h"

TEST_CASE("Computation of zoom values", "[computation]")
{
    Mandelzoom zoomer;
    const double real_min = -2.0;
    const double real_max = 1.0;
    const double ima_min = -1.5;
    const double ima_max = 1.5;

    double real_min_zoom = real_min;
    double real_max_zoom = real_max;
    double ima_min_zoom = ima_min;
    double ima_max_zoom = ima_max;

    // thats the width of the image
    unsigned int steps_real = 1000;
    // thats the height of the image
    unsigned int steps_ima = 1000;

    // zoom level
    unsigned int zoom = 30;
    // x/y coordinate in the image space
    unsigned int xcoord = 200;
    unsigned int ycoord = 300;

    SECTION("Test 30x zoom")
    {
        zoomer.calcalute_zoom_cpane(real_max_zoom, real_min_zoom, ima_max_zoom,
                                    ima_min_zoom, zoom, xcoord, ycoord,
                                    steps_real, steps_ima);
        REQUIRE(real_min_zoom == Catch::Detail::Approx(-1.45));
        REQUIRE(real_max_zoom == Catch::Detail::Approx(-1.35));
        REQUIRE(ima_min_zoom == Catch::Detail::Approx(-0.65));
        REQUIRE(ima_max_zoom == Catch::Detail::Approx(-0.55));
    }

    SECTION("Test 69x zoom")
    {
        zoom = 69;
        zoomer.calcalute_zoom_cpane(real_max_zoom, real_min_zoom, ima_max_zoom,
                                    ima_min_zoom, zoom, xcoord, ycoord,
                                    steps_real, steps_ima);
        REQUIRE(real_min_zoom == Catch::Detail::Approx(-1.42173913043));
        REQUIRE(real_max_zoom == Catch::Detail::Approx(-1.37826086957));
        REQUIRE(ima_min_zoom == Catch::Detail::Approx(-0.621739130435));
        REQUIRE(ima_max_zoom == Catch::Detail::Approx(-0.578260869565));
    }

    SECTION("Test 20x zoom, 640x480, 89x321")
    {
        zoom = 20;
        steps_real = 640;
        steps_ima = 480;
        xcoord = 89;
        ycoord = 321;

        zoomer.calcalute_zoom_cpane(real_max_zoom, real_min_zoom, ima_max_zoom,
                                    ima_min_zoom, zoom, xcoord, ycoord,
                                    steps_real, steps_ima);
        REQUIRE(real_min_zoom == Catch::Detail::Approx(-1.6578125));
        REQUIRE(real_max_zoom == Catch::Detail::Approx(-1.5078125));
        REQUIRE(ima_min_zoom == Catch::Detail::Approx(0.43125));
        REQUIRE(ima_max_zoom == Catch::Detail::Approx(0.58125));
    }

    SECTION("Test zoom 1000x at same coordinates like before")
    {
        zoom = 1000;
        steps_real = 640;
        steps_ima = 480;
        xcoord = 89;
        ycoord = 321;

        zoomer.calcalute_zoom_cpane(real_max_zoom, real_min_zoom, ima_max_zoom,
                                    ima_min_zoom, zoom, xcoord, ycoord,
                                    steps_real, steps_ima);
        REQUIRE(real_min_zoom == Catch::Detail::Approx(-1.5843125));
        REQUIRE(real_max_zoom == Catch::Detail::Approx(-1.5813125));
        REQUIRE(ima_min_zoom == Catch::Detail::Approx(0.50475));
        REQUIRE(ima_max_zoom == Catch::Detail::Approx(0.50775));
    }
}

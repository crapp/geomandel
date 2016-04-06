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

#include <vector>
#include <memory>
#include <algorithm>

#include "catch.hpp"

#include "global.h"
#include "fractalzoom.h"

#include "fractalcruncher_mock.h"

/**
 * @brief Fills a vector<int> with escape time integers.
 *
 * @param tbuff
 * @param z_real_min
 * @param z_real_max
 * @param z_ima_min
 * @param z_ima_max
 * @param bailout
 * @param width
 * @param height
 * @param crunch_test A mock like object that implements some wrapper methods so
 * we can access private Methods in Fractalcruncher class
 */
void fill_test_buffer(std::vector<int> &tbuff, double z_real_min,
                      double z_real_max, double z_ima_min, double z_ima_max,
                      unsigned int bailout, unsigned int width,
                      unsigned int height,
                      const FractalcruncherMock &crunch_test)
{
    double real_delta = (z_real_max - z_real_min) / width;
    double ima_delta = (z_ima_max - z_ima_min) / height;

    for (unsigned int yi = 0; yi < height; yi++) {
        for (unsigned int xi = 0; xi < width; xi++) {
            auto crunched_fractal =
                crunch_test.test_cruncher(z_real_min + (xi * real_delta),
                                          z_ima_min + (yi * ima_delta), bailout);
            tbuff.push_back(std::get<0>(crunched_fractal));
        }
    }
}

TEST_CASE("Computation of zoom values", "[computation]")
{
    Fractalzoom zoomer;
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
    double xcoord = 200;
    double ycoord = 300;

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

TEST_CASE(
    "Test computation of complex numbers and iteration count for Mandelbrot Set",
    "[computation][mandelbrot_set]")
{
    // creating a mock like object to be able to test computation. No need for
    // a real buffer or FractalParameters object
    constants::fracbuff b;
    std::shared_ptr<FractalParameters> params =
        std::make_shared<FractalParameters>();
    params->set_type = constants::FRACTAL::MANDELBROT;

    FractalcruncherMock crunch_test_mandel(b, params);

    SECTION(
        "Test some single point computations with the Mandelbrot Set, z1(-2.5, "
        "-1.5) - z2(1.0 - 1.5)")
    {
        double z_real_delta = (1.0 - (-2.5)) / 50;
        double z_ima_delta = (1.5 - (-1.5)) / 50;

        // testing point 10/15
        auto crunched_fractal = crunch_test_mandel.test_cruncher(
            -2.5 + 10 * z_real_delta, -1.5 + 15 * z_ima_delta, 100);
        REQUIRE(std::get<0>(crunched_fractal) == 2);
        REQUIRE(std::get<1>(crunched_fractal) == Catch::Detail::Approx(-3.0672));
        REQUIRE(std::get<2>(crunched_fractal) == Catch::Detail::Approx(2.7696));

        // testing point 33/25
        crunched_fractal = crunch_test_mandel.test_cruncher(
            -2.5 + 33 * z_real_delta, -1.5 + 25 * z_ima_delta, 100);
        REQUIRE(std::get<0>(crunched_fractal) == 100);
        REQUIRE(std::get<1>(crunched_fractal) ==
                Catch::Detail::Approx(-0.16332495807107994));
        REQUIRE(std::get<2>(crunched_fractal) == Catch::Detail::Approx(0));

        // testing point 41/27
        crunched_fractal = crunch_test_mandel.test_cruncher(
            -2.5 + 41 * z_real_delta, -1.5 + 27 * z_ima_delta, 100);
        REQUIRE(std::get<0>(crunched_fractal) == 55);
        REQUIRE(std::get<1>(crunched_fractal) ==
                Catch::Detail::Approx(2.3637850846657784));
        REQUIRE(std::get<2>(crunched_fractal) ==
                Catch::Detail::Approx(-2.264372597523388));
    }

    SECTION("Testing Mandelbrot Set 10x10, Bailout 10, -2.5 - 1.0 -1.5 - 1.5")
    {
        std::vector<int> result = {
            0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  0,  0,  0,  1,  2,  2,  2,
            2,  1,  1,  0,  0,  0,  2,  2,  3,  4,  10, 3,  1,  0,  0,  2,  2,
            3,  5,  10, 10, 10, 2,  0,  0,  3,  4,  10, 10, 10, 10, 10, 2,  0,
            0,  10, 10, 10, 10, 10, 10, 10, 3,  0,  0,  3,  4,  10, 10, 10, 10,
            10, 2,  0,  0,  2,  2,  3,  5,  10, 10, 10, 2,  0,  0,  0,  2,  2,
            3,  4,  10, 3,  1,  0,  0,  0,  1,  2,  2,  2,  2,  1,  1};

        std::vector<int> escape;

        fill_test_buffer(escape, -2.5, 1.0, -1.5, 1.5, 10, 10, 10,
                         crunch_test_mandel);

        std::pair<std::vector<int>::iterator, std::vector<int>::iterator>
            first_mismatch;
        // search for first mismatch
        first_mismatch =
            std::mismatch(result.begin(), result.end(), escape.begin());
        // okay if no mismatch
        REQUIRE(first_mismatch.first == result.end());
        REQUIRE(first_mismatch.second == escape.end());
    };

    SECTION("Testing Mandelbrot Set 20x40, Bailout 88, -2.5 - 1.0 -1.5 - 1.5")
    {
        std::vector<int> result = {
            0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
            1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,
            1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,
            1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,
            1,  1,  2,  2,  2,  2,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,
            0,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,
            0,  0,  0,  1,  1,  2,  2,  2,  2,  2,  3,  5,  3,  2,  1,  1,  1,
            1,  0,  0,  0,  0,  0,  1,  1,  2,  2,  2,  2,  3,  3,  5,  5,  3,
            2,  1,  1,  1,  0,  0,  0,  0,  0,  1,  2,  2,  2,  2,  2,  3,  4,
            6,  11, 3,  3,  2,  1,  1,  0,  0,  0,  0,  0,  1,  2,  2,  2,  2,
            3,  3,  4,  8,  10, 4,  3,  2,  1,  1,  0,  0,  0,  0,  1,  2,  2,
            2,  2,  2,  3,  3,  5,  24, 68, 4,  3,  2,  2,  1,  0,  0,  0,  0,
            1,  2,  2,  2,  2,  3,  3,  5,  6,  88, 88, 6,  4,  2,  2,  1,  0,
            0,  0,  0,  1,  2,  2,  2,  2,  3,  4,  10, 11, 16, 88, 12, 6,  3,
            2,  1,  0,  0,  0,  0,  2,  2,  2,  2,  3,  4,  5,  13, 25, 88, 88,
            88, 14, 4,  2,  2,  0,  0,  0,  0,  2,  2,  2,  3,  4,  4,  5,  12,
            88, 88, 88, 88, 88, 4,  2,  2,  0,  0,  0,  0,  2,  2,  2,  5,  4,
            5,  6,  88, 88, 88, 88, 88, 33, 4,  2,  2,  0,  0,  0,  0,  2,  3,
            3,  8,  6,  6,  7,  88, 88, 88, 88, 88, 88, 6,  2,  2,  0,  0,  0,
            1,  3,  3,  4,  7,  11, 16, 10, 88, 88, 88, 88, 88, 88, 5,  2,  2,
            0,  0,  0,  2,  3,  3,  4,  8,  88, 88, 13, 88, 88, 88, 88, 88, 88,
            5,  3,  2,  0,  0,  0,  2,  3,  4,  6,  11, 88, 88, 21, 88, 88, 88,
            88, 88, 88, 4,  3,  2,  0,  0,  0,  3,  4,  5,  8,  19, 88, 88, 42,
            88, 88, 88, 88, 88, 88, 4,  3,  2,  0,  0,  0,  88, 88, 88, 88, 88,
            88, 88, 88, 88, 88, 88, 88, 88, 11, 4,  3,  2,  0,  0,  0,  3,  4,
            5,  8,  19, 88, 88, 42, 88, 88, 88, 88, 88, 88, 4,  3,  2,  0,  0,
            0,  2,  3,  4,  6,  11, 88, 88, 21, 88, 88, 88, 88, 88, 88, 4,  3,
            2,  0,  0,  0,  2,  3,  3,  4,  8,  88, 88, 13, 88, 88, 88, 88, 88,
            88, 5,  3,  2,  0,  0,  0,  1,  3,  3,  4,  7,  11, 16, 10, 88, 88,
            88, 88, 88, 88, 5,  2,  2,  0,  0,  0,  0,  2,  3,  3,  8,  6,  6,
            7,  88, 88, 88, 88, 88, 88, 6,  2,  2,  0,  0,  0,  0,  2,  2,  2,
            5,  4,  5,  6,  88, 88, 88, 88, 88, 33, 4,  2,  2,  0,  0,  0,  0,
            2,  2,  2,  3,  4,  4,  5,  12, 88, 88, 88, 88, 88, 4,  2,  2,  0,
            0,  0,  0,  2,  2,  2,  2,  3,  4,  5,  13, 25, 88, 88, 88, 14, 4,
            2,  2,  0,  0,  0,  0,  1,  2,  2,  2,  2,  3,  4,  10, 11, 16, 88,
            12, 6,  3,  2,  1,  0,  0,  0,  0,  1,  2,  2,  2,  2,  3,  3,  5,
            6,  88, 88, 6,  4,  2,  2,  1,  0,  0,  0,  0,  1,  2,  2,  2,  2,
            2,  3,  3,  5,  24, 68, 4,  3,  2,  2,  1,  0,  0,  0,  0,  0,  1,
            2,  2,  2,  2,  3,  3,  4,  8,  10, 4,  3,  2,  1,  1,  0,  0,  0,
            0,  0,  1,  2,  2,  2,  2,  2,  3,  4,  6,  11, 3,  3,  2,  1,  1,
            0,  0,  0,  0,  0,  1,  1,  2,  2,  2,  2,  3,  3,  5,  5,  3,  2,
            1,  1,  1,  0,  0,  0,  0,  0,  1,  1,  2,  2,  2,  2,  2,  3,  5,
            3,  2,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  2,  2,  2,
            2,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,
            1,  1,  2,  2,  2,  2,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,
            0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,
            0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
            1};

        std::vector<int> escape;

        fill_test_buffer(escape, -2.5, 1.0, -1.5, 1.5, 88, 20, 40,
                         crunch_test_mandel);

        std::pair<std::vector<int>::iterator, std::vector<int>::iterator>
            first_mismatch;
        // search for first mismatch
        first_mismatch =
            std::mismatch(result.begin(), result.end(), escape.begin());
        // okay if no mismatch
        REQUIRE(first_mismatch.first == result.end());
        REQUIRE(first_mismatch.second == escape.end());
    };
}

TEST_CASE(
    "Test computation of complex numbers and iteration count for Julia Set",
    "[computation][julia_set]")
{
    // creating a mock like object to be able to test computation. No need for
    // a real buffer or FractalParameters object
    constants::fracbuff b;
    std::shared_ptr<FractalParameters> params =
        std::make_shared<FractalParameters>();
    params->set_type = constants::FRACTAL::JULIA;
    params->julia_real = -0.8;
    params->julia_ima = 0.156;

    FractalcruncherMock crunch_test_julia(b, params);

    SECTION(
        "Test some single point computations with the Julia Set, z1(-2.5-1.5j) "
        "- z2(1.0+1.5j)")
    {
        double z_real_delta = (1.0 - (-2.5)) / 50;
        double z_ima_delta = (1.5 - (-1.5)) / 50;

        // testing point 10/15
        auto crunched_fractal = crunch_test_julia.test_cruncher(
            -2.5 + 10 * z_real_delta, -1.5 + 15 * z_ima_delta, 100);
        REQUIRE(std::get<0>(crunched_fractal) == 1);
        REQUIRE(std::get<1>(crunched_fractal) == Catch::Detail::Approx(2.08));
        REQUIRE(std::get<2>(crunched_fractal) == Catch::Detail::Approx(2.316));

        // testing point 33/25
        crunched_fractal = crunch_test_julia.test_cruncher(
            -2.5 + 33 * z_real_delta, -1.5 + 25 * z_ima_delta, 100);
        REQUIRE(std::get<0>(crunched_fractal) == 22);
        REQUIRE(std::get<1>(crunched_fractal) ==
                Catch::Detail::Approx(1.6532446989801592));
        REQUIRE(std::get<2>(crunched_fractal) ==
                Catch::Detail::Approx(-1.939422755899791));

        // testing point 41/27
        crunched_fractal = crunch_test_julia.test_cruncher(
            -2.5 + 41 * z_real_delta, -1.5 + 27 * z_ima_delta, 100);
        REQUIRE(std::get<0>(crunched_fractal) == 12);
        REQUIRE(std::get<1>(crunched_fractal) ==
                Catch::Detail::Approx(-2.825685311882377));
        REQUIRE(std::get<2>(crunched_fractal) ==
                Catch::Detail::Approx(-2.588375399038022));
    }

    SECTION("Testing Julia Set 10x10, Bailout 10, -2.5 - 1.0 -1.5 - 1.5")
    {
        std::vector<int> result = {
            0,  0, 0, 0,  1,  1,  1,  1,  1,  1,  0, 0, 0, 1,  1,  1,  1,
            1,  1, 1, 0,  0,  0,  1,  1,  2,  2,  3, 3, 2, 0,  0,  1,  1,
            2,  3, 3, 10, 7,  4,  0,  0,  1,  2,  4, 7, 7, 10, 10, 9,  0,
            0,  1, 4, 10, 10, 10, 10, 10, 10, 0,  0, 1, 3, 10, 10, 10, 10,
            10, 8, 0, 0,  1,  2,  2,  3,  6,  10, 4, 3, 0, 0,  0,  1,  2,
            2,  2, 3, 2,  2,  0,  0,  0,  1,  1,  1, 1, 1, 1,  1};

        std::vector<int> escape;

        fill_test_buffer(escape, -2.5, 1.0, -1.5, 1.5, 10, 10, 10,
                         crunch_test_julia);

        std::pair<std::vector<int>::iterator, std::vector<int>::iterator>
            first_mismatch;
        // search for first mismatch
        first_mismatch =
            std::mismatch(result.begin(), result.end(), escape.begin());
        // okay if no mismatch
        REQUIRE(first_mismatch.first == result.end());
        REQUIRE(first_mismatch.second == escape.end());
    };
}

TEST_CASE(
    "Test computation of complex numbers and iteration count for Burning Ship "
    "Set",
    "[computation][burning_ship_set]")
{
    // creating a mock like object to be able to test computation. No need for
    // a real buffer or FractalParameters object
    constants::fracbuff b;
    std::shared_ptr<FractalParameters> params =
        std::make_shared<FractalParameters>();
    params->set_type = constants::FRACTAL::BURNING_SHIP;

    FractalcruncherMock crunch_test_bship(b, params);

    SECTION(
        "Test some single point computations with the Burning Ship Set, "
        "z1(-2.5-1.5j) - z2(1.0+1.5j)")
    {
        double z_real_delta = (1.0 - (-2.5)) / 50;
        double z_ima_delta = (1.5 - (-1.5)) / 50;

        // testing point 10/15
        auto crunched_fractal = crunch_test_bship.test_cruncher(
            -2.5 + 10 * z_real_delta, -1.5 + 15 * z_ima_delta, 100);
        REQUIRE(std::get<0>(crunched_fractal) == 2);
        REQUIRE(std::get<1>(crunched_fractal) == Catch::Detail::Approx(-3.0672));
        REQUIRE(std::get<2>(crunched_fractal) == Catch::Detail::Approx(2.7696));

        // testing point 33/25
        crunched_fractal = crunch_test_bship.test_cruncher(
            -2.5 + 33 * z_real_delta, -1.5 + 25 * z_ima_delta, 100);
        REQUIRE(std::get<0>(crunched_fractal) == 100);
        REQUIRE(std::get<1>(crunched_fractal) ==
                Catch::Detail::Approx(-0.163324958071));
        REQUIRE(std::get<2>(crunched_fractal) == Catch::Detail::Approx(0));

        // testing point 41/27
        crunched_fractal = crunch_test_bship.test_cruncher(
            -2.5 + 41 * z_real_delta, -1.5 + 27 * z_ima_delta, 100);
        REQUIRE(std::get<0>(crunched_fractal) == 100);
        REQUIRE(std::get<1>(crunched_fractal) ==
                Catch::Detail::Approx(0.469227595221));
        REQUIRE(std::get<2>(crunched_fractal) ==
                Catch::Detail::Approx(0.549612279558));
    }

    SECTION("Testing Burning Ship Set 10x10, Bailout 10, -2.5 - 1.0 -1.5 - 1.5")
    {
        std::vector<int> result = {
            0,  0,  0,  0,  1,  1,  1,  1,  2,  3,  0,  0,  0,  1,  2,  8, 2,
            2,  4,  10, 0,  0,  0,  2,  3,  10, 10, 10, 10, 8,  0,  0,  2, 2,
            10, 10, 10, 10, 10, 4,  0,  0,  3,  6,  10, 10, 10, 10, 10, 3, 0,
            0,  10, 10, 10, 10, 10, 10, 10, 3,  0,  0,  2,  2,  2,  3,  5, 10,
            10, 2,  0,  0,  1,  1,  2,  2,  2,  4,  4,  2,  0,  0,  0,  1, 1,
            1,  2,  2,  2,  1,  0,  0,  0,  1,  1,  1,  1,  2,  1,  1};

        std::vector<int> escape;

        fill_test_buffer(escape, -2.5, 1.0, -1.5, 1.5, 10, 10, 10,
                         crunch_test_bship);

        std::pair<std::vector<int>::iterator, std::vector<int>::iterator>
            first_mismatch;
        // search for first mismatch
        first_mismatch =
            std::mismatch(result.begin(), result.end(), escape.begin());
        // okay if no mismatch
        REQUIRE(first_mismatch.first == result.end());
        REQUIRE(first_mismatch.second == escape.end());
    };
}

TEST_CASE("Test computation of continuous index", "[computation]")
{
    // creating a mock like object to be able to test computation. No need for
    // a real buffer or FractalParameters object
    constants::fracbuff b;
    std::shared_ptr<FractalParameters> params =
        std::make_shared<FractalParameters>();
    params->set_type = constants::FRACTAL::MANDELBROT;
    FractalcruncherMock crunch_test_cindex(b, params);

    SECTION("Default index of its 2, -3.0672, 2.7696")
    {
        params->col_algo = constants::COL_ALGO::ESCAPE_TIME;
        auto it_object = crunch_test_cindex.test_iterfactory(2, -3.0672, 2.7696);
        REQUIRE(it_object.default_index == 2);
        REQUIRE(it_object.continous_index == 0);
    }

    SECTION("Default index of its 2, -3.0672, 2.7696 using ESCAPE_TIME_2")
    {
        params->col_algo = constants::COL_ALGO::ESCAPE_TIME_2;
        auto it_object = crunch_test_cindex.test_iterfactory(2, -3.0672, 2.7696);
        REQUIRE(it_object.default_index == 2);
        REQUIRE(it_object.continous_index == 0);
    }

    SECTION("Default index of its 2, -3.0672, 2.7696 using CONTINUOUS_BERN")
    {
        params->col_algo = constants::COL_ALGO::CONTINUOUS_BERN;
        auto it_object = crunch_test_cindex.test_iterfactory(2, -3.0672, 2.7696);
        REQUIRE(it_object.default_index == 2);
        REQUIRE(it_object.continous_index == 0);
    }

    SECTION("Continuous index of its 2, -3.0672, 2.7696")
    {
        params->col_algo = constants::COL_ALGO::CONTINUOUS_SINE;
        auto it_object = crunch_test_cindex.test_iterfactory(2, -3.0672, 2.7696);
        REQUIRE(it_object.default_index == 2);
        REQUIRE(it_object.continous_index ==
                Catch::Detail::Approx(2.758021706608733));
    }

    SECTION("Default index of its 55, 2.3637850846657784, -2.264372597523388")
    {
        params->col_algo = constants::COL_ALGO::ESCAPE_TIME;
        auto it_object = crunch_test_cindex.test_iterfactory(
            55, 2.3637850846657784, -2.264372597523388);
        REQUIRE(it_object.default_index == 55);
        REQUIRE(it_object.continous_index == 0);
    }

    SECTION(
        "Default index of its 55, 2.3637850846657784, -2.264372597523388 using "
        "ESCAPE_TIME_2")
    {
        params->col_algo = constants::COL_ALGO::ESCAPE_TIME_2;
        auto it_object = crunch_test_cindex.test_iterfactory(
            55, 2.3637850846657784, -2.264372597523388);
        REQUIRE(it_object.default_index == 55);
        REQUIRE(it_object.continous_index == 0);
    }

    SECTION(
        "Default index of its 55, 2.3637850846657784, -2.264372597523388 using "
        "CONTINUOUS_BERN")
    {
        params->col_algo = constants::COL_ALGO::CONTINUOUS_BERN;
        auto it_object = crunch_test_cindex.test_iterfactory(
            55, 2.3637850846657784, -2.264372597523388);
        REQUIRE(it_object.default_index == 55);
        REQUIRE(it_object.continous_index == 0);
    }

    SECTION("Continuous index of its 55, 2.3637850846657784, -2.264372597523388")
    {
        params->col_algo = constants::COL_ALGO::CONTINUOUS_SINE;
        auto it_object = crunch_test_cindex.test_iterfactory(
            55, 2.3637850846657784, -2.264372597523388);
        REQUIRE(it_object.default_index == 55);
        REQUIRE(it_object.continous_index ==
                Catch::Detail::Approx(55.69450318630743));
    }
}

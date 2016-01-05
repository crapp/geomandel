// A Mandelbrot Set paired with GeoTIFF
// Copyright © 2015 Christian Rapp
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
#include <complex>

int mandel_cruncher(double x, double y, int bailout)
{
    std::cout << "Calculating [" << x << "/" << y << "]" << std::endl;
    int iterations = 0;
    double x0 = x;
    double y0 = y;
    while (x * x + y * y <= 4.0 && iterations < bailout) {
        double x_old = x;
        x = x * x - y * y + x0;
        y = 2 * x_old * y + y0;
        iterations++;
    }
    if (iterations < bailout)
        return iterations;
    return 0;
}

void prnt_buff(const std::vector<std::vector<int>> &buff)
{
    for (auto &v : buff) {
        for (auto &val : v) {
            std::cout << val;
        }
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
    int bailout = 1000;
    std::vector<std::vector<int>> mandelbuffer;

    double x = -2.5;
    double xl = -2.0;
    double xh = 1.0;
    double xrange = 100;
    double y = -1.5;
    double yl = -1.5;
    double yh = 1.5;
    double yrange = 200;

    mandelbuffer.assign(xrange, std::vector<int>());
    for (auto &v : mandelbuffer) {
        v.assign(yrange, 0);
    }

    for (int ix = 0; ix < xrange; ix++) {
        for (int iy = 0; iy <= yrange; iy++) {
            int its = mandel_cruncher(x, y, bailout);
            if (its > 0)
                mandelbuffer[ix][iy] = 1;
            // if (its == bailout) {
            // mandelbuffer[ix][iy] = 0;
            //} else {
            // mandelbuffer[ix][iy] = 1;
            //}
            y += ((yl * -1) + yh) / yrange;
        }
        x += ((xl * -1) + xh) / xrange;
        y = yl;
    }

    prnt_buff(mandelbuffer);
    return 0;
}

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

#ifndef MANDELPARAMS_H
#define MANDELPARAMS_H

#include <string>

struct MandelParameters {
    int xrange;
    double xdelta;
    double x;
    double xl;
    double xh;

    int yrange;
    double ydelta;
    double y;
    double yl;
    double yh;

    int bailout;

    int zoom;

    std::string image_base;

    MandelParameters() {}
    MandelParameters(int xrange, double xl, double xh, int yrange, double yl,
                     double yh, double bailout, int zoom, std::string image_base)
        : xrange(xrange),
          xl(xl),
          xh(xh),
          yrange(yrange),
          yl(yl),
          yh(yh),
          bailout(bailout),
          zoom(zoom),
          image_base(image_base)
    {
        this->x = xl;
        this->y = yl;

        this->xdelta = (xh - xl) / xrange;
        this->ydelta = (yh - yl) / yrange;
    }
};
#endif /* ifndef MANDELPARAMS_H */

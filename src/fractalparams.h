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

#ifndef FRACTALPARAMS_H
#define FRACTALPARAMS_H

#include <string>

#include "global.h"

struct FractalParameters {
    constants::FRACTAL set_type;

    unsigned int xrange;
    double xdelta;
    double x;
    double xl;
    double xh;

    unsigned int yrange;
    double ydelta;
    double y;
    double yl;
    double yh;

    double julia_real;
    double julia_ima;

    unsigned int bailout;

    unsigned int zoom;
    double xcoord;
    double ycoord;

    std::string image_base;
    std::string fractal_type;

    unsigned int cores;

    constants::COL_ALGO col_algo;

    FractalParameters() {}
    FractalParameters(constants::FRACTAL set_type, unsigned int xrange,
                      double xl, double xh, unsigned int yrange, double yl,
                      double yh, double julia_real, double julia_ima,
                      unsigned int bailout, unsigned int zoom, double xcoord,
                      double ycoord, std::string image_base,
                      std::string fractal_type, unsigned int cores,
                      constants::COL_ALGO col_algo)
        : set_type(set_type),
          xrange(xrange),
          xl(xl),
          xh(xh),
          yrange(yrange),
          yl(yl),
          yh(yh),
          julia_real(julia_real),
          julia_ima(julia_ima),
          bailout(bailout),
          zoom(zoom),
          xcoord(xcoord),
          ycoord(ycoord),
          image_base(image_base),
          fractal_type(fractal_type),
          cores(cores),
          col_algo(col_algo)
    {
        this->x = xl;
        this->y = yl;

        this->xdelta = (xh - xl) / xrange;
        this->ydelta = (yh - yl) / yrange;
    }
};
#endif /* ifndef FRACTALPARAMS_H */

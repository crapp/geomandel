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

#ifndef FRACTALZOOM_H
#define FRACTALZOOM_H

#include <cmath>

#include "global.h"

class Fractalzoom
{
public:
    Fractalzoom();

    /**
     * @brief Calculates new complex plane values based on zoom level and coordinates
     *
     * @param xh Real to
     * @param xl Real from
     * @param yh Imaginary to
     * @param yl Imaginary from
     * @param zoom Zoom level
     * @param xcoord X coordinate in the complex plane
     * @param ycoord Y coordinate in the complex plane
     */
    void calcalute_zoom_cpane(double &xh, double &xl, double &yh, double &yl,
                              double zoom, double xcoord, double ycoord,
                              unsigned int width, unsigned int height);

private:
    /* data */
};

#endif /* ifndef FRACTALZOOM_H */

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

#include "mandelzoom.h"

Mandelzoom::Mandelzoom() {}
void Mandelzoom::calcalute_zoom_cpane(double &xh, double &xl, double &yh,
                                      double &yl, int zoom, int xcoord,
                                      int ycoord, int width, int height)
{
    /*
     * Nehme bisheriges delta und teile das durch das zoom level. Rechne
     * neue xl xh ausgehende von xcoordinate und width. equivalent fuer y
     */

    // calculate real and imaginary delta
    double xdelta = (xh - xl) / width;
    double ydelta = (yh - yl) / height;

    // map image coordinates to complex plane
    double xcoord_cplane = xl + xcoord * xdelta;
    double ycoord_cplane = yl + ycoord * ydelta;

    // new delta derived from zoom
    double xdelta_zoom = xdelta / zoom;
    double ydelta_zoom = ydelta / zoom;

    // update complex plane values in relation to the coordinates
    xl = xcoord_cplane - (width / 2 * xdelta_zoom);
    xh = xcoord_cplane + (width / 2 * xdelta_zoom);

    yl = ycoord_cplane - (height / 2 * ydelta_zoom);
    yh = ycoord_cplane + (height / 2 * ydelta_zoom);
}

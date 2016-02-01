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

#ifndef BUFFWRITER_H
#define BUFFWRITER_H

#include "global.h"

class Buffwriter
{
public:
    Buffwriter(const constants::mandelbuff &buff);
    virtual ~Buffwriter();

    virtual void write_buffer() = 0;

protected:
    const constants::mandelbuff &buff;
};

#endif /* ifndef BUFFWRITER_H */

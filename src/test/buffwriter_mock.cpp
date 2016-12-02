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

#include "buffwriter_mock.h"

BuffwriterMock::BuffwriterMock(const constants::fracbuff &buff)
    : Buffwriter(buff)
{
}
BuffwriterMock::~BuffwriterMock() {}
void BuffwriterMock::write_buffer(){};

std::string BuffwriterMock::test_filename_patterns(
    const std::string &filename_pattern, const std::string &fractal_type,
    unsigned int bailout, unsigned int xrange, unsigned int yrange, double zoom,
    unsigned int cores, double xcoord, double ycoord, double z_real_min,
    double z_real_max, double z_ima_min, double z_ima_max)
{
    return this->out_file_name(filename_pattern, fractal_type, bailout, xrange,
                               yrange, zoom, cores, xcoord, ycoord, z_real_min,
                               z_real_max, z_ima_min, z_ima_max);
}

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

#include "mandelcruncher_mock.h"

MandelcruncherMock::MandelcruncherMock(
    constants::fracbuff &buff, const std::shared_ptr<FractalParameters> &params)
    : Mandelcruncher(buff, params)
{
}

MandelcruncherMock::~MandelcruncherMock() {}
void MandelcruncherMock::fill_buffer() {}
std::tuple<unsigned int, double, double> MandelcruncherMock::test_cruncher(
    double real, double ima, unsigned int bailout) const
{
    return this->crunch_mandel_complex(real, ima, bailout);
}

constants::Iterations MandelcruncherMock::test_iterfactory(unsigned int its,
                                                           double z_real,
                                                           double z_ima) const
{
    return this->iterations_factory(its, z_real, z_ima);
}

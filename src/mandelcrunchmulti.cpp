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

#include "mandelcrunchmulti.h"

Mandelcrunchmulti::Mandelcrunchmulti(
    constants::mandelbuff &buff, const std::shared_ptr<MandelParameters> &params)
    : Mandelcruncher(buff, params)
{
}

Mandelcrunchmulti::~Mandelcrunchmulti() {}
void Mandelcrunchmulti::fill_buffer()
{
    // a vector filled with futures. We will wait for all of them to be finished.
    std::vector<std::future<void>> futures;
    ctpl::thread_pool tpl(this->params->cores);

    // calculate the mandelbrot set line by line. Each line will be pushed to the
    // thread pool as separate job. The id parameter of the lambda function
    // represents the thread id.
    double x = this->params->x;
    double y = this->params->y;
    int iy = 0; /**< row to calculate*/
    for (auto &int_vec : buff) {
        futures.push_back(tpl.push([&int_vec, x, y, iy, this](int id) {
            double ypass = y;  // y value is constant for each row
            double xpass = x;
            if (iy != 0)
                ypass += this->params->ydelta * iy;
            for (unsigned int ix = 0; ix < this->params->xrange; ix++) {
                auto crunched_mandel = this->crunch_mandel_complex(
                    xpass, ypass, this->params->bailout);
                unsigned int its = std::get<0>(crunched_mandel);
                if (this->params->col_algo == constants::COL_ALGO::ESCAPE_TIME)
                    int_vec[ix] = this->iterations_factory(its, 0, 0);
                if (this->params->col_algo == constants::COL_ALGO::CONTINUOUS) {
                    double Zx = std::get<1>(crunched_mandel);
                    double Zy = std::get<2>(crunched_mandel);
                    int_vec[ix] = this->iterations_factory(its, Zx, Zy);
                }
                // increment xpass by xdelta
                xpass += this->params->xdelta;
            }
        }));
        iy++;
    }
    // make sure all jobs are finished
    for (const std::future<void> &f : futures) {
        f.wait();
    }
}

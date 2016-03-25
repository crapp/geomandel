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

#ifndef MANDELCRUNCHER_H
#define MANDELCRUNCHER_H

#include <tuple>
#include <cmath>

#include "global.h"
#include "fractalparams.h"

class Mandelcruncher
{
public:
    Mandelcruncher(constants::fracbuff &buff,
                   const std::shared_ptr<FractalParameters> &params);
    virtual ~Mandelcruncher();

    virtual void fill_buffer() = 0;

protected:
    constants::fracbuff &buff;
    const std::shared_ptr<FractalParameters> &params;

    /**
     * @brief Mandelbrot algorithm
     *
     * @param x
     * @param y
     * @param bailout
     *
     * @details
     * Returns the number of iterations it took to check whether the complex
     * number made of x and y (representing the imaginary and the real part of a
     * complex number) is within our 4.0 radius and therefor inside the
     * Mandelbrot. This means if the number of iterations is equal to bailout we
     * assume this complex number to be part of the mandelbrot set.
     *
     * @return Return number of iterations as well as Real and Imaginary Part of
     * the Complex Number.
     */
    std::tuple<unsigned int, double, double> crunch_mandel_complex(
        double x, double y, unsigned int bailout) const;
    /**
     * @brief Returns an Iterations object based on the coloring algorithm
     *
     * @param its Number of iterations
     * @param Zx Real part of the complex number
     * @param Zy Imaginary part of the complex number
     * @param col-algo Coloring algorithm
     *
     * @return Mandelbrot Set Buffer tuple
     */
    constants::Iterations iterations_factory(unsigned int its, double Zx,
                                             double Zy) const;

private:
};

#endif /* ifndef MANDELCRUNCHER_H */

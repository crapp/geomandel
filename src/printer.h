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

#ifndef PRINTER_H
#define PRINTER_H

#include <iostream>
#include <memory>

typedef std::ostream& (*manip)(std::ostream&);

class Printer
{
public:
    const bool quiet;

    Printer(const bool quiet);
};

// TODO: I don't like inlining, the question is how else can this be solved?
template <class T>
inline std::shared_ptr<Printer>& operator<<(std::shared_ptr<Printer>& prnt,
                                            const T& x)
{
    if (!prnt->quiet)
        std::cout << x;
    return prnt;
}
template <class T>
inline const std::shared_ptr<Printer>& operator<<(
    const std::shared_ptr<Printer>& prnt, const T& x)
{
    if (!prnt->quiet)
        std::cout << x;
    return prnt;
}
inline std::shared_ptr<Printer>& operator<<(std::shared_ptr<Printer>& prnt,
                                            manip manipulator)
{
    if (!prnt->quiet)
        std::cout << manipulator;
    return prnt;
}
inline const std::shared_ptr<Printer>& operator<<(
    const std::shared_ptr<Printer>& prnt, manip manipulator)
{
    if (!prnt->quiet)
        std::cout << manipulator;
    return prnt;
}

#endif /* ifndef PRINTER_H */

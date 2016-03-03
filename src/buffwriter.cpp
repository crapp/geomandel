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

#include "buffwriter.h"

Buffwriter::Buffwriter(const constants::mandelbuff &buff) : buff(buff) {}
Buffwriter::~Buffwriter() {}
std::string Buffwriter::out_file_name(const std::string &string_pattern,
                                      unsigned int bailout, unsigned int xrange,
                                      unsigned int yrange, unsigned int zoom,
                                      unsigned int cores, unsigned int xcoord,
                                      unsigned int ycoord, double z_real_min,
                                      double z_real_max, double z_ima_min,
                                      double z_ima_max)
{
    // this might represent the classic example of overengineering

    std::vector<std::unique_ptr<RegexpatternIface>> regex_patterns;
    regex_patterns.emplace_back(new Regexpattern<unsigned int>(bailout, "%b"));
    regex_patterns.emplace_back(new Regexpattern<unsigned int>(xrange, "%w"));
    regex_patterns.emplace_back(new Regexpattern<unsigned int>(yrange, "%h"));
    regex_patterns.emplace_back(new Regexpattern<unsigned int>(zoom, "%z"));
    regex_patterns.emplace_back(new Regexpattern<unsigned int>(cores, "%c"));
    regex_patterns.emplace_back(new Regexpattern<unsigned int>(xcoord, "%x"));
    regex_patterns.emplace_back(new Regexpattern<unsigned int>(ycoord, "%y"));
    regex_patterns.emplace_back(new Regexpattern<double>(z_real_min, "%Zr"));
    regex_patterns.emplace_back(new Regexpattern<double>(z_real_max, "%ZR"));
    regex_patterns.emplace_back(new Regexpattern<double>(z_ima_min, "%Zi"));
    regex_patterns.emplace_back(new Regexpattern<double>(z_ima_max, "%ZI"));

    std::string filename = string_pattern;

    for (const auto &p : regex_patterns) {
        p->parse_filename(filename);
    }

    return filename;
}

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

#include "csvwriter.h"

CSVWriter::CSVWriter(const constants::mandelbuff &buff,
                     const std::shared_ptr<MandelParameters> &params)
    : Buffwriter(buff), params(params)
{
}
CSVWriter::~CSVWriter() {}
void CSVWriter::write_buffer()
{
    // generate a csv file for iterations and modulus
    // TODO: Why not simply pass the MandelParameters object?
    std::string filename = this->out_file_name(
        this->params->image_base, this->params->bailout, this->params->xrange,
        this->params->yrange, this->params->zoom, this->params->cores,
        this->params->xcoord, this->params->ycoord, this->params->xl,
        this->params->xh, this->params->yl, this->params->yh);
    std::ofstream csv_stream_iter(filename + "_iterindex.csv",
                                  std::ofstream::out);
    std::ofstream csv_stream_modulus(filename + "_contindex.csv",
                                     std::ofstream::out);
    csv_stream_iter.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    csv_stream_modulus.exceptions(std::ofstream::failbit |
                                  std::ofstream::badbit);
    try {
        if (csv_stream_iter.is_open() && csv_stream_modulus.is_open()) {
            // use a stringstream as buffer for each new row
            std::stringstream ss_iter;
            std::stringstream ss_modulus;
            for (auto &v : this->buff) {
                for (auto &itobj : v) {
                    ss_iter << std::to_string(itobj.default_index) << ";";
                    ss_modulus << std::to_string(itobj.continous_index) << ";";
                }
                // write the buffers into the file streams and make sure the last
                // colon gets deleted.
                std::string iter_line = ss_iter.str();
                iter_line.erase(--(iter_line.end()));
                csv_stream_iter << iter_line << std::endl;
                ss_iter.str(std::string());

                std::string modulus_line = ss_modulus.str();
                modulus_line.erase(--(modulus_line.end()));
                csv_stream_modulus << modulus_line << std::endl;
                ss_modulus.str(std::string());
            }
        } else {
            std::cerr << "CSV Files not open" << std::endl;
        }
    } catch (const std::ofstream::failure &e) {
        std::cerr << "Error writing csv files" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

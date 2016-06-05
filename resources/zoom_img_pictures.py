# -*- coding: utf-8 -*-

# Script to create video frames
# Copyright © 2016 Christian Rapp
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


"""
File: zoom_img_pictures.py
Author: Christian Rapp
Email: 0x2a@posteo.org
Github: https://github.com/crapp
Description: Small script to generate multiple mandelbrot images from the same
coordinates with an increasing zoom factor
"""

import subprocess
import math
import glob
import numpy as np
import numpy.polynomial.polynomial as poly


class ZoomPoly(object):

    """
    ZoomPoly is a container for a zoom range and the polynomial coefficients
    derived from numpy polyfit.
    """

    def __init__(self, zoom_range, coefs):
        """ZoomPoly initializer

        :param zoom_range: Numpy array defining the zoom range
        :param coefs: Polynomial coefficients
        """
        self.zoom_range = zoom_range
        self.coefs = coefs


def main():
    """main"""
    zoom_base_value_low = np.arange(1, 100)
    zoom_base_value_2 = np.arange(100, 300)
    zoom_base_value_3 = np.arange(300, 600)
    zoom_base_value_4 = np.arange(600, 1500)
    zoom_base_value_5 = np.arange(1500, 2250)
    zoom_base_value_6 = np.arange(2250, 3000)

    # determine some polynomials of 3rd degree that we use to calculate the zoom
    # values
    x_1 = np.array([1, 10, 50, 100])
    y_1 = np.array([1, 2, 4, 10])
    coefs_1 = poly.polyfit(x_1, y_1, 2)
    zp1 = ZoomPoly(zoom_base_value_low, coefs_1)
    x_2 = np.array([100, 160, 230, 300])
    y_2 = np.array([10, 25, 60, 160])
    coefs_2 = poly.polyfit(x_2, y_2, 2)
    zp2 = ZoomPoly(zoom_base_value_2, coefs_2)
    x_3 = np.array([300, 380, 470, 600])
    y_3 = np.array([160, 400, 850, 20000])
    coefs_3 = poly.polyfit(x_3, y_3, 2)
    zp3 = ZoomPoly(zoom_base_value_3, coefs_3)
    x_4 = np.array([600, 750, 1100, 1500])
    y_4 = np.array([20000, 250000, 1000000, 2500000])
    coefs_4 = poly.polyfit(x_4, y_4, 3)
    zp4 = ZoomPoly(zoom_base_value_4, coefs_4)
    x_5 = np.array([1500, 1750, 2000, 2250])
    y_5 = np.array([2500000, 6000000, 17000000, 41000000])
    coefs_5 = poly.polyfit(x_5, y_5, 3)
    zp5 = ZoomPoly(zoom_base_value_5, coefs_5)
    x_6 = np.array([2250, 2500, 2750, 3000])
    y_6 = np.array([41000000, 170000000, 500000000, 1500000000])
    coefs_6 = poly.polyfit(x_6, y_6, 3)
    zp6 = ZoomPoly(zoom_base_value_6, coefs_6)

    zp_list = ([zp2, zp3])

    # rotation_deg = 0

    for zp in zp_list:
        for zoom in zp.zoom_range:
            zoomcalc = poly.polyval(zoom, zp.coefs)
            # values below are not good
            if zoomcalc < 1:
                zoomcalc = 1

            zoomargument = "--zoom=" + str(zoomcalc)
            base_rgb = "--rgb-base=" + "200,200,200"
            frame = str(zoom).rjust(5, '0')
            # coordinates are located somewhere in the mandelbrot Seahorse
            # Valley
            subprocess.run(["../../build/src/geomandel", "--multi=4", "--bailout=4096",
                            "--image-png", "--xcoord=501.70535", "--ycoord=450.64976",
                            "--image-file=" + frame + "_mandelvideo_1000_poly3_%x-%y_%zx",
                            base_rgb, "--rgb-freq=0.02,0.016,0.012", "--rgb-phase=4,2,1",
                            zoomargument])

            # rotate image
            # if (zoom > 300):
            # if (rotation_deg == 360.0):
            # rotation_deg = 0
            # rotation_deg += 0.1
            # rotation_deg = round(rotation_deg, 1)
            # print("Rotation deg " + str(rotation_deg))
            # if (rotation_deg % 1 == 0.0):
            # # fname = frame + "_mandelvideo_1000_poly7_" + "501.705-450.65_" + str(zoomcalc) + "x"
            # fname = glob.glob(frame + "*.png")
            # print(fname)
            # subprocess.run(["convert", fname[0],
            # "-rotate", str(int(rotation_deg)), fname[0]])


if __name__ == "__main__":
    main()

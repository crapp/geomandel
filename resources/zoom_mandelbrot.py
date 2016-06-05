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
coordinates with an increasing zoom factor. Frames are combined with ffmpeg to a
video file

$ ffmpeg -f image2 -framerate 30 \
-pattern_type glob -i '*.png' \
-c:v libx264 -pix_fmt yuv420p \
video.mp4

"""

import subprocess
import math
import numpy as np


def main():
    """main"""

    no_frames = np.arange(1, 1801)
    # calculate zoom for a specific frame
    # zoom_orig = 1.0 * math.pow((1 + 1.5 / 100), 1200)
    zoom_orig = 1.0
    print("Starting zoom at " + str(no_frames[0]) + " " + str(zoom_orig))
    # return
    zoom_current = zoom_orig
    print("Generating frames for zooming in")
    for f in no_frames:
        if f != 1:
            zoomcalc = zoom_current + (zoom_current * 0.015)
            zoom_current = zoomcalc
        else:
            zoomcalc = zoom_orig
    zoomargument = "--zoom=" + str(zoomcalc)
    base_rgb = "--rgb-base=" + "200,200,200"
    # generate frame numbers
    frame = str(f).rjust(5, '0')
    # coordinates are located somewhere in the mandelbrot Seahorse
    # Valley
    subprocess.run(["../../build/src/geomandel", "--multi=4", "--bailout=4096",
                    "--image-png", "--xcoord=501.705349998", "--ycoord=450.64976",
                    "--image-file=" + frame + "_mandelvideo_2100frames_1.5pct_%x-%y_%zx",
                    base_rgb, "--rgb-freq=0.02,0.016,0.012", "--rgb-phase=4,2,1",
                    zoomargument])

    decay_rate = (math.pow(zoom_current, 1 / 300) - 1) * 100
    decay_rate = (
        math.pow((1.0 * math.pow((1 + 1.5 / 100), 1800)), 1 / 300) - 1) * 100
    # this one I got from playing around :/
    decay_rate = 0.085456 * 100
    print("Decay rate: " + repr(decay_rate))
    print("generating frames for zooming out")
    no_frames_out = np.arange(1, 301)
    for fr in no_frames_out:
        zoomcalc = zoom_current - ((zoom_current / 100) * decay_rate)
        zoom_current = zoomcalc
        print("Zoom value for decay frame " +
              str(fr) + " " + repr(zoom_current))
        if fr == 300:
            zoomcalc = 1.0

        zoomargument = "--zoom=" + str(zoomcalc)
        base_rgb = "--rgb-base=" + "200,200,200"
        frame = str(fr + 1800).rjust(5, '0')
        # coordinates are located somewhere in the mandelbrot Seahorse
        # Valley
        subprocess.run(["../../build/src/geomandel", "--multi=4", "--bailout=4096",
                        "--image-png", "--xcoord=501.705349998", "--ycoord=450.64976",
                        "--image-file=" + frame + "_mandelvideo_2100frames_out_%x-%y_%zx",
                        base_rgb, "--rgb-freq=0.02,0.016,0.012", "--rgb-phase=4,2,1",
                        zoomargument])


if __name__ == "__main__":
    main()

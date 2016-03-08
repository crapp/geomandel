# -*- coding: utf-8 -*-
"""
Small script to generate multiple mandelbrot images from the same coordinates
with an increasing zoom factor
"""
import subprocess
import math


def main():
    """main"""
    updown = True
    red = 255
    for zoom in range(1, 750):
        zoomcalc = math.floor(math.pow(zoom, 2) / 20.0)
        if zoomcalc == 0:
            zoomcalc = 1
        zoomargument = "--zoom=" + str(zoomcalc)
        base_rgb = "--rgb-base=" + str(red) + ",0,0"
        frame = str(zoom).rjust(5, '0')
        # coordinates are located in the Seahorse Valley
        subprocess.run(["../build/src/geomandel", "--bailout=2048", "--multi=4",
                        "--img-png", "--xcoord=484", "--ycoord=444",
                        "--image-file=" + frame + "_mandelvid750_x2_20", base_rgb,
                        "--rgb-freq=0,16,2", zoomargument])
        if updown:
            red -= 2
        else:
            red += 2
        if red < 0:
            updown = False
            red = 0
        if red > 255:
            updown = True
            red = 255

if __name__ == "__main__":
    main()

# -*- coding: utf-8 -*-
"""
Small script to generate multiple mandelbrot images from the same coordinates
with an increasing zoom factor
"""
import subprocess
import math


def main():
    """main"""
    for zoom in range(1, 500):
        zoomcalc = math.floor(math.pow(zoom, 2) / 20.0)
        zoomargument = "--zoom=" + str(zoomcalc)
        frame = str(zoom).rjust(5, '0')
        # coordinates are located in the Seahorse Valley
        subprocess.run(["src/geomandel", "--bailout=2048", "--multi=4",
                        "--img-png", "--xcoord=484", "--ycoord=444",
                        "--image-file=" + frame + "_mandelvid484_444",
                        "--rgb-freq=0,16,2", zoomargument])

if __name__ == "__main__":
    main()

# Geomandel

[![Build Status](https://travis-ci.org/crapp/geomandel.svg?branch=master)](https://travis-ci.org/crapp/geomandel)

## Introduction

geomandel is a command line application that allows you to calculate mandelbrot
fractals and visualize them using different image formats or export the
calculated data as csv.

The [Mandelbrot set](https://en.wikipedia.org/wiki/Mandelbrot_set), named after
Benoit Mandelbrot, is a [fractal](https://en.wikipedia.org/wiki/Fractal).
A fractal is for example a natural phenomenon or mathematical set that display
self-similarity at various scales.

One can define the Mandelbrot Set as the set of complex numbers (complex plane)
for whom the progression

![Mandelbrot set mathematical definition](https://crapp.github.io/geomandel/mandelbrot_set_equation.png "Mandelbrot set mathematical definition")

converges to. This is a rather complicated subset of the complex plane. If you
try to plot this set you will find areas that are similar to the main area. You
can zoom into this figure as much as you want. You will discover new formations as
well as parts with self similarity.

Here is a video showing a zoom into the seahorse valley.

LINK TO VIDEO

The coloring algorithm used in the video frames is called the Escape Time algorithm.
The application can generate images with a continuous coloring algorithm as well.

You may ask yourself how I came up with the name for this application. Well my initial
idea was to play with fractals and GeoTIFF to see what kind of effects I could achieve.
This is not yet implemented but still on my agenda :)

I also want to make you aware of the
[Mandelbrot project](https://github.com/willi-kappler/mandel-rust) a friend of
mine started. He uses Rust for his project and it motivated me to see how a C++11
solution would perform in comparison to his solution. I will provide some comparison
charts in the future.

## Setting up geomandel

In order to run the application on your system you either have to compile the
source code or use precompiled packages/installers.

### Source Code

To get the source code of the application
you may either download the [latest release](https://github.com/crapp/geomandel/releases/latest)
or you can check out the code from the [repository on github](https://github.com/crapp/geomandel).
Please use the master branch or one of the release branches.

In order to compile the source code on your machine the following requirements have to be met:
* cmake >= 3.2
* C++11 compliant compiler with proper ```std::regex``` support
    * gcc >= 4.9
    * clang >= 3.4
    * MSVC >= 14 (Visual Studio 2015)
    * MinGW >= 4.9
* [Simple and Fast Multimedia Library](http://www.sfml-dev.org/) for png/jpg support (optional)


#### Linux / OS X

Download and extract the source code. Enter the source code directory and use the
following commands to compile the application

```shell
# create a build directory and enter it
mkdir build
cd build
# run cmake to generate unix make files. Use -DUNIT_TEST=ON to build the
# unit tests application
cmake -DCMAKE_BUILD_TYPE=Release ../
# now compile the source code to create the geomandel executable. You can speed
# up compilation with the j option.
make
# optional run make install if you want to install the executable
make install
```

#### Windows

You can use the the cmake Visual Studio Solution generator to compile the
application.

```shell
# create build directory and enter it
mkdir build
cd build
# assuming you are using Visual Studio 2015 on a 64bit Windows installation
# please change these options so they suit your build environment
cmake -G"Visual Studio 14 2015 Win64" ../
```
Open the solution file and compile the application.

Another possibility is to use the cmake NMake generator.

```shell
# create build directory and enter it
mkdir build
cd build
# check the prefix path so it matches your Qt installation
cmake -G"NMake Makefiles" ../
# build the application
nmake
```
### Precompiled packages, Installer

Precompiled Linux packages are available for

* Archlinux (AUR)

## Usage

The command line application has sane default values for all options and you only
have to specify what kind of output you want to have. When the application is
started it will calculate the iteration count for all complex numbers in the complex
plane you defined. The iteration count can then be used to visualize the mandelbrot
set or the values can be exported to csv files. This is useful if you want to
test your own visualization algorithms.

### Command line options

geomandel uses the header only c++ command line parser [cxxopts](https://github.com/jarro2783)

The following command line options are available

```shell
$ geomandel --help
Usage:
  geomandel [OPTION...] - command line options

      --help              Show this help
  -m, --multi [=arg(=2)]  Use multiple cores
  -q, --quit              Don't write to stdout (This does not influence stderr)

 Mandelbrot options:

  -b, --bailout arg     Bailout value for the mandelbrot set algorithm
                        (default:1000)
      --creal-min arg   Real part minimum (default:-2.5)
      --creal-max arg   Real part maximum (default:1.0)
      --cima-min arg    Imaginary part minimum (default:-1.5)
      --cima-max arg    Imaginary part maximum (default:1.5)

 Image options:

      --image-file arg  Image file name pattern. You can use different printf
                        like '%' items interspersed with normal text for the
                        output file name. Have a look in the README for more
                        instructions. (default:geomandel)
  -w, --width arg       Image width (default:1000)
  -h, --height arg      Image height (default:1000)
      --img-pnm-bw      Write Buffer to PBM Bitmap
      --img-pnm-grey    Write Buffer to grey scale PGM
      --img-pnm-col     Write Buffer to PPM Bitmap
      --img-jpg         Write Buffer to JPG image
      --img-png         Write Buffer to PNG image
      --colalgo arg     Coloring algorithm 0->Escape Time, 1->Continuous
                        Coloring (default:0)
      --grey-base arg   Base grey color between 0 - 255 (default:55)
      --grey-freq arg   Frequency for grey shade computation (default:5)
      --rgb-base arg    Base RGB color as comma separated string
                        (default:255,0,0)
      --rgb-freq arg    Frequency for RGB computation as comma separated
                        string. You may use doubles but no negative values
                        (default:0,16,16)
      --rgb-phase arg   Phase for RGB computation as comma separated string
                        (default:0,2,4)
      --zoom arg        Zoom level. Use together with xcoord, ycoord
      --xcoord arg      Image X coordinate where you want to zoom into the
                        fractal
      --ycoord arg      Image Y coordinate where you want to zoom into the
                        fractal

 Export options:

  -p, --print           Print Buffer to terminal
      --csv             Export data to csv files

```

#### Mandelbrot Options

The complex plane is defined with the following options.

```
--creal-min   arg
--creal-max   arg
--cima-min    arg
--cima-max    arg
```
These are the default values ```z(-2.5, -1.5) z(1.0, 1.5)```

The bailout value `--bailout` defines the maximum amount of iterations used to
check whether a complex number is inside the Mandelbrot Set. The higher you set
this value the more precise the predictions of this algorithm will be. But
computation time will also be significantly higher. The escape criterion, shown in
the pseudo code example below, is not yet definable by a command line parameter
(defaults to 2.0)

```c++
while ( x*x + y*y <= ESCAPE  AND  iteration < BAILOUT  )
{
    xtemp = x*x - y*y + x0
    y = 2*x*y + y0
    x = xtemp

    iteration = iteration + 1
}
```

Computation speed can be reduced by using the ```--multi``` option. This will
distribute the workload on different threads. See the *performance* section for
more details.

#### Image Options

geomandel is able to generate different image formats from the image algorithms.
How this images are generated can be influenced by command line parameters.


##### File name

The default output file name is geomandel + appropriate image format file extension.
Fortunately you are able to use the `--image-file` command line option which allows
highly customizable file names.

You can use different printf like '%' items interspersed with normal text for the
output file name. These items will be replaced by the application with the values
you chose.

```
%b     Bailout value
%Zr    Complex number real minimum
%ZR    Complex number real maximum
%Zi    Complex number imaginary minimum
%ZI    Complex number imaginary maximum
%w     Image width
%h     Image height
%z     Zoom level
%x     Zoom x coordinate
%y     Zoom y coordinate
```

If you have set a bailout value of 2048 and are using the default values for the
complex plane you could a file name pattern like this

```
my_fractals_%bb_z(%Zr, %Zi)_z(%ZR, %ZI) -> my_fractals_2048b_z(-2.5, -1.5)_z(1.0, 1.5).[pgm|pbm|ppm|png|jpg]
```
Please note the naming scheme used for image files also applies for csv files
that will be generated when you use the ```--csv``` command line option.

##### Image size

Use the following parameters to control the image size

```
-w, --width
-h, --height
```

These parameters have a large impact on memory footprint and computation time

##### Image formats

The application can generate images in the [portable anymap format (PNM)](https://en.wikipedia.org/wiki/Netpbm_format).
The big advantage of this format it is easy to implement and doesn't need any external
libraries. The downside is large files, as geomandel does not use a binary format, and
low writing speed. You may also these images with a text editor of your choice
to what values geomandel actually wrote into the image file.

You can choose between `--img-pnm-bw` a simple black and white image, `--img-pnm-grey`
using grey scale to render the mandelbrot fractal and `--img-pnm-col` that generates
a RGB color image.

Additionally [SFML](http://www.sfml-dev.org/) can be used to generate jpg/png images.
These image formats use very little space on your disk and the library is quite fast.
You have to install the library and recompile geomandel if you want this kind of images.

##### Color Options

This is just a brief introduction into color command line options. See **Color**
for more information on this topic.

The `colalgo` parameter determines which coloring algorithm to use. Continuous
coloring will produce images without visible color bands. Computation time might
increase slightly though.
```
--colalgo arg     Coloring algorithm 0->Escape Time, 1->Continuous Coloring
```

Grey scale fractals are a nice alternative to colored ones. These parameters
control how the pgm images look like. Continuous coloring is not available for
grey scale images.
```
--grey-base arg   Base grey color between 0 - 255
--grey-freq arg   Frequency for grey shade computation

```

The RGB options apply to PPM as well as PNG and JPG images. The base color
(background color) is set with `--rgb-base`. The distribution of the colors in
the color spectrum for the escape time or continuous index can be defined with
`--rgb-freq` and `--rgb-base`. These options do different things depending on the
chosen coloring algorithm

```
--rgb-base arg    Base RGB color as comma separated string
--rgb-freq arg    Frequency for RGB computation as comma separated
                  string. You may use doubles but no negative values
--rgb-phase arg   Phase for RGB computation as comma separated string
```

## Color

The algorithms are well known and easy to implement. The more challenging part is
to draw beautiful fractals using different coloring algorithms. geomandel offers
two different color strategies, escape time based and continuous coloring. You
can use the `--colalgo` parameter too chose which one you want to apply.

### Escape Time

The Escape Time algorithm is the most common method to color fractals. This is
because the method is fast, easy to understand and there are a lot of implementations
out there one can build on.

The pseudo code example in [command line options](#Mandelbrot-Options) section
shows how the escape time is calculated. geomandel takes this value and calculates
a RGB tuple or a grey scale value with the following formula.

!["Escape Time Coloring Formula"](https://crapp.github.io/geomandel/escape_time_coloring.png)

#### Grey Scale

Lets have a look at grey scale PGM images first. You can provide a base grey
shade with `--grey-base` and a frequency value `--grey-frequency` which determines
how often sequences of grey shades repeat and their grading. This is best explained
with some images and plots.

The following Images and Plots were generated with `--grey-base=55`

!["Escape Time - Grey Scale Plots"](https://crapp.github.io/geomandel/color_escapetime_grey_plot.jpg)
!["Escape Time Fractals"](https://crapp.github.io/geomandel/color_escapetime_append_greymandel_2.jpg)

As you can see increasing the frequency will add better visual effects but also
worsen the problem of color banding. Grey scale PGM images can not be generated
with the continuous coloring algorithm.

#### RGB Images

Generating RGB images with the Escape Time algorithm works similar to grey scale
images. You now have to use `--rgb-base=R,G,B` to provide a base color and
`--rgb-freq=FREQ_R,FREQ_G,FREQ_B` to determine the frequencies. Setting a
frequency to **0** will leave the respective color component untouched.

The first three colorbars were created with a base color of `255,0,0` and a
RGB frequency of `0,2,0;0,8,0;0,16,0`. So only the green color component will
be changed.

![Escape Time colorbars](https://crapp.github.io/geomandel/escape_colorbar_1band.png)

The next three colorbars were created with the same base color and the same frequency
values for the green primary. Additionally I have added a frequency of 4 for the
blue primary.

![Escape Time colorbars two color components](https://crapp.github.io/geomandel/escape_colorbar_2band.png)

Here is what this values will look like as mandelbrot fractals.

![Escape time fractals](https://crapp.github.io/geomandel/escape_time_fractals_all_border.png)


Feel free to experiment with this values to get the result you want. There is a
jupyter notebook `EscapeTimeRGB` in the resources folder that makes it easy to
visualize different values for RGB fractals generated with the escape time
algorithm.

## Performance and Memory usage

Calculating the escape time for a Mandelbrot Set is costly and may consume large
amounts of RAM. The computation benefits greatly from parallelism and it is easy
to distribute the workload accordingly. You can do this with the `--multi`
Parameter. geomandel will create the amount of threads specified by the multi
Parameter and uses the modern and efficient thread pool library
[CTPL](https://github.com/vit-vit/CTPL) to distribute the workload.

### Benchmarks

![Performance Chart](https://crapp.github.io/geomandel/geomandel_benchmark.png "Performance Chart")

The chart shows how geomandel performs on different CPUs. The overhead seems to
be pretty small as doubling the number of threads nearly halves the time needed
for the computation.

While the Apple clang compiler generated binary performed as good as the gcc
binary on OS X (and even slightly better) the clang compiler on linux is
outperformed by gcc by a factor of 2:1.

### Performance breakdown

Calculating the mandelbrot set seems to be costly. But how much time is really
spend on computation? In order to answer this question it is necessary to
look at an application with a performance analyzing tool. Luckily we have
[perf](https://en.wikipedia.org/wiki/Perf_%28Linux%29) on Linux.

![Perf analyze graph](https://crapp.github.io/geomandel/perf_analyze.png "Perf analyze graph")

*Graph generated from perf data with gprof2dot*

Well as you can see ~97% of the applications cpu cycles were used to calculate the
mandelbrot set. So if you want to speed up this application this is the place to
start. I will try to maximize parallel computation with the use of GPUs in the
future.

### Memory Footprint

The amount of memory used by geomandel is mostly dependant from image size and
image format. The internal buffer that stores the results of the mandelbrot set
number cruncher is preallocated at the beginning of the application. Therefor it
is simple to calculate the minimum amount of free memory you need to run geomandel.
The internal Buffer stores the modulus of the complex numbers (double) and the
escape time (int32).

![Memory Footprint equation](https://crapp.github.io/geomandel/memory_footprint_equation.png "Memory Footprint equation")

As you can see in this equation you need around 12.6 MB for an image of the size
1024x1024. Of course this will be more in real life as I did not account for
[padding space](http://stackoverflow.com/a/937800/1127601) between data members
that is inserted by compilers to meet platform alignment requirements. In this
case 16 Bytes will be used meaning the application needs around 16 MB of free memory.

Lets see if valgrinds memory profiler massif is showing us the same values that
I just calculated

![Memory Footprint massif visualization](https://crapp.github.io/geomandel/geomandel_massif_memory_pngout.png "Memory profile")
*Memory profile derived from valgrinds massif tool*

Directly at the beginning the memory for the internal buffer is acquired and this
is close to my 16 MB assumption. At the end you can see another sharp rise in
memory usage. This is because I used SFML to generate a PNG image. SFML needs a
uint8_t data structure consisting of 4 bytes per pixel (RGBA). This leads to another
12 MB of memory acquired by geomandel. That comes to a total of ~28 MB of memory.
Please note this extra memory is only acquired when you use SFML (PNG/JPG).

## Development

Brief overview over the development process.

## Repositories
The [github repository](https://github.com/crapp/geomandel) of geomandel has
several different branches.

* master      : Main development branch. Everything in here is guaranteed to
compile and is tested. This is the place for new features and bugfixes. Pull requests welcome.
* dev         : Test branch. May not compile.
* release-x.x : Branch for a release. Only bugfixes are allowed here. Pull requests welcome.
* gh-pages    : Special branch for static HTML content hosted by github.io.

### Coding standards

The source code is formatted with clang-format using the following configuration

```
Language                            : Cpp,
BasedOnStyle                        : LLVM,
AccessModifierOffset                : -4,
AllowShortIfStatementsOnASingleLine : false,
AlwaysBreakTemplateDeclarations     : true,
ColumnLimit                         : 81,
IndentCaseLabels                    : false,
Standard                            : Cpp11,
IndentWidth                         : 4,
TabWidth                            : 4,
BreakBeforeBraces                   : Linux,
CommentPragmas                      : '(^ IWYU pragma:)|(^.*\[.*\]\(.*\).*$)|(^.*@brief|@param|@return|@throw.*$)|(/\*\*<.*\*/)'
```

### Versioning

I decided to use [semantic versioning](http://semver.org/)

### Unit Tests

I am using the great [Catch](https://github.com/philsquared/Catch) automated test framework.
Currently computation and command line parameters are covered by unit tests. This
will be expanded on coloring algorithms in the future.

To build the unit tests application use the cmake option ```-DUNIT_TEST=ON```

The unit tester can be run with geomandel_test. This will run all test cases and
output the result. See the Catch framework documentation for command line options
you can use to run only specific test cases or change the test results are displayed.

### Continuous Integration

[Travis CI](https://travis-ci.org/) is used as continous integration service.
The geomandel github repository is linked to Travis CI. You can see the build
history for the master branch and all release branches on the [travis project page](https://travis-ci.org/crapp/geomandel).

Besides testing compilation on different systems and compilers I also run the
unit tests after the application was compiled successfully.

## ToDo

Have a look in the todo folder. I am using the [todo.txt](http://todotxt.com/)
format for my todo lists.

## License
```
Copyright (C) 2015, 2016 Christian Rapp

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
```

Have a look into the license sub folder where license files of the used libraries
are located.

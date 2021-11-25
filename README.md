Color Space Comparison Utility
==============================

Lost in color space? Want to know how much and what color space your display covers? Or just want to compare data got from few displays? This small utility may help you!

It compares color space given as input to several well-known color spaces and finds best match on coverage. Also it finds nearest standard white point to given one as part of input.

Compilation
-----------

This utility has no any external dependencies, just requires compiler with C++17 support. CMake is used as build system.

```bash
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
cmake --build .
```

Usage
-----

As input app expects 4 pairs of values: red, green, blue primaries and white point (x, y) coordinates. They can be parsed from EDID or passed manually.

For example, to get these values from EDID on Linux system, next command can be used (just replace `card0-eDP-1` with corresponding to desired display, `card0-HDMI-A-1` for example):

```bash
cat /sys/class/drm/card0-eDP-1/edid | edid-decode | grep -Po '(\d\.\d+), (\d\.\d+)' | tr -d ',' | ./colorspace
```

The next output is produced for my display:

```txt
colorspace coverage: 99.992% DCI-P3 (Display P3)
nearest white point: D65 6504K
```

Utility accepts few command line options which instruct it to compare given color space with few standard color spaces alongside finding the best match. See app help for details.

```bash
edid-decode < mbp-111-edid.bin | grep -Po '(\d\.\d+), (\d\.\d+)' | tr -d ',' | ./colorspace --ntsc --srgb
```

and this produces next output:

```txt
colorspace coverage: 99.13%     BT.601 (PAL/SECAM)
                     73.7714%   NTSC
                     100%       sRGB
nearest white point: D65 6504K
```

To specify desired color space manually, just pass 4 pairs of values separated by space:

```bash
echo "0.6796 0.3203 0.2646 0.6904 0.1503 0.0527 0.3095 0.3291" | ./colorspace
```

First three pairs are R, G, B (x,y) coordinates, the last one is white point. Also values can be entered interactively.

Afterwords
----------

This app was written just for fun. It's heart is polygon intersection algorithm which implementation is based on implementation found [here](https://github.com/abreheret/polygon-intersection), just slightly rewritten in pure modern C++.

List of known [color spaces][color-spaces-link] and [white points][white-points-link] where taken from Wikipedia, not everything from that tables is included into app, see the beginning of [`main.cpp`][main-cpp-link] to find the list of color spaces and white points actually supported by app.

App was written on Linux system and was not tested on anything else. But by its nature it should work anywhere.

[color-spaces-link]: https://en.wikipedia.org/wiki/RGB_color_spaces#RGB_color_space_specifications
[white-points-link]: https://en.wikipedia.org/wiki/Standard_illuminant#White_points_of_standard_illuminants
[main-cpp-link]: https://github.com/Kolcha/colorspace/blob/master/main.cpp

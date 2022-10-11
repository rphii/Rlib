# Rlib

Commonly used functions by rphii.

## Library
| Header                   | Source                   | Description                                |
|--------------------------|--------------------------|--------------------------------------------|
| [`rstr.h`](src/rstr.h)   | [`rstr.c`](src/rstr.c)   | basic, quite versatile string structure    |
| [`r2str.h`](src/r2str.h) | [`r2str.c`](src/r2str.c) | handy for fast formattet text file output  |
| [`rfile.h`](src/rfile.h) | [`rfile.c`](src/rfile.c) | basic file input / output                  |
| [`map.h`](src/map.h)     | [`map.c`](src/map.c)     | generic map / dicionary                    |

## Examples
- [`rstr_ex`](examples/rstr_ex.c) Basic usage of Rstr
- [`r2str_ex`](examples/r2str_ex.c) Basic usage of R2str
- [`writecomp`](examples/writecomp.c) Benchmark of writing to files
- [`map1_ex`](examples/map1_ex.c) Using Map with basic types
- [`map2_ex`](examples/map2_ex.c) Using Map with custon structure (Rstr)
